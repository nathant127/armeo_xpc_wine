//
// Created by Ash on 7/11/2025.
//

#include <vector>
#include <array>

#include "xpcapi.h"
#include "xpc.h"
#include "xpc_signal.h"

const std::array wanted_signals = {
  XpcSignal{
    "Output/SoftwareSignals/EndEffector",
    {"/s1", "/s2", "/s3"},
    "end_effector_pos"
  },
  XpcSignal{
    "Output/SoftwareSignals/JointAngle",
    {"/s1", "/s2", "/s3", "/s4", "/s5", "/s6"},
    "joint_angles"
  },
};

std::vector<XpcRosTopic> get_ros_xpc_topics(const int port) {
  std::vector<XpcRosTopic> topics;
  topics.reserve(wanted_signals.size());

  for (const auto &[xpc_name, xpc_components, ros_topic]: wanted_signals) {
    std::vector<int> xpc_indexes;
    xpc_indexes.reserve(xpc_components.size());

    bool fail = false;
    for (const auto &component: xpc_components) {
      const std::string xpc_full_name = xpc_name + component;
      const int sig_idx = xPCGetSignalIdx(port, xpc_full_name.c_str());
      if (xpcError() || sig_idx == -1) {
        fprintf(stderr, "Failed to acquire signal %s!\n", xpc_full_name.c_str());
        fail = true;
        break;
      }

      xpc_indexes.push_back(sig_idx);
    }
    if (fail) {
      fprintf(stderr, "Skipping topic %s due to error.\n", ros_topic.c_str());
      continue;
    }

    topics.push_back({
      ros_topic,
      xpc_indexes
    });
  }

  return topics;
}