/*************************************************************************
 *  
 *  
 *  Copyright 2014  Adam Harmat (McGill University) 
 *                      [adam.harmat@mail.mcgill.ca]
 *                  Michael Tribou (University of Waterloo)
 *                      [mjtribou@uwaterloo.ca]
 *
 *  Multi-Camera Parallel Tracking and Mapping (MCPTAM) is free software:
 *  you can redistribute it and/or modify it under the terms of the GNU 
 *  General Public License as published by the Free Software Foundation,
 *  either version 3 of the License, or (at your option) any later
 *  version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *  MCPTAM is based on the Parallel Tracking and Mapping (PTAM) software.
 *  Copyright 2008 Isis Innovation Limited
 *  
 *  
 ************************************************************************/


//=========================================================================================
//
// Copyright 2012 Adam Harmat, McGill University
// adam.harmat@mail.mcgill.ca
//
//=========================================================================================

#include <mcptam/VideoSourceSingle.h>
#include <mcptam/SmallMatrixOpts.h>
#include <mcptam/Utility.h>
#include <mcptam/TaylorCamera.h>
#include <sensor_msgs/image_encodings.h>
#include <sensor_msgs/SetCameraInfo.h>
#include <TooN/TooN.h>
#include <TooN/se3.h>
#include <TooN/SVD.h>
#include <TooN/helpers.h>
#include <unsupported/Eigen/Polynomials>



using namespace TooN;

VideoSourceSingle::VideoSourceSingle(bool bGetPoseSeparately)
: mCVPtr(new cv_bridge::CvImage)
, mNodeHandlePriv("~")
, mbGetPoseSeparately(bGetPoseSeparately)
{
  ROS_INFO("VideoSourceSingle: Initializing");
  
  // Make sure we're using our own callback queue
  mNodeHandle.setCallbackQueue(&mCallbackQueue);
  mNodeHandlePriv.setCallbackQueue(&mCallbackQueue);
  mpImageTransport = new image_transport::ImageTransport(mNodeHandle);
  
  std::string imageTopic, infoTopic, poseTopic;
  
  mNodeHandlePriv.param<std::string>("cam_name", mCamName ,"camera"); 
  mNodeHandlePriv.param<std::string>("image_topic", imageTopic ,"image_raw"); 
  mNodeHandlePriv.param<std::string>("info_topic", infoTopic ,"camera_info"); 
  mNodeHandlePriv.param<std::string>("pose_topic", poseTopic ,"pose"); 
  mNodeHandlePriv.param<std::string>("set_info_topic", mSetInfoTopic ,"set_camera_info");
  //added rgb/ for the following two lines and also for service setcamerainfo
  mImageSub = mpImageTransport->subscribe(std::string(mCamName + "/" + imageTopic), 1, &VideoSourceSingle::ImageCallback, this);
  mInfoSub = mNodeHandle.subscribe<sensor_msgs::CameraInfo>(std::string(mCamName + "/" + infoTopic), 1, &VideoSourceSingle::InfoCallback, this); 
    
  if(mbGetPoseSeparately)
    mPoseSub = mNodeHandle.subscribe<geometry_msgs::Pose>(std::string(mCamName + "/" + poseTopic), 1, &VideoSourceSingle::PoseCallback, this); 
  
  ROS_INFO("VideoSourceSingle: Set up all subscriptions");
  
  // Acquire an image and get image size from it
  mbAcquiredImage = false;
  while(!mbAcquiredImage && ros::ok())
  {
    // Trigger ROS callbacks
    mCallbackQueue.callAvailable();
  }
  
  ROS_INFO("VideoSourceSingle: Got first image, extracting size");
  mirSize = CVD::ImageRef(mCVPtr->image.cols,mCVPtr->image.rows);
  
  // Acquire the calibration info
  mbAcquiredInfo = false;
  while(!mbAcquiredInfo && ros::ok())
  {
    mCallbackQueue.callAvailable(ros::WallDuration(1.0));
    ROS_INFO("Waiting for calibration info...");
  }
  
  // Don't need info after getting one message so just shut it down
  mInfoSub.shutdown();  
  ROS_INFO("VideoSourceSingle: Got calibration info");
  
  mirFullScaleSize = CVD::ImageRef(mirSize.x * mirBinning.x, mirSize.y * mirBinning.y);
  
  // Acquire pose info
  if(mbGetPoseSeparately)
  {
    mbAcquiredPose = false;
    while(!mbAcquiredPose && ros::ok())
    {
      mCallbackQueue.callAvailable();
    }
    
    // Don't need pose after getting one message so just shut it down
    mPoseSub.shutdown();
    ROS_INFO("VideoSourceSingle: Got pose");
  }
}

