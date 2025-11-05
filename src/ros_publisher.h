#pragma once

#include <chrono>
#include <map>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/point.hpp"

// XXX HACK
extern std::map<const char *, int> signals;
extern int port;

class ArmeoXPCPublisher final : public rclcpp::Node {
public:
  explicit ArmeoXPCPublisher(std::chrono::microseconds sample_time);

  void publish() const;
private:
  rclcpp::Publisher<geometry_msgs::msg::Point>::SharedPtr end_effector_pub;
  rclcpp::TimerBase::SharedPtr publish_timer;
};
