#include "ros/ros.h"
#include <iostream>
#include "nav_msgs/Odometry.h"
#include "mas_ros_hackathon/event_in.h"
#include "mas_ros_hackathon/input.h"
#include "string.h"

int event_on = 0;
int input = 0;

bool event_in_callback(mas_ros_hackathon::event_in::Request &req, mas_ros_hackathon::event_in::Response &res)
{
  char *event_in_cmd = (char*)malloc(sizeof(char) *100);
  strcpy(event_in_cmd, req.cmd.c_str());
  
  if(!strcmp(event_in_cmd, "e_start"))
  {
    event_on = 1;
    ROS_INFO("Event turned ON");
  }
  else if(!strcmp(event_in_cmd, "e_stop"))
  {
    event_on = 0;
    ROS_INFO("Event turned OFF");
  }
  else
  {
    ROS_WARN("Invalid input");
  }
  return true;
}

bool input_callback(mas_ros_hackathon::input::Request &req, mas_ros_hackathon::input::Response &res)
{
  char *input_cmd = (char*)malloc(sizeof(char) *100);
  strcpy(input_cmd, req.cmd.c_str());
  
  if(!strcmp(input_cmd, "left") && event_on)
  {
    input = 1;
    res.status = "Success";
    ROS_INFO("Received input: %s", input_cmd);
  }
  else if(!strcmp(input_cmd, "right") && event_on)
  {
    event_on = 2;
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

  ros::spin();

  return 0;
}