VideoSourceSingle::~VideoSourceSingle()
{
  mImageSub.shutdown();
  
  delete mpImageTransport;
}

// Called from the image subscriber, stores the image for later conversion
void VideoSourceSingle::ImageCallback(const sensor_msgs::ImageConstPtr& imageMsg)
{
  try
  {
    // Try a conversion
    mCVPtr = cv_bridge::toCvShare(imageMsg);
  }
  catch (cv_bridge::Exception& e)
  {
    ROS_ERROR("VideoSourceSingle: cv_bridge exception: %s", e.what());
    return;
  }
  
  mbAcquiredImage = true;

}

// Called from the info subscriber, fills out various member variables
void VideoSourceSingle::InfoCallback(const sensor_msgs::CameraInfo::ConstPtr& infoMsg)
{
  ROS_DEBUG("VideoSourceSingle: Setting Camera Parameters");
  
  bool bValidCalibration = (infoMsg->D.size() != 0);
    
  if(!bValidCalibration || infoMsg->distortion_model != "taylor")
  {
    ROS_INFO_STREAM("VideoSourceSingle: Camera does not have a valid Taylor model calibration. Using set of default values.");
    
    // The calibration is not a valid one, use the default set of parameters
    TooN::Vector<9> v9Params = Zeros;
    SaveParams(v9Params);
  }
  else
  {
    int binning_x = infoMsg->binning_x;
    int binning_y = infoMsg->binning_y;
    
    if(binning_x == 0)
      binning_x = 1;
      
    if(binning_y == 0)
      binning_y = 1;
      
    mirBinning = CVD::ImageRef(binning_x, binning_y);  
    mirCalibSize = CVD::ImageRef(infoMsg->width, infoMsg->height);
    
    // The parameters are:
    // 0 - a0 coefficient
    // 1 - a2 coefficient
    // 2 - a3 coefficient
    // 3 - a4 coefficient
    // 4 - image center xc
    // 5 - image center yc
    // 6 - affine transform param c
    // 7 - affine transform param d
    // 8 - affine transform param e
      
    double c = infoMsg->K[0];
    double d = infoMsg->K[1];
    double e = infoMsg->K[3];
    
    double xc = infoMsg->K[2];
    double yc = infoMsg->K[5];
    
    
    double a0 = infoMsg->D[0];
    double a2 = infoMsg->D[1];
    double a3 = infoMsg->D[2];
    double a4 = infoMsg->D[3];
    
    mv9Params = makeVector(a0,a2,a3,a4,xc,yc,c,d,e);
    
    if(!mbGetPoseSeparately)
    {
      TooN::Matrix<3> m3Rot;
      
      for(int j=0; j < 3; ++j)
      {
        for(int i=0; i < 3; ++i)
        {
          m3Rot(j,i) = infoMsg->R[j*3 + i];
        }
      }
      TooN::Vector<3> v3Trans = TooN::makeVector(infoMsg->P[3], infoMsg->P[7], infoMsg->P[11]);
    
      // Check the rotation matrix to see if it is a proper rotation    
      TooN::Matrix<3> m3Result = m3Rot * m3Rot.T();  // this should be close to identity
      TooN::Vector<3> v3Ones = Ones; //makeVector(1,1,1);
      TooN::Vector<3> v3Diff = v3Ones - m3Result*v3Ones;  // should be close to zero
    
      if(v3Diff * v3Diff > 1e-4)
      {
        ROS_WARN_STREAM("The rotation matrix inside the CameraInfo message is invalid!");
        ROS_WARN_STREAM("Perhaps you meant to get the camera pose separately but forgot to set the get_pose_separately parameter to true?");
        ROS_WARN_STREAM("Setting the rotation matrix to identity!");
        ROS_WARN_STREAM("If you are calibrating the camera, continue without worry. If you are not, things will be messed up.");
      
        m3Rot = TooN::Identity;
        v3Trans = Zeros;
        ROS_WARN_STREAM("VideoSourceSingle: Invalid rotation matrix. Defaulting to IDENTITY transformation.");
      }
      
      TooN::SE3<> se3Pose(TooN::SO3<>(m3Rot), v3Trans);
      
      // Need to invert the pose before storing since it is stored in the "usual" format in the
      // info message (ie pose of the camera IN another reference frame, such that x_r = P_c * x_c
      // where x_r is point in reference frame, P_c is camera's pose, x_c is point in camera frame)
      // whereas the KeyFrames where these poses end up store the poses in the inverse manner
      // (ie x_c = P_c * x_r)
      mse3Pose = se3Pose.inverse();
    }
    
    mbAcquiredInfo = true;
  }
}

