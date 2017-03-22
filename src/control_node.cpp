#include "ros/ros.h"
#include <iostream>
#include "nav_msgs/Odometry.h"
#include "mas_ros_hackathon/event_in.h"
#include "mas_ros_hackathon/input.h"
#include "geometry_msgs/Twist.h"
#include "string.h"
#include <cmath> 
#include <tf/transform_broadcaster.h>
#include "math.h"

int event_on = 0;
int input = 0;   // 1 - right | 2 - left
int event_status = 0;   // 0 - Idle | 1 - executing | 2 - Completed
ros::Publisher velocity_publisher;
double start_x, start_y, start_heading;
double start_time, timeout = 30, curr_time;

bool event_in_callback(mas_ros_hackathon::event_in::Request &req, mas_ros_hackathon::event_in::Response &res)
{
  char *event_in_cmd = (char*)malloc(sizeof(char) *100);
  strcpy(event_in_cmd, req.cmd.c_str());
  
  if(!strcmp(event_in_cmd, "e_start"))
  {
    event_on = 1;
    event_status = 0;
    ROS_INFO("Event turned ON");
  }
  else if(!strcmp(event_in_cmd, "e_stop"))
  {
    event_on = 0;
    event_status = 0;
    ROS_INFO("Event turned OFF");
  }
  else
  {
    ROS_WARN("Invalid input");
  }
  return true;
}

void odom_callback(const nav_msgs::Odometry::ConstPtr& msg)
{
  if(event_on & event_status == 1)
  {
    curr_time = ros::Time::now().toSec();
    double time_diff = curr_time - start_time;
    if(time_diff > timeout)
    {
      ROS_WARN("Timeout occured");
      geometry_msgs::Twist vel_msg;
      vel_msg.linear.x = 0.0;
      vel_msg.linear.y = 0.0;
      vel_msg.angular.z = 0.0;
      velocity_publisher.publish(vel_msg);
    }
    else
    {
      double x = msg->pose.pose.position.x;
      double y = msg->pose.pose.position.y;
      double theta = msg->pose.pose.orientation.z;


      tf::Quaternion q(msg->pose.pose.orientation.x, msg->pose.pose.orientation.y, msg->pose.pose.orientation.z, msg->pose.pose.orientation.w);
      tf::Matrix3x3 m(q);
      double roll, pitch, yaw;
      m.getRPY(roll, pitch, yaw);

      // ROS_INFO("Current Postion X:%f|Y:%f|Heading:%f ", x, y, yaw);

      geometry_msgs::Twist vel_msg;
      // ROS_INFO("Debug 1: %f",std::abs(std::abs(y) - std::abs(start_y)));

      double y_diff = std::abs(start_y) - std::abs(y);
      double x_diff = std::abs(start_x) - std::abs(x);

      double dist = sqrt(pow(x_diff,2) + pow(y_diff,2));

      if(dist > 1.0)
      {
        vel_msg.linear.x = 0.0;
        vel_msg.linear.y = 0.0;
        vel_msg.angular.z = 0.0;
        event_status = 2;
        ROS_INFO("Final position: X:%f|Y:%f|Heading:%f ", x, y, yaw);
      }
      else
      {
        vel_msg.linear.x = 0.0;

        if(input == 1)
          vel_msg.linear.y = 0.1;
        else if(input == 2)
          vel_msg.linear.y = -0.1;
        
        vel_msg.angular.z = 0.0;
      }
      
      velocity_publisher.publish(vel_msg);
    }
  }
  else if(event_on & event_status == 0)
  {
    double x = msg->pose.pose.position.x;
    double y = msg->pose.pose.position.y;
    double theta = msg->pose.pose.orientation.z;

    if(input == 1 || input == 2)
    {
      tf::Quaternion q(msg->pose.pose.orientation.x, msg->pose.pose.orientation.y, msg->pose.pose.orientation.z, msg->pose.pose.orientation.w);
      tf::Matrix3x3 m(q);
      double roll, pitch, yaw;
      m.getRPY(roll, pitch, yaw);

      // ROS_INFO("Current Postion X:%f|Y:%f|Heading:%f", x, y, yaw);

      start_x = x;
      start_y = y;
      event_status = 1;
      ROS_INFO("Start position set to X:%f|Y:%f", start_x, start_y);

      start_time = ros::Time::now().toSec();
    }

  }
  else if(event_on & event_status == 2)
  {
    // ROS_INFO("Event completed");
    input = 0;
  }

}

bool input_callback(mas_ros_hackathon::input::Request &req, mas_ros_hackathon::input::Response &res)
{
  char *input_cmd = (char*)malloc(sizeof(char) *100);
  strcpy(input_cmd, req.cmd.c_str());
  
  if(!strcmp(input_cmd, "left") && event_on)
  {
    input = 1;
    event_status = 0;
    res.status = "Success";
    ROS_INFO("Received input: %s", input_cmd);
  }
  else if(!strcmp(input_cmd, "right") && event_on)
  {
    input = 2;
    event_status = 0;
    res.status = "Success";
    ROS_INFO("Received input: %s", input_cmd);
  }
  else
  {
    res.status = "Looks like invalid input or Event is not enabled !"; 
    ROS_WARN("Looks like invalid input or Event is not enabled !");
  }

  return true;
}


int main(int argc, char *argv[])
{
  ros::init(argc, argv, "control_node");
  ros::Time::init();
  ros::NodeHandle n;

  ros::ServiceServer event_in_server, input_server;
  event_in_server = n.advertiseService("event_in", event_in_callback);
  input_server = n.advertiseService("input", input_callback);

  ros::Subscriber odom_subscriber;
  odom_subscriber = n.subscribe("/odom", 1000, odom_callback);

  velocity_publisher = n.advertise<geometry_msgs::Twist>("/cmd_vel", 3);

  ros::spin();

  return 0;
}