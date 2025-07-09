# AMR Sensors - ROS 2 Driver Integration

This module handles integration and configuration of LiDAR and IMU sensors for autonomous mobile robot (AMR) platforms using ROS 2 (Jazzy).

---

## 📦 Included Drivers

- **SICK LiDAR**: via [`sick_scan_xd`](https://github.com/SICKAG/sick_scan_xd)
- **XSENS IMU**

---

## 🛰️ SICK LiDAR Driver (`sick_scan_xd`)

### 🔧 Build Instructions

1. **Add the Driver as a Submodule**
   ```bash
   cd ./lma_amr_ros2_source/src/amr_sensors
   git submodule add https://github.com/SICKAG/sick_scan_xd
   cd ../../../
   ```

   > 📝 Only required during first-time setup or reinstallation.

2. **Build the Driver (Without LD-MRS Support)**

    `libsick_ldmrs` is only required to support LD-MRS sensors. If you do not need or want to support LD-MRS, you can skip building `libsick_ldmrs`. To build `sick_generic_caller` without LD-MRS support, switch off option `BUILD_WITH_LDMRS_SUPPORT` in `CMakeLists.txt` or call colcon with option `-DLDMRS=0`:

   ```bash
   colcon build --packages-select sick_scan_xd \
     --cmake-args " -DROS_VERSION=2" " -DLDMRS=0" \
     --event-handlers console_direct+
   ```
---

### ⚙️ Configuration

Each LiDAR device is configured through a dedicated `.launch file` containing XML-style arguments that are passed as ROS 2 launch parameters.

An example template is provided at:

```bash
   sick_scan_xd/launch/sick_tim_5xx.launch
```

#### 🔑 Key Parameters (example from `sick_back.launch`)

Some key configurable parameters are:

```xml
<arg name="hostname" default="192.200.253.206" />
<arg name="cloud_topic" default="Sick_Cloud_Back" />
<arg name="frame_id" default="Sick_Back_D_Link" />
```

- `hostname`: IP address of the device
- `cloud_topic`: Topic name where point clouds will be published
- `frame_id`: TF frame associated with the device

Additional configurable options:
- `range_min`, `range_max`
- `angle_min`, `angle_max`
- `add_transform_xyz_rpy`, etc.

> ⚠️ Each device (front/back) must have its own launch configuration, we unique IP addresses and topic names.

#### 📁 Project Launch Files
- `src/amr_sensors/launch/sick_front.launch`
- `src/amr_sensors/launch/sick_back.launch`

---

### 🚀 Launching Both LiDAR Devices

Use the provided launch file to start both front and back devices:

```bash
ros2 launch amr_sensors lidar.launch.py
```

This internally invokes the `sick_generic_caller` node using both launch configurations.

---

## 🧭 XSENS IMU Driver

### 📦 Install Required Dependencies

```bash
sudo apt install ros-jazzy-nmea-msgs
sudo apt install ros-jazzy-mavros-msgs
```

These packages are needed for NMEA and MAVROS-compatible IMU data parsing.

---

## 📂 Repository Structure

```
.
lma_amr_ros2_source/
├── src/
|    ├── amr_sensors/
|    │   ├── include
|    │   ├── launch/
│    │   │   ├── sick_front.launch
│    │   │   ├── sick_back.launch
│    │   │   ├── imu.launch.py
│    │   │   └── lidar.launch.py
|    │   ├── lib
|    │   ├── package.xml
|    │   ├── param
|    │   ├── src
|    │   └── xsens_mti_node.yaml
|    ├── Xsens_MTi_ROS_Driver_and_Ntrip_Client/ ← Git submodule
|    └── sick_scan_xd/ ← Git submodule

---

## ✅ Notes

- Compatible with **ROS 2 Jazzy**.
- Ensure network access to LiDAR devices (correct `hostname` setting).
- Each device should publish to a unique topic and frame ID.

---

## 📞 Support

For additional configuration examples and troubleshooting:
- SICK SCAN XD Docs: [https://github.com/SICKAG/sick_scan_xd](https://github.com/SICKAG/sick_scan_xd)
- XSENS ROS Drivers: [https://github.com/xsens](https://github.com/xsens)