// Called from the pose subscriber, saves recieved pose
void VideoSourceSingle::PoseCallback(const geometry_msgs::Pose::ConstPtr& poseMsg)
{
  mse3Pose = util::PoseMsgToSE3(*poseMsg);
  
  mbAcquiredPose = true;
}

// Acquires a new image from the camera, outputs it in greyscale and color RGB formats
bool VideoSourceSingle::GetAndFillFrameBWandRGB(ros::WallDuration timeout, CVD::Image<CVD::byte> &imBW, CVD::Image<CVD::Rgb<CVD::byte> > &imRGB)
{
  bool bSuccess = NewAcquisition(timeout);
  
  if(!bSuccess)
    return false;
    
  util::ConversionRGB(mCVPtr->image, imRGB);
  util::ConversionBW(mCVPtr->image, imBW);
  
  return true;
}

// Acquires a new image from the camera, outputs it in greyscale format
bool VideoSourceSingle::GetAndFillFrameBW( ros::WallDuration timeout, CVD::Image<CVD::byte> &imBW)
{
  bool bSuccess = NewAcquisition(timeout);
  
  if(!bSuccess)
    return false;
    
  util::ConversionBW(mCVPtr->image, imBW);
  
  return true;
}

bool VideoSourceSingle::GetAndFillFrameBW( ros::WallDuration timeout, CVD::Image<CVD::byte> &imBW, ros::Time& timestamp)
{
  bool bSuccess = NewAcquisition(timeout);
  
  if(!bSuccess)
    return false;
    
  util::ConversionBW(mCVPtr->image, imBW);
  timestamp = mCVPtr->header.stamp;
  
  return true;
}

// Acquire a new frame
bool VideoSourceSingle::NewAcquisition(ros::WallDuration timeout)
{
  mbAcquiredImage = false;
  ros::WallTime start = ros::WallTime::now();
  
  while(!mbAcquiredImage && ros::ok())
  {
    mCallbackQueue.callAvailable();
    
    if(timeout != ros::WallDuration(0) && (ros::WallTime::now() - start) > timeout)  // timed out
      break;
  }
  
  return mbAcquiredImage;
}

