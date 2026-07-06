import numpy as np
import matplotlib.pyplot as plt
from scipy.fft import rfft, rfftfreq
import pandas as pd


bag_name = "robot_not_moving_sqlite"
csv_file_name="imu_angular_velocity.csv"
csv_file_path = f"../../bags/{bag_name}/{csv_file_name}"
df = pd.read_csv(csv_file_path)

data = df["vector.z"]
# Your signal
data = df["vector.z"].to_numpy()

# Sampling rate
fs = 400  # Hz
dt = 1 / fs

# Number of samples
N = len(data)

# Time axis
time = np.arange(N) * dt

# Remove DC offset / mean
data_centered = data - np.mean(data)

# FFT
fft_values = rfft(data_centered)
freqs = rfftfreq(N, dt)

# Amplitude spectrum
amplitude = np.abs(fft_values) / N

# One-sided amplitude correction
if N % 2 == 0:
    amplitude[1:-1] *= 2
else:
    amplitude[1:] *= 2

# Plot original signal
plt.figure(figsize=(12, 4))
plt.plot(time, data)
plt.xlabel("Time [s]")
plt.ylabel("vector.z")
plt.title("Original signal: vector.z")
plt.grid(True)
plt.tight_layout()
plt.show()

# Plot FFT amplitude spectrum
plt.figure(figsize=(12, 4))
plt.plot(freqs, amplitude)
plt.xlabel("Frequency [Hz]")
plt.ylabel("Amplitude")
plt.title("FFT amplitude spectrum of vector.z")
plt.grid(True)
plt.tight_layout()
plt.show()