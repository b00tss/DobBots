// general includes
#include <iostream>
#include <ros/ros.h>
#include <stdlib.h> // math lib
// needed messages 
#include <geometry_msgs/PoseStamped.h>


//make a class to perform callback and publish'
class Flyrectangle{
	// attributes
	float X, Y, Z;
	float tol = 2;
	int side = 0;
	ros::NodeHandle nh;
	//publisher for a next waypoint	
	ros::Publisher pub = nh.advertise<geometry_msgs::PoseStamped> ("/mavros/setpoint_position/local", 10); 
	ros::Subscriber subPosition; //subscriber on current position
	// local version of the msgs
	geometry_msgs::PoseStamped localpos;	
	geometry_msgs::PoseStamped localsetpoint;
	
	public:
	// Constructors
	Flyrectangle(ros::NodeHandle& n):nh(n) {
		std::cout<< "constructor called" << std::endl;
		X = 30; 
		Y = 30;
		Z = 4;
		localsetpoint.pose.position.x = 0;
		localsetpoint.pose.position.y = 0;
		localsetpoint.pose.position.z = Z;
//		localsetpoint.pose.orientation.x = 1;
//		localpos.pose.position.x = 0;
//		localpos.pose.position.y = 0;
		pub = nh.advertise<geometry_msgs::PoseStamped> ("/mavros/setpoint_position/local", 10); 
		pub.publish(localsetpoint);
		//subPosition = nh.subscribe("/mavros/local_position/pose", 1, &Flyrectangle::determineaction, this);
	};
	
	//desctructor
	//~Flyrectangle(); 
	
	// manual call for subscriber
	void subb(){	
		subPosition = nh.subscribe("/mavros/local_position/pose", 1, &Flyrectangle::determineaction, this);
	}

	void determineaction(const geometry_msgs::PoseStamped& msgPose){
		//std::cout<< "callback fucntion 'determinaction' reached" << std::endl;
		// fill local position var update
		localpos.header = msgPose.header;
		localpos.pose = msgPose.pose;
		
		// check if diff between waypoint and pose is less then tol. if so add a waypoint
		float Xd, Yd, Zd;  
//		std::cout << "received position x =" << localpos.pose.position.x << std::endl;	
//		std::cout << "received position y =" << localpos.pose.position.y << std::endl;
//		std::cout << "received position z =" << localpos.pose.position.z << std::endl;
		
//		std::cout << "set position x =" << localsetpoint.pose.position.x << std::endl;	
//		std::cout << "set position y =" << localsetpoint.pose.position.y << std::endl;
//		std::cout << "set position z =" << localsetpoint.pose.position.z << std::endl;
		
		Xd = std::abs(localpos.pose.position.x - localsetpoint.pose.position.x);
		Yd = std::abs(localpos.pose.position.y - localsetpoint.pose.position.y);
//		std::cout<< "Xd = " << Xd << "   "<< "Yd = " << Yd << std::endl;


		
		if (Xd < tol && Yd < tol){	
		std::cout << "side =" << side << std::endl;
			
			if (side == 1 ){
				localsetpoint.pose.position.y = localpos.pose.position.y + Y;
				localsetpoint.pose.orientation.y = 1;	
				localsetpoint.pose.orientation.x = 1;
			}
			else if (side == 2){
				localsetpoint.pose.position.x = localpos.pose.position.x - X;
				localsetpoint.pose.orientation.y = 1;	
				localsetpoint.pose.orientation.x = 0;
			}
			else if (side == 3){
				localsetpoint.pose.position.y = localpos.pose.position.y - Y;
				localsetpoint.pose.orientation.y = -1;	
				localsetpoint.pose.orientation.x = 1;
			}
			else if (side == 4){
				localsetpoint.pose.position.x = localpos.pose.position.x + X;
				localsetpoint.pose.orientation.y = 0;	
				localsetpoint.pose.orientation.x = 1;
			};

			side++;
			if(side == 5){
				side = 1;
			};
			
			// publish new set point
		//	pub.publish(localsetpoint);
		//	std::cout<< "new setpoint set" << std::endl;
		}
		else {
		//	std::cout << "set point is not reached yet" << std::endl;
		}
	
	
	pub.publish(localsetpoint);
		
	}

};

int main(int argc, char** argv){
	//initialize the ROS system & node decl. 
	ros::init( argc, argv, "Flyrectangle");		
	ros::NodeHandle n;
	ros::Rate loop_rate(5);

	// make class object
	Flyrectangle flypath(n);
	std::cout << "Flyrectangle object made" << std::endl;
	// let ros take over
	while (ros::ok()){
		flypath.subb();
//		flypath.pubb();	
		loop_rate.sleep();
		ros::spinOnce();
	}
}
