C:\pixi_ws\.pixi\envs\default\Library/bin\cmake.EXE C:\pixi_ws\ros2_ws\src\armeo_xpc -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -G Ninja -DAMENT_CMAKE_SYMLINK_INSTALL=1 -DCMAKE_INSTALL_PREFIX=C:\pixi_ws\ros2_ws\install
C:\pixi_ws\.pixi\envs\default\Library/bin\cmake.EXE --build C:\pixi_ws\ros2_ws\build\armeo_xpc -- -j16 -l16
C:\pixi_ws\.pixi\envs\default\Library/bin\cmake.EXE --install C:\pixi_ws\ros2_ws\build\armeo_xpc
cp compile_commands.json ..