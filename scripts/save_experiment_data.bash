#!/bin/bash

. ../install/local_setup.bash

mkdir -p correct_better_zone_marvelmind_bag_data_without_distances
while true
do
    read -p "Point number:" point_number
    #echo "timestamp_ms,x_m,y_m,z_m,flags" >>  ./marvelmind_csv_data/$point_number.csv
    ros2 bag record --all-topics -o ./correct_better_zone_marvelmind_bag_data_without_distances/point_$point_number
    #ros2 topic echo /hedgehog_pos --csv >>  ./marvelmind_csv_data/$point_number.csv
done
