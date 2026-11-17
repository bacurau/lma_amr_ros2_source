"""ROS-free logic of the IMU preprocessing: stamp offset and gyro z bias filter."""
from dataclasses import dataclass
from typing import List, Optional


class StampOffset:
    """Map device-time header stamps to the ROS clock, keeping the sensor timing.

    offset = running minimum of (clock at receipt - header), so transport delay is not added
    to the stamps. Times are integer nanoseconds.
    """

    def __init__(self) -> None:
        self.offset_ns: Optional[int] = None

    def apply(self, clock_ns: int, header_ns: int) -> int:
        """Return the corrected stamp for a message received at clock_ns."""
        delta = clock_ns - header_ns
        if self.offset_ns is None or delta < self.offset_ns:
            self.offset_ns = delta
        return header_ns + self.offset_ns


@dataclass
class BiasFilterConfig:
    """Tuning of GyroBiasFilter. Units: rad/s, m/s, s."""

    q_rw: float               # bias random walk, rad/s/sqrt(s)
    sigma_wheel: float        # stddev of (mean gyro - wheel yaw rate) while driving straight
    w_max: float              # largest |wheel yaw rate| counted as driving straight
    v_min: float              # smallest |speed| counted as driving
    still_eps: float          # |v| and |w| below this count as stopped
    settle: float             # time stopped before stationary updates start
    gyro_stddev_z: float      # gyro z noise per sample


class GyroBiasFilter:
    """One-state Kalman filter on the gyro z bias, updated at each wheel odometry message.

    Stopped for at least `settle`:  z = mean gyro             R = gyro_stddev_z^2 / n
    Driving straight:               z = mean gyro - wheel w   R = sigma_wheel^2
    Otherwise there is no update, since wheel yaw-rate scale errors and slip grow with |w|.

    Driving straight, any wheel yaw per metre (e.g. a wheel radius difference) is absorbed
    into the bias estimate.
    """

    def __init__(self, b0: float, std0: float, cfg: BiasFilterConfig) -> None:
        self.b = b0
        self.P = std0 ** 2
        self.cfg = cfg
        self.t: Optional[float] = None
        self.still_since: Optional[float] = None
        self.n_updates: List[int] = [0, 0]   # stationary, moving

    def step(self, t: float, gz_mean: float, n: int, v: float, w: float) -> None:
        """Advance to time t with the mean of n gyro samples and the wheel speeds v, w."""
        c = self.cfg
        if self.t is not None:
            self.P += c.q_rw ** 2 * max(t - self.t, 0.0)
        self.t = t
        still = abs(v) < c.still_eps and abs(w) < c.still_eps
        if not still:
            self.still_since = None
        elif self.still_since is None:
            self.still_since = t
        if n == 0:
            return
        if still and t - self.still_since >= c.settle:
            z, R, kind = gz_mean, c.gyro_stddev_z ** 2 / n, 0
        elif abs(v) > c.v_min and abs(w) < c.w_max:
            z, R, kind = gz_mean - w, c.sigma_wheel ** 2, 1
        else:
            return
        K = self.P / (self.P + R)
        self.b += K * (z - self.b)
        self.P *= 1.0 - K
        self.n_updates[kind] += 1
