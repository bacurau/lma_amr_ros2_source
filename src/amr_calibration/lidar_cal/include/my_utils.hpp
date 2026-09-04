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

  static void
  add_cloud_with_color(pcl::visualization::PCLVisualizer::Ptr viewer,
                       pcl::PointCloud<pcl::PointXYZ>::Ptr cloud,
                       const RGB &color, const std::string &label = "")
  {

    static uint32_t id;
    auto cloud_label =
        label.empty() ? "auto_cloud_" + std::to_string(id++) : label;

    viewer->addPointCloud<pcl::PointXYZRGB>(
        my_utils::cloudXYZ2XYZRGB(cloud, color.r, color.g, color.b),
        cloud_label);

    viewer->setPointCloudRenderingProperties(
        pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 5, cloud_label);
  }

  static void add_line_with_color(pcl::visualization::PCLVisualizer::Ptr viewer,
                                  const Line &line, const RGB &color,
                                  const std::string &label = "")
  {
    static uint32_t id;
    auto line_label =
        label.empty() ? "auto_line_" + std::to_string(id++) : label;

    viewer->addLine(line.m_p1, line.m_p2, color.r / 255.0, color.g / 255.0,
                    color.b / 255.0, line_label);
  }

  static void add_circle(pcl::visualization::PCLVisualizer::Ptr viewer, float x,
                         float y, float r, const std::string &label = "")
  {
    static uint32_t id;
    auto circle_label =
        label.empty() ? "auto_circle_" + std::to_string(id++) : label;

    pcl::ModelCoefficients circle_coeff;
    circle_coeff.values.resize(3); // We need 3 values
    circle_coeff.values[0] = x;
    circle_coeff.values[1] = y;
    circle_coeff.values[2] = r;

    viewer->addCircle(circle_coeff, circle_label);

    viewer->setShapeRenderingProperties(
        pcl::visualization::PCL_VISUALIZER_LINE_WIDTH, 8000, circle_label);
  }

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

    for (auto i = 0; i < B->points.size(); i++)
    {
      bool in = false;
      for (auto j = 0; j < A->points.size(); j++)
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