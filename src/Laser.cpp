#include "mas_ros_hackathon/Laser.hpp"

Laser::Laser(ros::NodeHandle n)
{
  safe_mode = 0;
}

// callback function for messages published on /base_scan topic
void Laser::front_laser_scan_callback(const sensor_msgs::LaserScan::ConstPtr& msg)
{
  int start = 105;  // 105 for bot | 40 for simulation
  int spacing = 10; // 10 for bot | 2 for simulation

  for(int i = 0; i < 30; i++)
  {
    front_set_avg[i] = calculate_avg(&msg->ranges[start + (i*spacing)],spacing);
  }

  if(detect_obstacle(front_set_avg, safe_distance))
  {
    ROS_WARN("Obstacle detected by front scannar");
    publish_cmd_vel(-x_vel, 0.0, 0.0);
    safe_mode = 1;
  }
}

void Laser::back_laser_scan_callback(const sensor_msgs::LaserScan::ConstPtr& msg)
{
  int start = 105;  // 105 for bot | 40 for simulation
  int spacing = 10; // 10 for bot | 2 for simulation

  for(int i = 0; i < 30; i++)
  {
    back_set_avg[i] = calculate_avg(&msg->ranges[start + (i*spacing)],spacing);
  }

  if(detect_obstacle(back_set_avg, safe_distance))
  {
    ROS_WARN("Obstacle detected by back scannar");    
    publish_cmd_vel(x_vel, 0.0, 0.0);
    safe_mode = 1;
  }
}

// calculates average value of given set of ranges
double Laser::calculate_avg(const float *range,int length)
{
  double average = 0;
  for(int i = 0; i < length; i++)
  {
    average = average + *(range + i);
  }
  return average/length;
}

// detects obstacle using given range set
bool Laser::detect_obstacle(double set_avg[30], double safe_distance)
{
  for(int i = 0; i < 30; i++)
  {
    if(set_avg[i] < safe_distance)
    {
      return true;
    }
  }
  return false;
}

// publishes specified velocities on /cmd_vel topic
void Laser::publish_cmd_vel(double x_vel_t, double y_vel_t, double angular_vel_t)
{
  geometry_msgs::Twist vel_msg;
  vel_msg.linear.x = x_vel_t;
  vel_msg.linear.y = y_vel_t;
  vel_msg.angular.z = angular_vel_t;
  velocity_publisher.publish(vel_msg);
}

