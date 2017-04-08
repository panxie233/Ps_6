// wsn example program to illustrate LIDAR processing.  1/23/15

#include <ros/ros.h> //Must include this for all ROS cpp projects
#include <sensor_msgs/LaserScan.h>
#include <std_msgs/Float32.h> //Including the Float32 class from std_msgs
#include <std_msgs/Bool.h> // boolean message 


const double MIN_SAFE_DISTANCE = 1.0; // set alarm if anything is within 0.5m of the front of robot

// these values to be set within the laser callback
float ping_dist_in_front_=3.0; // global var to hold length of a SINGLE LIDAR ping--in front
int ping_index_= -1; // NOT real; callback will have to find this
double angle_min_=0.0;
double angle_max_=0.0;
double angle_increment_=0.0;
double range_min_ = 0.0;
double range_max_ = 0.0;
std_msgs::Bool laser_alarm_;

ros::Publisher my_publisher;


// really, do NOT want to depend on a single ping.  Should consider a subset of pings
// to improve reliability and avoid false alarms or failure to see an obstacle

void laserCallback(const sensor_msgs::LaserScan& laser_scan) {
        //for first message received, set up the desired index of LIDAR range to eval
        angle_min_ = laser_scan.angle_min;
        angle_max_ = laser_scan.angle_max;
        angle_increment_ = laser_scan.angle_increment;
        range_min_ = laser_scan.range_min;
        range_max_ = laser_scan.range_max;
        
        // what is the index of the ping that is straight ahead?
        ROS_INFO("the min/max angle is (%f, %f),  the min/max range is (%f, %f)", angle_min_, angle_max_, range_min_, range_max_);
        
        ping_index_ = (int)((0-angle_min_)/angle_increment_);
        ROS_INFO("ping index is %u", ping_index_);
        ping_dist_in_front_ = laser_scan.ranges[ping_index_];
        ROS_INFO("the straight ahead range distance is %f", ping_dist_in_front_);

        if(ping_dist_in_front_ < 1){
			laser_alarm_.data = true;}
		else{laser_alarm_.data = false;}
		
		ROS_INFO("After we try to publish");
		//ROS_INFO(laser_alarm_.data);
			
  }
        

int main(int argc, char **argv) {
    ros::init(argc, argv, "lidar_alarm"); //name this node
    ros::NodeHandle nh; 
    //create a Subscriber object and have it subscribe to the lidar topic
    ros::Subscriber lidar_subscriber = nh.subscribe("/scan", 1, laserCallback);
    ros::Publisher my_publisher = nh.advertise<std_msgs::Bool>("lidar_alarm", 1);	
    ros::Rate naptime(10.0);
    //ros::Publisher pub = nh.advertise<std_msgs::Bool>("lidar_alarm", 1);
    //laser_alarm_.data = false; 
 
   while(ros::ok())
   {
		ros::spinOnce(); //this is essentially a "while(1)" statement, except it
		my_publisher.publish(laser_alarm_); 
		naptime.sleep(); 
    }
    // forces refreshing wakeups upon new data arrival
    // main program essentially hangs here, but it must stay alive to keep the callback function alive
    return 0; // should never get here, unless roscore dies
}

