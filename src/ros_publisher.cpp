//
// Created by Ash on 5/11/2025.
//
#include "ros_publisher.h"

#include "xpcapi.h"

ArmeoXPCPublisher::ArmeoXPCPublisher(const std::chrono::microseconds sample_time)
  : Node("armeo_xpc") {
  end_effector_pub = this->create_publisher<geometry_msgs::msg::Point>("end_effector", 10);
  publish_timer = this->create_wall_timer(sample_time, [this] { this->publish(); });
  printf("Sample time: %lldus\n", sample_time.count());
}

void ArmeoXPCPublisher::publish() const {
  // printf("Time to publish!\n");

  auto x_idx = signals["Output/SoftwareSignals/EndEffector/s1"];
  auto y_idx = signals["Output/SoftwareSignals/EndEffector/s2"];
  auto z_idx = signals["Output/SoftwareSignals/EndEffector/s3"];

  auto x = xPCGetSignal(port, x_idx);
  auto y = xPCGetSignal(port, y_idx);
  auto z = xPCGetSignal(port, z_idx);

  geometry_msgs::msg::Point ee_point;
  ee_point.x = x;
  ee_point.y = y;
  ee_point.z = z;

  this->end_effector_pub->publish(ee_point);
}
