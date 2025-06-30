## SICK SCAN Drivers

Run the following steps to build sick_scan_xd on Linux with ROS 2:

Create a workspace folder, e.g. sick_scan_ws (or any other name):

mkdir -p ./sick_scan_ws
cd ./sick_scan_ws
Clone repositories https://github.com/SICKAG/sick_scan_xd:

mkdir ./src
cd ./src
git clone https://github.com/SICKAG/libsick_ldmrs.git
git clone -b master https://github.com/SICKAG/sick_scan_xd.git
cd ..
rm -rf ./build ./build_isolated/ ./devel ./devel_isolated/ ./install ./install_isolated/ ./log/ # remove any files from a previous build

In the current version, the sick_scan_xd was added as a submodule to the project. The previous steps will only be necessary in case of a reinstall.

libsick_ldmrs is only required to support LD-MRS sensors. If you do not need or want to support LD-MRS, you can skip building libsick_ldmrs. To build sick_generic_caller without LD-MRS support, switch off option BUILD_WITH_LDMRS_SUPPORT in CMakeLists.txt or call colcon with option -DLDMRS=0:

colcon build --packages-select sick_scan_xd --cmake-args " -DROS_VERSION=2" " -DLDMRS=0" --event-handlers console_direct+


## XSENS IMU Drivers

To install the dependencies for the XSENS IMU driver, run the following:

sudo apt install ros-jazzy-nmea-msgs
sudo apt install ros-jazzy-mavros-msgs