bool VideoSourceSingle::SaveParams(const TooN::Vector<9>& v9Params)
{
  ROS_INFO_STREAM("VideoSourceSingle: Saving camera parameters: " << v9Params);
    
  // Connect to calib info service
  ros::ServiceClient client = mNodeHandle.serviceClient<sensor_msgs::SetCameraInfo>(std::string(mCamName + "/" + mSetInfoTopic));
  TaylorCamera *new_cam = new TaylorCamera(v9Params,this->GetSize(),this->GetSize(),this->GetSize());
 //new_cam->SetParams(v9Params);
 //new_cam->RefreshParams();
  
  sensor_msgs::SetCameraInfo srv;
  srv.request.camera_info.width = mirSize.x;
  srv.request.camera_info.height = mirSize.y;
  srv.request.camera_info.distortion_model = "taylor";
  
  // The parameters are:
  // 0 - a0 coefficient
  // 1 - a2 coefficient
  // 2 - a3 coefficient
  // 3 - a4 coefficient
  // 4 - image center xc
  // 5 - image center yc
  // 6 - affine transform param c
  // 7 - affine transform param d
  // 8 - affine transform param e
  
  // Affine transform and image center params
  srv.request.camera_info.K[0] = v9Params[6];
  srv.request.camera_info.K[1] = v9Params[7];
  srv.request.camera_info.K[2] = v9Params[4];
  srv.request.camera_info.K[3] = v9Params[8];
  srv.request.camera_info.K[4] = 1;
  srv.request.camera_info.K[5] = v9Params[5];
  
  // Polynomial distortion coeffs
  srv.request.camera_info.D.resize(4);
  srv.request.camera_info.D[0] = v9Params[0];
  srv.request.camera_info.D[1] = v9Params[1];
  srv.request.camera_info.D[2] = v9Params[2];
  srv.request.camera_info.D[3] = v9Params[3];
  
  // The pose parts (R and P) of the message are not filled in.
  // This will erase any pose saved with this camera, but that's what we want:
  // if the camera intrinsics change, the extrinsics will need to be 
  // recalibrated anyway
      
  if(!(client.call(srv) && srv.response.success))
  {
    ROS_ERROR_STREAM("VideoSourceSingle: Failed to save calibration to camera, response: "<<srv.response.status_message);
    return false;
  }
  
  mv9Params = v9Params;  // save internally too
  ROS_INFO_STREAM("VideoSourceSingle: Saved calibration to camera, response: "<<srv.response.status_message);
  return true;
}

