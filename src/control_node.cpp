#include "ros/ros.h"
#include "mas_ros_hackathon/Event.hpp"


int main(int argc, char *argv[])
{
  ros::init(argc, argv, "control_node");
  ros::Time::init();
  ros::NodeHandle n;

  Event event_obj(n);
  event_obj.timeout = 30;

  ros::param::getCached("distance", event_obj.distance);
  ros::param::getCached("x_vel", event_obj.x_vel);
  ros::param::getCached("y_vel", event_obj.y_vel);
  ros::param::getCached("ang_vel", event_obj.ang_vel);

  event_obj.odom_subscriber = n.subscribe("/odom", 1000, &Event::odom_callback, &event_obj);
  event_obj.event_in_server = n.advertiseService("event_in", &Event::event_in_callback, &event_obj);
  event_obj.input_server = n.advertiseService("input", &Event::input_callback, &event_obj);


  event_obj.velocity_publisher = n.advertise<geometry_msgs::Twist>("/cmd_vel", 3);
  event_obj.status_publisher = n.advertise<mas_ros_hackathon::event_out>("/event_out", 3);

  ros::spin();

  return 0;
}