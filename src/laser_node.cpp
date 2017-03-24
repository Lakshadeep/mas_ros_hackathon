#include "mas_ros_hackathon/Laser.hpp"

int main(int argc, char *argv[])
{
  ros::init(argc, argv, "laser_node");
  ros::Time::init();
  ros::NodeHandle n;
  int timeout;

  // initialise the laser object
  Laser laser_obj(n);

  // loads the ros params
  ros::param::getCached("safe_distance", laser_obj.safe_distance);
  ros::param::getCached("x_vel", laser_obj.x_vel);
  ros::param::getCached("timeout", timeout);

  // initialise the laser scanner subscriber
  laser_obj.laser_scan_subscriber_front = n.subscribe("/scan_front", 1000, &Laser::front_laser_scan_callback, &laser_obj);
  // laser_obj.laser_scan_subscriber_front = n.subscribe("/base_scan", 1000, &Laser::front_laser_scan_callback, &laser_obj);
  laser_obj.laser_scan_subscriber_back = n.subscribe("/scan_rear", 1000, &Laser::back_laser_scan_callback, &laser_obj);
  // laser_obj.laser_scan_subscriber_back = n.subscribe("/base_scan_back", 1000, &Laser::back_laser_scan_callback, &laser_obj);
  
  // initialise the /cmd_vel publisher
  laser_obj.velocity_publisher = n.advertise<geometry_msgs::Twist>("/cmd_vel", 3);

  while(ros::ok())
  {

    double start_time, curr_time;
    if(laser_obj.safe_mode == 1)
    {
      start_time = ros::Time::now().toSec();
      laser_obj.safe_mode = 2;
    }
    else if(laser_obj.safe_mode == 2)
    {
      curr_time = ros::Time::now().toSec();
      double time_diff = curr_time - start_time;
      // ROS_INFO("Time diff %f", time_diff);
      if(time_diff > timeout)
      {
        laser_obj.publish_cmd_vel(0, 0, 0);
        laser_obj.safe_mode = 0;
        ROS_WARN("Timeout Occured");
      }
    }
    
    ros::spinOnce();
  }

  return 0;
}