/**
* This file is part of ORB-SLAM2.
*
* Copyright (C) 2014-2016 Raúl Mur-Artal <raulmur at unizar dot es> (University of Zaragoza)
* For more information see <https://github.com/raulmur/ORB_SLAM2>
*
* ORB-SLAM2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-SLAM2 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ORB-SLAM2. If not, see <http://www.gnu.org/licenses/>.
*/
//path2voc ./Examples/small_orb_omni_voc_9_6.yml  
//path2settings ./Examples/Lafida/Slam_Settings_indoor1.yaml 
//path2calib ./Examples/Lafida

#include<iostream>
#include<algorithm>
#include<fstream>
#include<chrono>
#include <sstream>

#include<ros/ros.h>
#include <cv_bridge/cv_bridge.h>
#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>

#include<opencv2/core/core.hpp>

#include"../../../include/cSystem.h"


#include "MultiColSLAM/Image_msg.h"

using namespace std;

class ImageGrabber
{
public:
    ImageGrabber(MultiColSLAM::cSystem* pSLAM):mpSLAM(pSLAM){}

    void GrabRGBD(const sensor_msgs::ImageConstPtr& msgRGB,const sensor_msgs::ImageConstPtr& msgD);
void GrabImage_file(const MultiColSLAM::Image_msg::ConstPtr &msg);

    MultiColSLAM::cSystem* mpSLAM;
};

int main(int argc, char **argv)
{
    ros::init(argc, argv, "MultiCamSLAM");
    ros::start();
	char voc[]="/home/e4e/workspace/multicam_orbslam/multicam_orbslam/MultiCol-SLAM/Examples/small_orb_omni_voc_9_6.yml";
	char settings[]="/home/e4e/workspace/multicam_orbslam/multicam_orbslam/MultiCol-SLAM/Examples/Lafida/Slam_Settings_indoor1.yaml";
	char calib[]="/home/e4e/workspace/multicam_orbslam/multicam_orbslam/MultiCol-SLAM/Examples/Lafida/";	
    //if(argc != 3)
    //{
     //   cerr << endl << "Usage: rosrun TWOCAM pathtovoc path_to_settings path_to_calib" << endl;        
    //    ros::shutdown();
    //    return 1;
    //}    

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    MultiColSLAM::cSystem SLAM(voc,settings,calib,true);

    ImageGrabber igb(&SLAM);

    ros::NodeHandle nh;
/*
    message_filters::Subscriber<sensor_msgs::Image> rgb_sub(nh, "/camera/rgb/image_raw", 1);
    message_filters::Subscriber<sensor_msgs::Image> depth_sub(nh, "camera/depth_registered/image_raw", 1);
    typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::Image, sensor_msgs::Image> sync_pol;
    message_filters::Synchronizer<sync_pol> sync(sync_pol(10), rgb_sub,depth_sub);
    sync.registerCallback(boost::bind(&ImageGrabber::GrabRGBD,&igb,_1,_2));
*/
	//file subscribe
	ros::Subscriber sub = nh.subscribe("/image_publish",10,&ImageGrabber::GrabImage_file,&igb);	
    ros::spin();

    // Stop all threads
    SLAM.Shutdown();

	{
		std::stringstream ss;
		ss << "ORBSLAM_KeyTraj_" << ros::Time::now() << ".txt";
		std::string ss_str = ss.str();

    	// Save camera trajectory
    	//SLAM.SaveKeyFrameTrajectoryTUM(ss_str.c_str());
    	SLAM.SaveMKFTrajectoryLAFIDA(ss_str.c_str());
	}

	{
		std::stringstream ss;
		ss << "ORBSLAM_Traj_" << ros::Time::now() << ".txt";
		std::string ss_str = ss.str();

    	// Save camera trajectory
    	//SLAM.SaveTrajectoryTUM(ss_str.c_str());
    	SLAM.SaveMKFTrajectoryLAFIDA(ss_str.c_str());
	}

    ros::shutdown();

    return 0;
}
void ImageGrabber::GrabImage_file(const MultiColSLAM::Image_msg::ConstPtr &msg)
{
	ROS_INFO("Image subscriber call back");
	cv_bridge::CvImageConstPtr cv_ptrRGB1,cv_ptrRGB2,cv_ptrRGB3;
	std::vector<cv::Mat> imgs(3);
	try{
		cv_ptrRGB1 = cv_bridge::toCvShare(msg->Img1,msg);
		imgs[0] = cv_ptrRGB1->image;
		cv_ptrRGB2 = cv_bridge::toCvShare(msg->Img2,msg);
		imgs[1] = cv_ptrRGB2->image;
		cv_ptrRGB3 = cv_bridge::toCvShare(msg->Img3,msg);	
		imgs[2] = cv_ptrRGB3->image;
	}
	catch(cv_bridge::Exception &e)
	{
		ROS_ERROR("cv bridge failure: %s",e.what());
	}
	//get time stamp 
	double timestamp = msg->stamp;
	mpSLAM->TrackMultiColSLAM(imgs,timestamp);
}
void ImageGrabber::GrabRGBD(const sensor_msgs::ImageConstPtr& msgRGB,const sensor_msgs::ImageConstPtr& msgD)
{
    // Copy the ros image message to cv::Mat.
    cv_bridge::CvImageConstPtr cv_ptrRGB;
    try
    {
        cv_ptrRGB = cv_bridge::toCvShare(msgRGB);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }

    cv_bridge::CvImageConstPtr cv_ptrD;
    try
    {
        cv_ptrD = cv_bridge::toCvShare(msgD);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }

    //mpSLAM->TrackRGBD(cv_ptrRGB->image,cv_ptrD->image,cv_ptrRGB->header.stamp.toSec());
    mpSLAM->TrackMultiColSLAM(cv_ptrRGB->image,cv_ptrRGB->header.stamp.toSec());
}


