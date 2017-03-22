#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "string.h"
#include <cmath> 
#include <tf/transform_broadcaster.h>
#include "math.h"
#include "sensor_msgs/LaserScan.h"
#include "nav_msgs/Odometry.h"

class Laser {
public:
  ros::Subscriber laser_scan_subscriber;
  ros::Publisher velocity_publisher;
  ros::Subscriber odom_subscriber;
  Laser(ros::NodeHandle n);
  double range_set1[10], range_set2[10], range_set3[10], range_set4[10], range_set5[10], range_set6[10], range_set7[10]; 
  double set_avg[7];
  double safe_distance, x_vel;
  void laser_scan_callback(const sensor_msgs::LaserScan::ConstPtr& msg);
  double calculate_avg(const float *range,int length);
  bool detect_obstacle(double set_avg[7], double safe_distance);
  void publish_cmd_vel(double x_vel_t, double y_vel_t, double angular_vel_t);
};