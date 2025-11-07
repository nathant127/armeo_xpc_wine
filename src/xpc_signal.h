#pragma once

#include <string>
#include <vector>

struct XpcSignal {
  std::string xpc_name;
  // datastructures ouchie
  std::vector<std::string> xpc_components = { "" };

  std::string ros_topic;
};

struct XpcRosTopic {
  std::string ros_topic;
  std::vector<int> xpc_indexes;
};

std::vector<XpcRosTopic> get_ros_xpc_topics(int port);