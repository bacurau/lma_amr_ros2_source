


import csv
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np


bags_path="/persistent/scripts/bags/correct_better_zone_marvelmind_bag_data_without_distances/"

# points 1,2,3 and 4, are not the fixed beacons point. They are the mobile beacons place close.
# The comparison will be done between 1 marvelmind and 1.2 real (mobile beacon pose on map).
point_array = ["1","2","3","4","5","6","7","7.1","7.2","7.3","7.4","7.5","8","8.1","8.2","8.3","10.1","10.2"]
mean_dictionary= dict()
X=[]
Y=[]
Z=[]
for point in point_array:

  csv_file_path=bags_path+f"point_{point}/hedgehog_pos/hedgehog_pos.csv"


  # # Read the CSV file (replace 'data.csv' with your file path)
  df = pd.read_csv(csv_file_path,usecols=['x_m','y_m','z_m'])

  # # Calculate column averages
  column_averages = df.mean()
  X.append(np.round(column_averages['x_m'],4))
  Y.append(np.round(column_averages['y_m'],4))
  Z.append(np.round(column_averages['z_m'],4))
  # mean_dictionary[f"{point}"]={
  #     "x": column_averages['x_m'],
  #     "y": column_averages['y_m'],
  #     "z": column_averages['z_m']
  #     }
  # print(f"point {point}: {mean_dictionary[point]}")




plt.grid()

plt.plot(X,Y, 'ro')
for x,y,p in zip(X, Y,point_array):                                       
    plt.annotate(f"{p}:({x}, {y})", xy=(x,y), xytext=(x, y)) 

plt.show()



