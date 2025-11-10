# armeo_xpc - ROS 2 bridge driver for Hocoma ArmeoPower exoskeleton

This is a ROS 2 driver for the control PC ("AM-PC") present in the ArmeoPower upper-body exoskeleton. For more on the architecture of the AM-PC and the exoskeleton as a whole, see [armeo-re](https://github.com/ashquarky/armeo-re).

Due to the use of Simulink Real-Time (formerly xPC) in the AM-PC, this ROS bridge can only run on **Windows** platforms. Of course, further control software running on any platform can subscribe to the topics over the network. Operation under Wine remains untested.

This bridge is designed to have the minimum behaviour needed to get the data out from xPC signals and into ROS2 topics. Therefore, calculation of transforms, joint states, etc. is out of scope and these tasks will need to be performed by another node that receives the raw data armeo_xpc publishes.

## Limitations
This software is developed and tested on UTS's ArmeoPower and compatibility with other models or even particular units of Hocoma exoskeleton is unknown. Specifically, it assumes the use of the `AM2_3_1_01HWfullV2_PCIe` model, which may not be suitable for all instances of the ArmeoPower.

This software should be considered research-alpha quality and must be closely audited before use with a human inside the exoskeleton. However, since it makes no attempt to make the robot move, it should be safe.

This software makes no attempt to enable the motor controllers or apply torques on the arm, as the process to do this is not yet understood. See [armeo-re](https://github.com/ashquarky/armeo-re) and Armeocontrol RAL log files.

## Using
### Basic setup
1. [Install ROS 2 (Jazzy Jalisco) for Windows](https://docs.ros.org/en/jazzy/Installation/Windows-Install-Binary.html). You will also need to [install the appropriate MSVC tools](https://docs.ros.org/en/jazzy/Installation/Alternatives/Windows-Development-Setup.html#install-msvc) to compile.
2. [Create a workspace](https://docs.ros.org/en/jazzy/Tutorials/Beginner-Client-Libraries/Creating-A-Workspace/Creating-A-Workspace.html) and source it.
3. In the workspace's `src` directory, clone this repository and its dependencies:
```powershell
git clone https://github.com/ashquarky/armeo_xpc
vcs import --input armeo_xpc/armeo_xpc_jazzy.repos .
```
4. Build:
```powershell
colcon build --symlink-install --merge-install
```
5. Test run (choose any [`ROS_DOMAIN_ID`](https://docs.ros.org/en/jazzy/Concepts/Intermediate/About-Domain-ID.html) as appropriate - this prevents conflicts with other robots on the network):
```powershell
ROS_DOMAIN_ID=80 ros2 run armeo_xpc armeo_xpc
```
Expected output:
```
Connecting to 10.10.10.11:22222...
xPC errror 16: TCP/IP Connect Error
Failed to connect to target.
```

### Connecting to AM-PC
1. The rear of the AM-PC has an Ethernet connection labelled "Armeocontrol PC". Disconnect this from the Armeocontrol PC and into your ROS 2 Windows machine.

![Rear of the AM-PC, showing multi-pin connectors and blue Ethernet cable](/docs/AM-PC%20rear.jpeg)
*The blue cable visible here should go directly between the AM-PC (pictured) and your own device.*

2. Assign a static IP in `10.10.10.0/24`. The AM-PC does not support DHCP so don't bother with a modem. Note that the original Armeocontrol PC uses `10.10.10.10`.

![Static IP configuration for AM-PC connection](/docs/Host%20PC%20Static%20IP.png)
*Known good settings for Windows 10 are `10.10.10.10`, subnet `255.255.255.0`, gateway and DNS blank.*

3. Power on the exoskeleton. If the Armeocontrol PC starts, do **not** open the Armeocontrol software.
4. Run armeo_xpc again.
```powershell
ROS_DOMAIN_ID=80 ros2 run armeo_xpc armeo_xpc
```
If you see `Signals loaded for 2 topics. Ready to start!`, the bridge is running and topics are being published.

**Note:** Once the model starts on the AM-PC, please make sure to cleanly exit armeo_xpc, otherwise the model may not stop on the AM-PC side and the robot will not be idle!

### Topics published
Since the intent behind this bridge is to do the absolute minimal amount of work (since this allows the most flexibility in setup), and in recognition of the unusual nature of a ROS node with a hard Windows dependency, armeo_xpc makes no attempt to publish standardised topics like `/tf` or `/joint_states`. See Intended Use below. Instead, all topics use `XpcBridgedData` from [armeo_xpc_interfaces](https://github.com/ashquarky/armeo_xpc_interfaces), which is a raw float64 array.

In the initial release, the two topics published are:
- `/end_effector_pos` - `float64[3]` - XYZ position of end-effector, presumably in meters.
- `/joint_angles` - `float64[6]` - Angle of each revolute joint, not including lift. Presumably in radians.

More topics can be added by editing the `wanted_signals` array in `src/xpc_signal.cpp`. Interesting signals can be identified by reading the Armeocontrol software's RAL log.

These topics are published at the sample rate of the xPC model, which can be quite high.

## Intended Use
It's quite unusual to have a ROS node that must run on Windows, and it's assumed that most users would prefer to write their ArmeoPower control code on Linux. This informs the design choices of armeo_xpc and the ways it can be deployed.

### Usecase 1: Minimal bridge
![Diagram showing "Hocoma model" running in Simulink, "armeo_xpc" in Windows, "Your ROS driver" & "Your control app" in Ubuntu](/docs/Basic%20Bridge.png)
In this setup, the Windows machine (possibly even the Armeocontrol PC) runs armeo_xpc only, communicating over the network with your own ROS driver which consumes the XpcBridgedData topics and converts them into standardised transforms, joint angles, etc.

The benefit of this setup is that the development of the ROS driver can occur in the familiar Ubuntu environment, without strictly needing Windows compilers and tooling set up everywhere. Additionally, the driver can be tested with a rosbag of armeo_xpc's output, rather than only when attached to the actual AM-PC.

### Usecase 2: Driver bridge
![Diagram showing "Hocoma model" running in Simulink, "armeo_xpc" & "Your ROS driver" in Windows, "Your control app" in Ubuntu](/docs/Driver%20Bridge.png)
In this setup, the ROS driver (publishing standard topics like transforms) runs on the Windows PC, either as a seperate process or as functionality added to armeo_xpc itself. Then, the control app reads the standard ROS topics over the network, with no armeo_xpc data exiting the Windows side.

This would be the ideal long-term deployment once a stable ROS driver exists, since the Windows PC could be permanently installed inside the exo, and prospective users could connect to the same network and immediately see the standard ROS topics without further work on their end.

### Usecase 3: Driver bridge
![Diagram showing "Hocoma model" running in Simulink, "armeo_xpc" & "Your ROS driver" & "Your control app" in Windows, nothing in Ubuntu](/docs/No%20Bridge.png)
In this seutp, everything is developed against ROS for Windows. This is the most unfamiliar for existing ROS2 users, but is actually quite convenient for devleoping armeo_xpc itself, or if the ArmeoPower is going to semi-permanently run the same control app from now on, since one less computer needs to be involved.

### Usecase 4: Wine
Any of the above three use-cases could also possibly be achieved on a single physical computer via the use of virtual machines or Wine. This possibility has not yet been explored and armeo_xpc's behaviour in this situation is unknown.

## Code structure
- The `xpc` directory contains Simulink Real-Time components and APIs needed to talk to the AM-PC,
- `armeo_xpc.cpp` contains the main application startup and intitialisation, including connection to the AM-PC,
- `xpc.cpp` contains helpers for dealing with errors in the xPC API,
- `xpc_signal.cpp` contains the list of signals to publish and fetches their Signal ID numbers for publication,
- `ros_publisher.cpp` contains the actual ROS node, including a timer to publish all of the topics and code to poll the latest signal data for each.
