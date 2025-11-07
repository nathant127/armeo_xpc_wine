//
// Created by Ash on 5/11/2025.
//
#include "ros_publisher.h"

#include <utility>

#include "xpc.h"
#include "xpcapi.h"

ArmeoXPCPublisher::ArmeoXPCPublisher(const std::chrono::microseconds sample_time, std::vector<XpcRosTopic> topics)
  : Node("armeo_xpc"), topics(make_publishers(std::move(topics))) {
  publish_timer = this->create_wall_timer(sample_time, [this] { this->publish(); }, nullptr, false);
  printf("Sample time: %lldus\n", sample_time.count());
}

decltype(ArmeoXPCPublisher::topics) ArmeoXPCPublisher::make_publishers(std::vector<XpcRosTopic> topics) {
  std::vector<std::pair<XpcRosTopic, Publisher::SharedPtr>> result;

  for (auto &topic : topics) {
    result.emplace_back(
      topic,
      this->create_publisher<armeo_xpc_interfaces::msg::XpcBridgedData>(topic.ros_topic, 10)
    );
  }

  return result;
}

void ArmeoXPCPublisher::publish() const {
  for (const auto &[xpc, publisher] : topics) {
    armeo_xpc_interfaces::msg::XpcBridgedData message;
    message.data.reserve(xpc.xpc_indexes.size());
    bool fail = false;

    for (const auto signal_idx : xpc.xpc_indexes) {
      double val = xPCGetSignal(port, signal_idx);
      if (xpcError()) {
        fail = true;
        break;
      }

      message.data.push_back(val);
    }
    if (fail) {
      fprintf(stderr, "Skipping publish for topic %s.\n", xpc.ros_topic.c_str());
      continue;
    }

    publisher->publish(message);
  }
}
