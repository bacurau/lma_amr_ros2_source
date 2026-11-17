"""Republish /imu/data for robot_localization as /imu/fixed.

Restamps device-time headers, removes the gyro z bias (constant or estimated online from the
wheel odometry) and fills the covariances, which the driver leaves at zero. The bias estimate
is published on /imu/gyro_bias (vector.z = bias, vector.x = stddev, rad/s).
"""
import math
from typing import List

from geometry_msgs.msg import Vector3Stamped
from nav_msgs.msg import Odometry
import rclpy
from rclpy.node import Node
from rclpy.parameter import Parameter
from rclpy.time import Time
from sensor_msgs.msg import Imu

from amr_ekf.bias_filter import BiasFilterConfig, GyroBiasFilter, StampOffset


def diagonal(stddev: List[float]) -> List[float]:
    """Return a 3x3 row-major covariance from per-axis standard deviations."""
    return [stddev[0] ** 2, 0.0, 0.0, 0.0, stddev[1] ** 2, 0.0, 0.0, 0.0, stddev[2] ** 2]


class ImuFix(Node):
    """Preprocess the IMU; see the module docstring."""

    def __init__(self) -> None:
        super().__init__('imu_fix')
        p = self.get_param
        self.restamp = p('restamp', Parameter.Type.BOOL)
        self.bias_mode = p('bias_mode', Parameter.Type.STRING)
        self.bias_value = p('bias_value', Parameter.Type.DOUBLE)
        gyro = p('gyro_stddev', Parameter.Type.DOUBLE_ARRAY)
        accel = p('accel_stddev', Parameter.Type.DOUBLE_ARRAY)
        if self.bias_mode not in ('const', 'online'):
            raise ValueError(f"bias_mode must be 'const' or 'online', got '{self.bias_mode}'")
        self.gyro_cov = diagonal(list(gyro))
        self.accel_cov = diagonal(list(accel))
        self.stamp = StampOffset()
        self.kf = None
        self.count = 0
        self.gz_sum = 0.0
        self.gz_n = 0
        self.pub = self.create_publisher(Imu, '/imu/fixed', 200)
        self.bias_pub = self.create_publisher(Vector3Stamped, '/imu/gyro_bias', 10)
        self.create_subscription(Imu, '/imu/data', self.imu_cb, 200)
        if self.bias_mode == 'online':
            self.kf = GyroBiasFilter(
                p('bias_init', Parameter.Type.DOUBLE), p('bias_init_std', Parameter.Type.DOUBLE),
                BiasFilterConfig(
                    q_rw=p('q_rw', Parameter.Type.DOUBLE),
                    sigma_wheel=p('sigma_wheel', Parameter.Type.DOUBLE),
                    w_max=p('w_max', Parameter.Type.DOUBLE),
                    v_min=p('v_min', Parameter.Type.DOUBLE),
                    still_eps=p('still_eps', Parameter.Type.DOUBLE),
                    settle=p('settle', Parameter.Type.DOUBLE),
                    gyro_stddev_z=gyro[2]))
            self.create_subscription(Odometry, '/odom', self.odom_cb, 50)

    def get_param(self, name: str, kind: Parameter.Type):
        """Declare a required parameter and return its value."""
        self.declare_parameter(name, kind)
        return self.get_parameter(name).value

    def odom_cb(self, m: Odometry) -> None:
        """Update the bias with the gyro samples received since the last odometry message."""
        t = m.header.stamp.sec + m.header.stamp.nanosec * 1e-9
        mean = self.gz_sum / self.gz_n if self.gz_n else 0.0
        self.kf.step(t, mean, self.gz_n, m.twist.twist.linear.x, m.twist.twist.angular.z)
        self.gz_sum = 0.0
        self.gz_n = 0
        out = Vector3Stamped()
        out.header = m.header
        out.vector.z = self.kf.b
        out.vector.x = math.sqrt(self.kf.P)
        self.bias_pub.publish(out)

    def imu_cb(self, m: Imu) -> None:
        """Fix one IMU message and republish it."""
        if self.restamp:
            now = self.get_clock().now()
            if now.nanoseconds == 0:   # /clock not received yet
                return
            fixed = self.stamp.apply(now.nanoseconds, Time.from_msg(m.header.stamp).nanoseconds)
            m.header.stamp = Time(nanoseconds=fixed).to_msg()
        self.gz_sum += m.angular_velocity.z
        self.gz_n += 1
        m.angular_velocity.z -= self.kf.b if self.kf else self.bias_value
        m.orientation_covariance = [-1.0] + [0.0] * 8   # orientation not provided
        m.angular_velocity_covariance = self.gyro_cov
        m.linear_acceleration_covariance = self.accel_cov
        self.pub.publish(m)
        self.count += 1
        if self.count % 40000 == 0:
            s = f'{self.count} msgs, offset {(self.stamp.offset_ns or 0) * 1e-9:.4f} s'
            if self.kf:
                s += (f', bias {self.kf.b:.5f} +- {math.sqrt(self.kf.P):.5f} rad/s, '
                      f'updates still/moving {self.kf.n_updates}')
            self.get_logger().info(s)


def main() -> None:
    rclpy.init()
    rclpy.spin(ImuFix())


if __name__ == '__main__':
    main()
