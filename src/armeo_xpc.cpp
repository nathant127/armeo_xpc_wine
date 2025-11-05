#include <cstdio>
#include <csignal>
#include <filesystem>
#include <atomic>
#include <thread>
#include <chrono>
#include <map>

#include "xpcapi.h"
#include "xpcapiconst.h"

constexpr char target_ip[] = "10.10.10.11";
constexpr char target_port[] = "22222";
constexpr char target_model[] = "AM2_3_1_01HWfullV2_PCIe";

const char *wanted_signals[] = {
  "Output/SoftwareSignals/EndEffector/s1",
  "Output/SoftwareSignals/EndEffector/s2",
  "Output/SoftwareSignals/EndEffector/s3",
  "Output/SoftwareSignals/JointAngle/s1",
  "Output/SoftwareSignals/JointAngle/s2",
  "Output/SoftwareSignals/JointAngle/s3",
  "Output/SoftwareSignals/JointAngle/s4",
  "Output/SoftwareSignals/JointAngle/s5",
  "Output/SoftwareSignals/JointAngle/s6",
  "Output/SoftwareSignals/JointAngle/s7",
};

static int xpcError();

static std::map<const char *, int> signals;
static std::atomic<bool> quit = false;

static void mainloop(const int port) {
  using namespace std::chrono_literals;

  while (!quit) {
    for (const auto [name, num]: signals) {
      const auto val = xPCGetSignal(port, num);
      printf("%s: %f\n", name, val);
    }

    std::this_thread::sleep_for(1ms);
  }
}

int main(int argc, char **argv) {
  (void) argc;
  (void) argv;

  printf("hello world armeo_xpc package\n");
  auto exe_path = std::filesystem::path(argv[0]).remove_filename();
  printf("exe_path: %ls\n", exe_path.c_str());

  if (xPCInitAPI()) {
    fprintf(stderr, "xPC failed to start - is xpcapi.dll present?\n");
    return -1;
  }

  printf("Connecting to %s:%s...\n", target_ip, target_port);
  const int port = xPCOpenTcpIpPort(target_ip, target_port);
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

  for (const char *signal: wanted_signals) {
    const int sig_idx = xPCGetSignalIdx(port, signal);
    if (xpcError() || sig_idx == -1) {
      fprintf(stderr, "Failed to acquire signal %s - skipping...\n", signal);
      continue;
    }

    signals[signal] = sig_idx;
  }

  printf("%llu signals loaded. Ready to start!\n", signals.size());
  // No bailing out now!
  if (!xPCIsAppRunning(port)) {
    xPCStartApp(port);
    printf("xPC started.\n");
  } else {
    fprintf(stderr, "App was already running? Continuing anyway\n");
  }

  SetConsoleCtrlHandler([](const DWORD ctrl_type) -> BOOL {
    if (ctrl_type != CTRL_C_EVENT && ctrl_type != CTRL_BREAK_EVENT) {
      return FALSE;
    }

    printf("Received %lu - asking to exit...\n", ctrl_type);
    quit = true;
    return TRUE;
  }, true);

  mainloop(port);

  printf("Application finished. Cleaning up.\n");
  if (xPCIsAppRunning(port)) {
    xPCStopApp(port);
  }
  xPCUnloadApp(port);

  return 0;
}

static int xpcError() {
  const int err = xPCGetLastError();
  if (err == ENOERR) return 0;

  const char *err_msg = xPCErrorMsg(err, nullptr);
  fprintf(stderr, "xPC error %d: %s\n", err, err_msg);
  return err;
}
