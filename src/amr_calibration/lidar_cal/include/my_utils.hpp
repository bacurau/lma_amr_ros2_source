#pragma once

#include <pcl/filters/extract_indices.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/visualization/pcl_visualizer.h>

#include "line.hpp"
#include "rgb.hpp"

class my_utils
{

  using PointType = pcl::PointXYZ;
  using PointRGBType = pcl::PointXYZRGB;
  using Cloud = pcl::PointCloud<PointType>;
  using CloudPtr = pcl::PointCloud<PointType>::Ptr;

public:
  static pcl::PointCloud<pcl::PointXYZ>::Ptr load_cloud(std::string file_name);

  static pcl::PointCloud<pcl::PointXYZRGB>::Ptr
  cloudXYZ2XYZRGB(pcl::PointCloud<pcl::PointXYZ>::Ptr mono_cloud, uint8_t r = 0,
                  uint8_t g = 0, uint8_t b = 0);

  static CloudPtr cloud_without_indices(CloudPtr cloud,
                                        pcl::PointIndices::Ptr inliers)
  {

    // Setup the ExtractIndices filter
    pcl::ExtractIndices<pcl::PointXYZ> extract;
    extract.setInputCloud(cloud);
    extract.setIndices(inliers);
    extract.setNegative(true); // Set to true to exclude the indices

    // Create a new point cloud to hold the result
    CloudPtr p_filtered(new pcl::PointCloud<pcl::PointXYZ>());

    // Apply the filter
    extract.filter(*p_filtered);

    return p_filtered;
  }

  static CloudPtr cloud_only_indices(CloudPtr cloud,
                                     pcl::PointIndices::Ptr inliers)
  {

    CloudPtr in_cloud(new Cloud);
    *in_cloud = Cloud(*cloud, inliers->indices);

    return in_cloud;
  }

  static CloudPtr subtract_cloud(CloudPtr A, CloudPtr B)
  {
    pcl::PointIndices::Ptr to_remove(new pcl::PointIndices());

    for (std::size_t i = 0; i < B->points.size(); i++)
    {
      // TODO clean up
      // bool in = false;
      for (std::size_t j = 0; j < A->points.size(); j++)
      {
        auto point = B->points[i];
        auto other = A->points[j];

        if (point.x == other.x && point.y == other.y)
        {
          to_remove->indices.push_back(j);
          break;
        }
      }
    }
    return cloud_without_indices(A, to_remove);
  }

  // Create a homogenous transformation, from a angle of rotation, and a point
  // as a translation.
  static Eigen::Affine3f homogeneous(double angle, PointType translation)
  {
    auto T = Eigen::Affine3f::Identity();
    T.translation() << translation.x, translation.y, translation.z;
    T.rotate(Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitZ()));
    return T;
  }
};