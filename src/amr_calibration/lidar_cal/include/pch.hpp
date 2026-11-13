#pragma once

// STL includes
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <utility>
#include <vector>

// ROS2 includes
#include "pcl_conversions/pcl_conversions.h"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"

// PCL includes
#include <pcl/ModelCoefficients.h>
#include <pcl/PointIndices.h>
#include <pcl/common/transforms.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/impl/point_types.hpp>
#include <pcl/io/pcd_io.h>
#include <pcl/memory.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/point_types_conversion.h>
#include <pcl/search/kdtree.h>
#include <pcl/segmentation/extract_clusters.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/types.h>
#include <pcl/visualization/pcl_visualizer.h>
