clc
clear all
close all

%file_name = 'imu_angular_velocity2.csv';
file_name = 'filtered_imu_on_z.csv';
table = readtable(file_name);

time = table.(1);
data = table.(6);

fs = 400;
N = length(data);

data_fft = fft(data);
freqs = (0:N-1)*(fs/N);

figure

subplot(2,1,1)
plot(freqs(1:end/2),2/N*abs(data_fft(1:end/2)))

subplot(2,1,2)
plot(freqs(1:end/2),mag2db(2/N*abs(data_fft(1:end/2))))
grid on




