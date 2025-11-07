#include <cstdio>
#include <csignal>
#include <filesystem>
#include <atomic>
#include <thread>
#include <chrono>
#include <map>

using namespace std::chrono_literals;

#include "xpc.h"
#include "xpc_signal.h"
#include "ros_publisher.h"

#include "xpcapi.h"
#include "xpcapiconst.h"

constexpr char target_ip[] = "10.10.10.11";
constexpr char target_port[] = "22222";
constexpr char target_model[] = "AM2_3_1_01HWfullV2_PCIe";

int port;

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);

  printf("hello world armeo_xpc package\n");
  const auto exe_path = std::filesystem::path(argv[0]).remove_filename();
  printf("exe_path: %ls\n", exe_path.c_str());

  if (xPCInitAPI()) {
    fprintf(stderr, "xPC failed to start - is xpcapi.dll present?\n");
    return -1;
  }

  printf("Connecting to %s:%s...\n", target_ip, target_port);
  port = xPCOpenTcpIpPort(target_ip, target_port);
  if (xpcError()) {
    fprintf(stderr, "Failed to connect to target.\n");
    return -1;
  }

  xPCLoadApp(port, exe_path.generic_string().c_str(), target_model);
  if (xpcError()) {
    fprintf(stderr, "Failed to load model - is %s.dlm present?\n", target_model);
    return -1;
  }

  printf("Application %s loaded.\n SampleTime: %g\n StopTime: %g\n", target_model, xPCGetSampleTime(port),
         xPCGetStopTime(port));


  auto topics = get_ros_xpc_topics(port);
  const size_t topic_count = topics.size();

  const auto sample_time = std::chrono::duration<double>(xPCGetSampleTime(port));
  const auto sample_time_us = std::chrono::duration_cast<std::chrono::microseconds>(sample_time);
  const auto ros_node = std::make_shared<ArmeoXPCPublisher>(sample_time_us, std::move(topics));

  printf("Signals loaded for %llu topics. Ready to start!\n", topic_count);
  // No bailing out now!
  if (!xPCIsAppRunning(port)) {
    xPCStartApp(port);
    printf("xPC started.\n");
  } else {
    fprintf(stderr, "App was already running? Continuing anyway\n");
  }

  ros_node->start();
  rclcpp::spin(ros_node);

  printf("Application finished. Cleaning up.\n");
  rclcpp::shutdown();
  if (xPCIsAppRunning(port)) {
    xPCStopApp(port);
  }
  xPCUnloadApp(port);

  return 0;
}

