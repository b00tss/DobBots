// general includes
#include <iostream>
#include <ros/ros.h>
#include <stdlib.h> // math lib

// needed messages 
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/WaypointList.h>

//make a class to perform callback and publish'
class Flyrectangle{
	// attributes
	float X, Y, Z;
	float tol = 0.5;
	int side = 0;
	ros::NodeHandle nh; //node handle
	ros::Publisher pub = nh.advertise<mavros_msgs::WaypointList> ("/mavros/mission/waypoints", 100); 
	//publisher for a next waypoint
	ros::Subscriber subPosition; //subscriber on current position
	ros::Subscriber subWaypoint; //subscriber on already set waypoint
	
		// local version of the msgs
	mavros_msgs::WaypointList locallist;
	geometry_msgs::PoseStamped localpos;
	
	public:
	// Constructors
	Flyrectangle():nh() {
		//X = 0;	Y = 0;	Z = 0;
		subWaypoint = nh.subscribe("/mavros/mission/waypoints", 1, &Flyrectangle::updatemission, this);
		subPosition = nh.subscribe("/mavros/local_position/pose", 1, &Flyrectangle::determineaction, this);
	//	pubFlY = nh.advertise<mavros_msgs::WaypointList> ("/mavros_msgs/mission/waypoints", 100);
	};

//	Flyrectangle(float x, float y, float z){
//		X = x; Y = y; Z = z;
//	}

	//desctructor
	~Flyrectangle(); 

	//member fucntions
	void updatemission(const mavros_msgs::WaypointList& msgWay){
		//if empty then set to zero?? or auto fill? and publish
		locallist.current_seq = msgWay.current_seq;
		locallist.waypoints = msgWay.waypoints;
		//if not empty update local list for pub
		if (locallist.current_seq < 1){
			//// make list of four wapoints	
			locallist.waypoints[0].x_lat = 20;
			locallist.waypoints[0].y_long = 0;
			locallist.waypoints[0].z_alt = 2.5;

			locallist.waypoints[0].x_lat = 20;
			locallist.waypoints[0].y_long = 20;
			locallist.waypoints[0].z_alt = 2.5;

			locallist.waypoints[0].x_lat = 0;
			locallist.waypoints[0].y_long = 20;
			locallist.waypoints[0].z_alt = 2.5;
			
			locallist.waypoints[0].x_lat = 0;
			locallist.waypoints[0].y_long = 0;
			locallist.waypoints[0].z_alt = 2.5;
			
			// publish new list
			pub.publish(locallist);
		}
	}

	void determineaction(const geometry_msgs::PoseStamped& msgPose){
		// fill local position var update
		localpos.header = msgPose.header;
		localpos.pose = msgPose.pose;
		// check if diff between waypoint and pose is less then tol. if so add a waypoint
		float Xd, Yd, Zd;  
		Xd = localpos.pose.position.x - locallist.waypoints[0].x_lat;
		Yd = localpos.pose.position.y - locallist.waypoints[0].y_long;
		Zd = localpos.pose.position.z - locallist.waypoints[0].z_alt;
		if (Xd < tol && Yd < tol){
			if (side == 0 || side == 4){
				locallist.waypoints[3].x_lat = locallist.waypoints[2].x_lat + 20;
			}
			else if(side == 1){
				locallist.waypoints[3].y_long = locallist.waypoints[2].y_long + 20;
			} 
			else if(side == 2){
				locallist.waypoints[3].x_lat = locallist.waypoints[2].x_lat - 20;
			} 
			else if(side == 3){
				locallist.waypoints[3].y_long = locallist.waypoints[2].y_long - 20;
			} 
			
			//else{
				//throw exception.... fault
			//}

			side++;
			if (side == 5){
				side = 1;
			}
			// publish new way point list
			pub.publish(locallist);
		}
	}

};

int main(int argc, char** argv){
	//initialize the ROS system & node decl. 
	ros::init( argc, argv, "Flyrectangle");		
	ros::NodeHandle n;

	// make class object
	Flyrectangle flypath();

	// let ros take over
	ros::spin();
	
}
