# Source - https://stackoverflow.com/a/25192640
# Posted by Warren Weckesser, modified by community. See post 'Timeline' for change history
# Retrieved 2026-07-03, License - CC BY-SA 4.0

import numpy as np
from scipy.signal import butter, lfilter, freqz
import matplotlib.pyplot as plt
import pandas as pd
import plotly.express as px

def butter_lowpass(cutoff, fs, order=5):
    return butter(order, cutoff, fs=fs, btype='low', analog=False)

def butter_lowpass_filter(data, cutoff, fs, order=5):
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = lfilter(b, a, data)
    return y


# Filter requirements.
order = 5
fs = 400       # sample rate, Hz
cutoff = 2  # desired cutoff frequency of the filter, Hz

# Get the filter coefficients so we can check its frequency response.
b, a = butter_lowpass(cutoff, fs, order)

# Plot the frequency response.
w, h = freqz(b, a, fs=fs, worN=8000)
plt.subplot(2, 1, 1)
plt.plot(w, np.abs(h), 'b')
plt.plot(cutoff, 0.5*np.sqrt(2), 'ko')
plt.axvline(cutoff, color='k')
plt.xlim(0, 0.5*fs)
plt.title("Lowpass Filter Frequency Response")
plt.xlabel('Frequency [Hz]')
plt.grid()



bag_name = "5.298m_straight_line"
topic_name="imu_angular_velocity"
csv_file_path = f"./bags/{bag_name}/{topic_name}/{topic_name}.csv"
csv_file_path2="./bags/only_angular_z_velocity/5.298_400_20_1/ros2_studio_bag_20260706_181855/corrected_angular_velocities_from_imu/corrected_angular_velocities_from_imu.csv"
df = pd.read_csv(csv_file_path)
df2= pd.read_csv(csv_file_path2)

mean = df["vector.z"].mean()
mean2 = df2["vector.z"].mean()

bias_correction = 0 - mean
df["vector.z"] = df["vector.z"] + bias_correction
df2["vector.z"] = df2["vector.z"] -mean2
data = df["vector.z"]

# Demonstrate the use of the filter.
# First make some data to be filtered.
# T = 5        # seconds
# n = int(T * fs) # total number of samples
# t = np.linspace(0, T, n, endpoint=False)
# # "Noisy" data.  We want to recover the 1.2 Hz signal from this.
# data2 = np.sin(1.2*2*np.pi*t) + 1.5*np.cos(9*2*np.pi*t) + 0.5*np.sin(12.0*2*np.pi*t)

# Filter the data, and plot both the original and filtered signals.
y = butter_lowpass_filter(data, cutoff, fs, order)
# y2 = butter_lowpass_filter(data2, cutoff, fs, order)

# plt.subplot(2, 1, 2)
# plt.plot(t, data2, 'b-', label='data')
# plt.plot(t, y2, 'g-', linewidth=2, label='filtered data')
# plt.xlabel('Time [sec]')
# plt.grid()
# plt.legend()

# plt.subplots_adjust(hspace=0.35)

df['new_z']=y
df['filtered_z_by_cpp']=df2["vector.z"]
# Source - https://stackoverflow.com/a/16923367
#df.to_csv("filtered_imu_on_z.csv",sep=',', encoding='utf-8', index=True, header=True)


fig = px.line(
    df,
    x="timestamp",
    y=["vector.z", "new_z","filtered_z_by_cpp"],
    labels={
        "time": "Time [s]",
        "value": "Value",
        "variable": "Axis"
    },
    title="X, Y, Z over time"
)

fig.show()

# fig2 = px.line(
#     df2,
#     x="timestamp",
#     y=["vector.z"],
#     labels={
#         "time": "Time [s]",
#         "value": "Value",
#         "variable": "Axis"
#     },
#     title="X, Y, Z over time"
# )

# fig2.show()

# plt.show()
