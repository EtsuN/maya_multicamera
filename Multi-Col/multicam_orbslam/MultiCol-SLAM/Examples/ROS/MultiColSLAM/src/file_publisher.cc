#include <ros/ros.h>
//#include <ros/Publisher.h>
#include <sensor_msgs/Image.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>

#include "MultiColSLAM/Image_msg.h"
#include <iostream>
#include <fstream> 
#include <iomanip>
#include <thread>
#include <mutex>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
using namespace std;
using namespace ros;
void LoadImagesAndTimestamps(
	const int startFrame,
	const int endFrame,
	const string path2imgs,
	vector<vector<string>> &vstrImageFilenames,
	vector<double> &vTimestamps);


int main(int argc, char **argv)
{
	ros::init(argc,argv,"Image_publisher");
	ros::NodeHandle nh;
	ros::Publisher pub = nh.advertise<MultiColSLAM::Image_msg>("/image_publish",1);
	ros::Rate loop_rate(10);
	const int endFrame = 759;
	const int startFrame = 25;
	
	vector<vector<string>> imgFilenames;
	vector<double> timestamps;
	LoadImagesAndTimestamps(startFrame, endFrame,
			"/home/e4e/Downloads/indoor_dynamic"
			, imgFilenames, timestamps);

	int nImages = imgFilenames[0].size();
	
	//read from folder and publish till last image.	       const int nrCams = static_cast<int>(imgFilenames.size());
	std::vector<cv::Mat> imgs(3);
	for (int ni = 0; ni < nImages; ni++)
	{
		// Read image from file
		std::vector<bool> loaded(3);
		for (int c = 0; c < 3; ++c)
		{
			imgs[c] = cv::imread(imgFilenames[c][ni], CV_LOAD_IMAGE_GRAYSCALE);
			if (imgs[c].empty())
			{
				cerr << endl << "Failed to load image at: " << imgFilenames[c][ni] << endl;
				return 1;
			}
		}
		MultiColSLAM::Image_msg msg;
		double tframe = timestamps[ni];
		msg.stamp = tframe;
		cv_bridge::CvImage(std_msgs::Header(),"mono8",imgs[0]).toImageMsg(msg.Img1);
		cv_bridge::CvImage(std_msgs::Header(),"mono8",imgs[1]).toImageMsg(msg.Img2);
		cv_bridge::CvImage(std_msgs::Header(),"mono8",imgs[2]).toImageMsg(msg.Img3);
		//msg.Img3 = img;
		// Pass the image to the SLAM system
		//MultiSLAM.TrackMultiColSLAM(imgs, tframe);
		//publish the image set and timestamp in a message
		pub.publish(msg);
		loop_rate.sleep();
	}
	
}


void LoadImagesAndTimestamps(const int startFrame,
	const int endFrame,
	const string path2imgs,
	vector<vector<string>> &vstrImageFilenames,
	vector<double> &vTimestamps)
{
	vstrImageFilenames.resize(3);
	ifstream fTimes;
	string strPathTimeFile = path2imgs + "/images_and_timestamps.txt";

	fTimes.open(strPathTimeFile.c_str());
	string line;


	int cnt = 1;
	while (std::getline(fTimes, line))
	{
		if (cnt >= startFrame && cnt < endFrame) // skip until startframe
		{
			std::istringstream iss(line);
			double timestamp;
			string pathimg1, pathimg2, pathimg3;
			if (!(iss >> timestamp >> pathimg1 >> pathimg2 >> pathimg3))
				break;
			vTimestamps.push_back(timestamp);
			vstrImageFilenames[0].push_back(path2imgs + '/' + pathimg1);
			vstrImageFilenames[1].push_back(path2imgs + '/' + pathimg2);
			vstrImageFilenames[2].push_back(path2imgs + '/' + pathimg3);
		}
		++cnt;

	}
}

