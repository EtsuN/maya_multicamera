#include "ros/ros.h"
#include "sensor_msgs/Image.h"
#include "message_filters/subscriber.h"
#include "message_filters/synchronizer.h"
#include "image_transport/image_transport.h"
#include "image_transport/subscriber_filter.h"
#include "message_filters/sync_policies/approximate_time.h"
//#include "syncnode/syncimg.h"
#include "MultiColSLAM/Image_msg.h"
#include "std_msgs/String.h"


#include "cv_bridge/cv_bridge.h"
#include "sensor_msgs/image_encodings.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"


using namespace message_filters;
using namespace sensor_msgs;
using namespace ros;
//using namespace syncnode;
using namespace image_transport;

class subs{

	NodeHandle nh;
	ros::Subscriber sub1,sub2;
	ros::Publisher pub1,pub2,pub;
	
	message_filters::Subscriber<Image> s1,s2;
	typedef sync_policies::ApproximateTime<Image,Image> ApproxTimePolicy;
	message_filters::Synchronizer<ApproxTimePolicy> *syn;
	image_transport::SubscriberFilter *isub1,*isub2;
	image_transport::ImageTransport *Itrans;
	public:
	subs()
	{
		sub1 = nh.subscribe("/camera1/fisheye/image_raw",1,&subs::callback1,this);
		sub2 = nh.subscribe("/camera2/fisheye/image_raw",1,&subs::callback2,this);
	//message_filters::Subscriber<syncnode::new_msg> s1(nh,"/test_publish1",10);
	//message_filters::Subscriber<syncnode::new_msg> s2(nh,"/test_publish2",10);
	//const ros::Subscriber c = s1.getSubscriber();
	//const std_msgs::String da = c.getTopic();
	//ROS_INFO_STREAM("Check :" << c.getTopic()); 
	//message_filters::Synchronizer<msyncpolicy> syn(msyncpolicy(10),s1,s2);
	//s1 = new message_filters::Subscriber<Image>(nh,"/camera1/color/image_raw",10);
	//s2 = new message_filters::Subscriber<Image>(nh,"/camera2/color/image_raw",10);
	//s1.subscribe(nh,"/camera1/color/image_raw",10);
	//s2.subscribe(nh,"/camera2/color/image_raw",10);
	//syn = new message_filters::Synchronizer<msyncpolicy>(msyncpolicy(10),s1,s2);
	//syn.connectInput(s1,s2);
	//syn->registerCallback(boost::bind(&subs::callback,this,_1,_2));
	//pub1 = nh.advertise<Image>("/syncimage_test1",5);
	//pub2 = nh.advertise<Image>("/syncimage_test2",5);
	pub = nh.advertise<MultiColSLAM::Image_msg>("/image_publish",1);
	syn = new message_filters::Synchronizer<ApproxTimePolicy>(ApproxTimePolicy(60));
	Itrans = new image_transport::ImageTransport(nh);
	isub1 = new image_transport::SubscriberFilter();
	isub2 = new image_transport::SubscriberFilter();
	isub1->subscribe(*Itrans,"/camera1/fisheye/image_raw",60);
	isub2->subscribe(*Itrans,"/camera2/fisheye/image_raw",60);
	syn->connectInput(*isub1,*isub2);
	syn->registerCallback(&subs::callback,this);
	//cv::namedWindow("Image 1");
	//cv::namedWindow("Image 2");
	cv::namedWindow("Image 11");
	//cv::namedWindow("Image 21");
	}
	void callback1(const ImageConstPtr &msg)
	{
		/*ROS_INFO("Mesage1 recv ");
		cv_bridge::CvImagePtr cv_ptr1;
	try{
	cv_ptr1 = cv_bridge::toCvCopy(msg);
	//cv_ptr1 = cv_bridge::toCvCopy(msg,sensor_msgs::image_encodings::BGR8);
	}
	catch(cv_bridge::Exception&)
	{ROS_INFO("Error in image conversion");}

	cv::imshow("Image 11",cv_ptr1->image);
	int key = cv::waitKey(3);*/
	/*static int img1_count=0;
	if(key == 's')
	{
		std::stringstream sstream;
		sstream << "/home/e4e/workspace/multicam_orbslam/0-"<<img1_count<<".jpg";
		ROS_ASSERT(cv::imwrite(sstream.str(),cv_ptr1->image));
		img1_count++;
		ROS_INFO("Saved img 0");
	}*/
	
	
	/*MultiColSLAM::Image_msg imgmsg;
	cv_bridge::CvImage(std_msgs::Header(),"8UC1",cv_ptr1->image).toImageMsg(imgmsg.Img1);
	//cv_bridge::CvImage(std_msgs::Header(),"mono8",cv_ptr1->image).toImageMsg(imgmsg.Img1);
	ros::Time s = ros::Time::now(); 	
	imgmsg.stamp = s.nsec;

	pub.publish(imgmsg);*/
	}
	void callback2(const ImageConstPtr &msg)
	{
		ROS_INFO("Mesage2 recv");
	/*	cv_bridge::CvImagePtr cv_ptr1;
	try{
	cv_ptr1 = cv_bridge::toCvCopy(msg);
	}
	catch(cv_bridge::Exception&)
	{ROS_INFO("Error in image conversion");}

	cv::imshow("Image 21",cv_ptr1->image);
	int key = cv::waitKey(3);
	static int img2_count=0;
	if(key == 's')
	{
		std::vector<int> compression_params;
		     compression_params.push_back( CV_IMWRITE_JPEG_QUALITY );
		         compression_params.push_back( 100 );

		std::stringstream sstream;
		sstream << "/home/e4e/workspace/multicam_orbslam/1-"<<img2_count<<".jpg";
		ROS_ASSERT(cv::imwrite(sstream.str(),cv_ptr1->image,compression_params));
		img2_count++;
		ROS_INFO_STREAM("Saved img1 "<<sstream.str());
	}
	*/
	}
	void callback(const ImageConstPtr &msg1,const ImageConstPtr &msg2)
{
	ROS_INFO("Sync call back");
	//syncnode::syncimg msg;
	/*msg.Img1.height = msg1->height;
	msg.Img1.width = msg1->width;
	msg.Img1.encoding = msg1->encoding;
	msg.Img1.is_bigendian = msg1->is_bigendian;
	msg.Img1.step = msg1->step;
	msg.Img1.data = msg1->data;
	msg.Img1.header = msg1->header;
	msg.Img2.height = msg2->height;
	msg.Img2.width = msg2->width;
	msg.Img2.encoding = msg2->encoding;
	msg.Img2.is_bigendian = msg2->is_bigendian;
	msg.Img2.step = msg2->step;
	msg.Img2.data = msg2->data;
	msg.Img2.header = msg2->header;*/


	cv_bridge::CvImagePtr cv_ptr1,cv_ptr2;
	try{
	cv_ptr1 = cv_bridge::toCvCopy(msg1);
	cv_ptr2 = cv_bridge::toCvCopy(msg2);
	}
	catch(cv_bridge::Exception&)
	{ROS_INFO("Error in image conversion");}

	cv::imshow("Image 1",cv_ptr1->image);
	cv::waitKey(1);
	cv::imshow("Image 2",cv_ptr2->image);
	cv::waitKey(1);
	//msg.Img2 = msg2;
	//pub1.publish(msg1);
	//pub2.publish(msg2);
	MultiColSLAM::Image_msg imgmsg;
	cv_bridge::CvImage(std_msgs::Header(),"8UC1",cv_ptr1->image).toImageMsg(imgmsg.Img1);
	cv_bridge::CvImage(std_msgs::Header(),"8UC1",cv_ptr2->image).toImageMsg(imgmsg.Img2);
	ros::Time s = ros::Time::now(); 	
	imgmsg.stamp = s.nsec;
	//imgmsg.Img1 = cv_ptr1->toImageMsg();
	//imgmsg.Img2 = cv_ptr2->toImageMsg();
	pub.publish(imgmsg);

}
	/*~subs()
	{
	delete s1;
	delete s2;
	delete syn;
	}*/

};


int main(int argc,char **argv)
{
	ros::init(argc,argv,"sub");
	subs sub1;
	//ros::NodeHandle nh;
	//message_filters::Subscriber<syncnode::new_msg> s1(nh,"/test_publish1",10);
	//message_filters::Subscriber<syncnode::new_msg> s2(nh,"/test_publish2",10);
	//typedef sync_policies::ApproximateTime<new_msg,new_msg> msyncpolicy;
	//message_filters::Synchronizer<msyncpolicy> syn(msyncpolicy(10),s1,s2);
	//syn.registerCallback(callback);

	ros::spin();
	return 0;
}
