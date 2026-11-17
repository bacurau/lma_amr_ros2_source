import pytest

from amr_ekf.bias_filter import BiasFilterConfig, GyroBiasFilter, StampOffset

CFG = BiasFilterConfig(q_rw=1e-5, sigma_wheel=0.016, w_max=0.05, v_min=0.05, still_eps=1e-4,
                       settle=0.5, gyro_stddev_z=0.00167)


def run(kf, seconds, gz, v, w, t0=0.0, n=20, hz=20):
    for k in range(int(seconds * hz)):
        kf.step(t0 + k / hz, gz, n, v, w)


def test_stamp_offset_keeps_minimum_delay():
    s = StampOffset()
    assert s.apply(1000, 100) == 1000
    assert s.apply(1200, 200) == 1100          # larger delay: keeps earlier offset
    assert s.apply(1250, 400) == 1250          # smaller delay: offset drops
    assert s.offset_ns == 850


def test_converges_when_stopped():
    kf = GyroBiasFilter(0.0, 1e-3, CFG)
    run(kf, 5.0, -0.0025, 0.0, 0.0)
    assert kf.b == pytest.approx(-0.0025, abs=1e-5)
    assert kf.n_updates[0] > 0 and kf.n_updates[1] == 0


def test_no_stationary_update_before_settle():
    kf = GyroBiasFilter(0.0, 1e-3, CFG)
    run(kf, 0.4, -0.0025, 0.0, 0.0)
    assert kf.b == 0.0 and kf.n_updates == [0, 0]


def test_converges_driving_straight():
    kf = GyroBiasFilter(0.0, 1e-3, CFG)
    run(kf, 600.0, -0.0025 + 0.01, 0.4, 0.01)
    assert kf.b == pytest.approx(-0.0025, abs=1e-4)
    assert kf.n_updates[1] > 0


@pytest.mark.parametrize('v,w', [(0.4, 0.2), (0.01, 0.0)])
def test_no_update_turning_or_creeping(v, w):
    kf = GyroBiasFilter(-0.001, 1e-3, CFG)
    run(kf, 5.0, 0.3, v, w)
    assert kf.b == -0.001 and kf.n_updates == [0, 0]


def test_no_gyro_samples_is_not_an_update():
    kf = GyroBiasFilter(-0.001, 1e-3, CFG)
    run(kf, 5.0, 0.0, 0.0, 0.0, n=0)
    assert kf.b == -0.001


def test_covariance_grows_without_updates():
    kf = GyroBiasFilter(0.0, 1e-3, CFG)
    kf.step(0.0, 0.0, 0, 0.4, 0.2)
    kf.step(100.0, 0.0, 0, 0.4, 0.2)
    assert kf.P == pytest.approx(1e-6 + 1e-10 * 100)