/*
// Fit a polynomial of specified degree to the given input vectors
// See header file for more details and the doxygen HTML page for a nicer view of the equation
Vector<> PolyFit_1(Vector<> vxX, Vector<> vxY, int nDegree)
{
  Vector<> v0Empty(0);
  
  if(vxX.size() != vxY.size())
    return v0Empty;
    
  if(nDegree < 1)
    return v0Empty;
    
  int nDimensions = vxX.size();
    
  // Uses the Vandermonde matrix method of fitting a least squares polynomial to a set of data
  Matrix<> mxVandermondeTrans(nDegree+1,nDimensions);
  
  mxVandermondeTrans[0] = Ones(nDimensions);
  mxVandermondeTrans[1] = vxX;
  
  for(int i=2; i <= nDegree; ++i)
  {
    mxVandermondeTrans[i] = mxVandermondeTrans[i-1] * vxX.as_diagonal();
  }
  
  // create the SVD decomposition
  SVD<> svd(mxVandermondeTrans.T());
  
  Vector<> a = svd.backsub(vxY);
  
  return a;
}

// Evalute a polynomial with given coefficients at a specified location
template<int Size, class Precision, class Base> double PolyVal_1(const Vector<Size, Precision, Base>& vxCoeff, double dEvalAt)
{
	// Polynomials are stored with the coefficient of zero in the first spot
  // This is the opposive of how matlab's polyval function takes the coefficients
	double val=0;
	for(int i=vxCoeff.size()-1; i > 0; i--)
	{
		val += vxCoeff[i];	
		val *= dEvalAt;
	}

	val += vxCoeff[0];
	return val;
}

// Find the inverse of the polynomial describing the Taylor camera model
// See the header and/or doxygen HTML file for more info
Vector<> FindInvPolyUsingRoots_1(const Vector<5>& v5PolyCoeffs, int nSpecifiedDegree, double dErrorLimit)
{
  // We're going to generate a series of closely spaced thetas, and find the corresponding rho for
  // each theta using a polynomial root finder in the Eigen package. After discrading all thetas
  // where no valid rho was found, a polynomial is fitted to the resulting data, keeping in mind
  // that the thetas are now the x values and the rhos are the y values. This polynomial is the
  // inverse of the one passed into the function, to a certain error tolerance.
  
  // Generate the vector of thetas
  double dThetaStart = -M_PI/2 + 0.001;
  double dThetaEnd = M_PI/2 - 0.001;
  double dThetaStep = 0.01;
  int nThetaNum = ceil((dThetaEnd-dThetaStart)/dThetaStep) + 1;
  
  std::vector<double> vThetaVector(nThetaNum);
  std::vector<double> vRhoVector(nThetaNum);
  int nDeleteNum = 0;

  vThetaVector[0] = dThetaStart;  
  for(unsigned i=1; i < vThetaVector.size(); ++i)
  {
    vThetaVector[i] = vThetaVector[i-1] + dThetaStep;
  }
   
  // Find rho from the theta vector by solving for roots of polynomial equation
  for(unsigned i=0; i < vRhoVector.size(); ++i)
  {
    Eigen::Matrix<double,5,1> polynomial;
    polynomial << v5PolyCoeffs[0], v5PolyCoeffs[1] - tan(vThetaVector[i]), v5PolyCoeffs[2], v5PolyCoeffs[3], v5PolyCoeffs[4];
   
    Eigen::PolynomialSolver<double,4> psolve( polynomial );
    std::vector<double> vRealRoots;
    psolve.realRoots( vRealRoots );
    
    for(int j=vRealRoots.size()-1; j >= 0; --j)
    {
      if(vRealRoots[j] < 0.0 || vRealRoots[j] > mdMaxRho)
        vRealRoots.erase(vRealRoots.begin() + j);
    }
    
    if(vRealRoots.size() != 1)  // Zero or more than one real root means theta outside range of model
    {
      vRhoVector[i] = -9999;  // mark with a recognizable value
      nDeleteNum++;
    }
    else
      vRhoVector[i] = vRealRoots[0];
  }
      
  // Create the vectors that will be fitted with a polynomial
  Vector<> vxTheta(nThetaNum - nDeleteNum);
  Vector<> vxRho(nThetaNum - nDeleteNum);
  std::vector<double> vError(nThetaNum - nDeleteNum);
  
  // Fill the vectors and compute mean of valid thetas
  int j = 0;
  mdThetaMean = 0;
  for(unsigned i=0; i < vRhoVector.size(); ++i)
  {
    if(vRhoVector[i] == -9999)
      continue;
      
    vxTheta[j] = vThetaVector[i];
    vxRho[j] = vRhoVector[i];
    
    mdThetaMean += vThetaVector[i];
    
    j++;
  }
  
  mdThetaMean /= vxTheta.size();
  
  // Now get standard deviation
  Vector<> vxThetaShifted = vxTheta - TooN::Ones(vxTheta.size()) * mdThetaMean;
  mdThetaStd = sqrt((vxThetaShifted * vxThetaShifted) / vxThetaShifted.size());
  
  // Center and scale the thetas to make sure the polynomial is well conditioned
  vxTheta = CenterAndScale(vxTheta, mdThetaMean, mdThetaStd);

  if(nSpecifiedDegree < 0)  // We should find the best polynomial fit (within limits)
  {
    int nDegree = 2;  // Start with 2nd order poly
    double dMaxError = 1e10;
    
    while(dMaxError > dErrorLimit && nDegree <= MAX_INV_DEGREE)
    {
      Vector<> vxInvCoeffs = PolyFit_1(vxTheta, vxRho, nDegree);
      
      // Evaluate the fit at each theta
      for(int i=0; i < vxTheta.size(); ++i)
      {
        double rho_fit = PolyVal_1(vxInvCoeffs, vxTheta[i]);
        vError[i] = fabs(vxRho[i] - rho_fit);
      }
      
      dMaxError = *(std::max_element(vError.begin(), vError.end()));
      
      if(dMaxError <= dErrorLimit)
        return vxInvCoeffs;
        
      ++nDegree;
    }
    
    ROS_ERROR_STREAM("Hit degree limit, max error was: "<<dMaxError<<" and the limit was: "<<dErrorLimit);
    
    // If we're here, we've hit the degree limit so return an empty
    // vector, which will indicate a bad fit
    Vector<> v0Empty(0);
    return v0Empty;
  
  }
  
  // Otherwise we're given a specified degree, so just find a fit and return the coeffs
  return PolyFit_1(vxTheta, vxRho, nSpecifiedDegree);
  
}

*/
