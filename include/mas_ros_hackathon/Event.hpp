#include "ros/ros.h"
#include <iostream>
#include "nav_msgs/Odometry.h"
#include "mas_ros_hackathon/event_in.h"
#include "mas_ros_hackathon/input.h"
#include "mas_ros_hackathon/event_out.h"
#include "geometry_msgs/Twist.h"
#include "string.h"
#include <cmath> 
#include <tf/transform_broadcaster.h>
#include "math.h"

class Event {
public:
  double distance, x_vel, y_vel, ang_vel;
  ros::ServiceServer event_in_server; 
  ros::ServiceServer input_server;
  ros::Subscriber odom_subscriber;
  ros::Publisher velocity_publisher;
  ros::Publisher status_publisher;
  Event(ros::NodeHandle n);
  bool event_in_callback(mas_ros_hackathon::event_in::Request &req, mas_ros_hackathon::event_in::Response &res);
  void odom_callback(const nav_msgs::Odometry::ConstPtr& msg);
  bool input_callback(mas_ros_hackathon::input::Request &req, mas_ros_hackathon::input::Response &res);
  double timeout;
private:
  int event_on;
  int input;   // 1 - right | 2 - left
  int event_status;   // 0 - Idle | 1 - executing | 2 - Completed
  double start_x, start_y, start_heading;
  double start_time, curr_time;
  void publish_cmd_vel(double x_vel, double y_vel, double angular_vel);
  void convert_to_rpy(double orientation_x, double orientation_y, double orientation_z, double orientation_w);
  double pitch,roll,yaw;
};