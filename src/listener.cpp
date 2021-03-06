#include <signal.h>

#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Bool.h"

#include "continuous.h"

using namespace std;

void setup_sig_handler();
void sig_handler(int sig);

std_msgs::Bool feedback;

void Wait_speech_end(const std_msgs::Bool::ConstPtr& ack)
{
	feedback.data = ack->data;
}

decoder* decod;

int main(int argc, char **argv)
{
	// Setup signal handlers
	setup_sig_handler();
	
	string path_par, language, phon_dict;
	feedback.data = true;
	ros::init(argc, argv, "listener");
	ros::NodeHandle n;
	ros::Publisher ear_pub = n.advertise<std_msgs::String>("ear", 1000);
	ros::Subscriber feedback_mouth = n.subscribe("feedback_mouth", 1000, Wait_speech_end);

	decod = new decoder();
	
	ros::Rate loop_rate(10);
	
	while(ros::ok())
	{
		//Message
		std_msgs::String msg;
		
		msg.data = decod->recognize();
		if(feedback.data)	ear_pub.publish(msg);
		// feedback.data = false;

		loop_rate.sleep();
		
		ros::spinOnce();
		
		//if(msg.data == "Good Bye")	break;
	}

	return 0;
}

void setup_sig_handler()
{
	signal(SIGINT, &sig_handler);
	signal(SIGTSTP, &sig_handler);
	signal(SIGABRT, &sig_handler);
	signal(SIGTERM, &sig_handler);
}

void sig_handler(int sig)
{
	switch(sig)
	{
		case SIGINT:
			signal(SIGINT, SIG_IGN);
			break;
		case SIGTSTP:
			signal(SIGTSTP, SIG_IGN);
			break;
		case SIGABRT:
			signal(SIGABRT, SIG_IGN);
			break;
		case SIGTERM:
			signal(SIGTERM, SIG_IGN);
			break;
	}
	
	// cout << "Closing hardware nicely" << endl;
	ROS_INFO("Closing hardware nicely\n");
	
	decod->~decoder();

	exit(0);
}
