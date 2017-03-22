#include "mas_ros_hackathon/Laser.hpp"

int main(int argc, char *argv[])
{
  ros::init(argc, argv, "laser_node");
  ros::Time::init();
  ros::NodeHandle n;

  Laser laser_obj(n);

  ros::param::getCached("safe_distance", laser_obj.safe_distance);
  ros::param::getCached("x_vel", laser_obj.x_vel);

  laser_obj.laser_scan_subscriber = n.subscribe("/base_scan", 1000, &Laser::laser_scan_callback, &laser_obj);
  laser_obj.velocity_publisher = n.advertise<geometry_msgs::Twist>("/cmd_vel", 3);

  ros::spin();

  return 0;
}