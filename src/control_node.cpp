#include "ros/ros.h"
#include <iostream>
#include "nav_msgs/Odometry.h"
#include "mas_ros_hackathon/event_in.h"
#include "string.h"

int event_on = 0;

bool event_in_callback(mas_ros_hackathon::event_in::Request &req, mas_ros_hackathon::event_in::Response &res)
{
  char *event_in_cmd = (char*)malloc(sizeof(char) *100);
  strcpy(event_in_cmd, req.cmd.c_str());
  
  if(!strcmp(event_in_cmd, "e_start"))
  {
    event_on = 1;
  }
  else
  {
    event_on = 0;
  }
  return true;
}


int main(int argc, char *argv[])
{
  ros::init(argc, argv, "control_node");
  ros::Time::init();
  ros::NodeHandle n;

  ros::ServiceServer event_in_server;
  event_in_server = n.advertiseService("event_in", event_in_callback);

  ros::spin();

  return 0;
}