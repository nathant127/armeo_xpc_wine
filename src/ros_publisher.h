#pragma once

#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "armeo_xpc_interfaces/msg/xpc_bridged_data.hpp"

#include "xpc_signal.h"

// XXX HACK
extern int port;

class ArmeoXPCPublisher final : public rclcpp::Node {
public:
  explicit ArmeoXPCPublisher(std::chrono::microseconds sample_time, std::vector<XpcRosTopic> topics);

  void publish() const;

  void start() const {
    publish_timer->reset();
  }
  void stop() const {
    publish_timer->cancel();
  }

private:
  using Publisher = rclcpp::Publisher<armeo_xpc_interfaces::msg::XpcBridgedData>;
  rclcpp::TimerBase::SharedPtr publish_timer;

  const std::vector<std::pair<XpcRosTopic, Publisher::SharedPtr>> topics;
  decltype(topics) make_publishers(std::vector<XpcRosTopic> topics);
};
