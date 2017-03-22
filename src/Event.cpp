#include "mas_ros_hackathon/Event.hpp"

Event::Event(ros::NodeHandle n)
{

}

bool Event::event_in_callback(mas_ros_hackathon::event_in::Request &req, mas_ros_hackathon::event_in::Response &res)
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


void Event::odom_callback(const nav_msgs::Odometry::ConstPtr& msg)
{
  if(event_on & event_status == 1)
  {
    curr_time = ros::Time::now().toSec();
    double time_diff = curr_time - start_time;
    if(time_diff > timeout)
    {
      ROS_WARN("Timeout occured");
      publish_cmd_vel(0,0,0);

      mas_ros_hackathon::event_out event_out_msg;
      event_out_msg.status = "e_failed";
      status_publisher.publish(event_out_msg);
    }
    else
    {
      double x = msg->pose.pose.position.x;
      double y = msg->pose.pose.position.y;
      double theta = msg->pose.pose.orientation.z;


      convert_to_rpy(msg->pose.pose.orientation.x, msg->pose.pose.orientation.y, msg->pose.pose.orientation.z, msg->pose.pose.orientation.w);

      // ROS_INFO("Current Postion X:%f|Y:%f|Heading:%f ", x, y, yaw);

      geometry_msgs::Twist vel_msg;

      double y_diff = std::abs(start_y) - std::abs(y);
      double x_diff = std::abs(start_x) - std::abs(x);

      double dist = sqrt(pow(x_diff,2) + pow(y_diff,2));

      if(dist > 1.0)
      {
        publish_cmd_vel(0,0,0);
        event_status = 2;
        ROS_INFO("Final position: X:%f|Y:%f|Heading:%f ", x, y, yaw);
        mas_ros_hackathon::event_out event_out_msg;
        event_out_msg.status = "e_success";
        status_publisher.publish(event_out_msg);
      }
      else
      {
        if(input == 1)
          publish_cmd_vel(0,0.1,0);
        else if(input == 2)
          publish_cmd_vel(0,-0.1,0);
        
      }
    }
  }
  else if(event_on & event_status == 0)
  {
    double x = msg->pose.pose.position.x;
    double y = msg->pose.pose.position.y;
    double theta = msg->pose.pose.orientation.z;

    if(input == 1 || input == 2)
    {

      convert_to_rpy(msg->pose.pose.orientation.x, msg->pose.pose.orientation.y, msg->pose.pose.orientation.z, msg->pose.pose.orientation.w);
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

bool Event::input_callback(mas_ros_hackathon::input::Request &req, mas_ros_hackathon::input::Response &res)
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


void Event::publish_cmd_vel(double x_vel, double y_vel, double angular_vel)
{
  geometry_msgs::Twist vel_msg;
  vel_msg.linear.x = x_vel;
  vel_msg.linear.y = y_vel;
  vel_msg.angular.z = angular_vel;
  velocity_publisher.publish(vel_msg);
}

void Event::convert_to_rpy(double orientation_x, double orientation_y, double orientation_z, double orientation_w)
{
  tf::Quaternion q(orientation_x, orientation_y, orientation_z, orientation_w);
  tf::Matrix3x3 m(q);
  m.getRPY(roll, pitch, yaw);
}
