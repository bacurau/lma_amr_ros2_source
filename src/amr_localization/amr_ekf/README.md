# amr_ekf

Local EKF for the AMR (wheel odometry + gyro) with real-time gyro bias removal, plus the
slam_toolbox parameters validated with it on the back SICK.

| Item | Purpose |
|---|---|
| `imu_fix_node` | `/imu/data` -> `/imu/fixed`: restamps, removes the gyro z bias, fills covariances. Publishes `/imu/gyro_bias`. |
| `amr_ekf/bias_filter.py` | ROS-free logic: stamp offset and the 1-state bias Kalman filter. |
| `config/ekf.yaml` | robot_localization: `vx`, `vy`, `vyaw` from `/odom`, `vyaw` from `/imu/fixed`, 50 Hz. |
| `config/imu_fix.yaml` | IMU preprocessing parameters. |
| `config/slam_mapping_back.yaml` | slam_toolbox mapping, loop search 8 m. |
| `config/slam_localization_back.yaml` | slam_toolbox localization. Set `map_file_name` first. |
| `launch/ekf_launch.py` | Wheel odometry without its TF, `imu_fix_node` and `ekf_node`. |

## Install

Needs `ros-jazzy-robot-localization` and `ros-jazzy-slam-toolbox`, which are not in the
current Dockerfile.

## Run

```bash
ros2 launch amr_ekf ekf_launch.py use_sim_time:=true

ros2 launch amr_slam slam_launch.py use_sim_time:=true scan_topic:=/sick_tim_5xx_back/scan \
  slam_params_file:=$(ros2 pkg prefix amr_ekf)/share/amr_ekf/config/slam_mapping_back.yaml
```

Play bags at rate 1. Higher rates changed the EKF output and the loop closure timing.

## Bias filter

Updated at each `/odom` message:

- Stopped for at least `settle`: `z = mean gyro`, `R = sigma_g^2 / n`.
- Driving straight: `z = mean gyro - wheel yaw rate`, `R = sigma_wheel^2`.
- Otherwise: no update.

Tests: `python3 -m pytest test` (no ROS needed).

## Known limits

- Driving straight, wheel yaw per metre (e.g. a wheel radius difference) leaks into the bias
  estimate.
- Starting far from the true bias needed about 400 s to converge on bag `mini_test2`.
- `restamp: true` was validated only on bags (device-time stamps). Check it on the live driver.
- The SICK extrinsic yaw looks off by about -2.5 deg (from a sweep on `mini_test2`, not a
  measurement). The URDF is unchanged.
