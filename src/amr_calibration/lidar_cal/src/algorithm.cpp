#include "algorithm.hpp"
#include "my_utils.hpp"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include <pcl/ModelCoefficients.h>
#include <pcl/PointIndices.h>
#include <pcl/common/transforms.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/impl/point_types.hpp>
#include <pcl/memory.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/point_types_conversion.h>
#include <pcl/search/kdtree.h>
#include <pcl/segmentation/extract_clusters.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/types.h>

// Computa os pontos iniciais e finais da linha a partir dos model coefficients
// e da nuvem que os gerou.
// Z é ignorado
Line get_line(pcl::ModelCoefficients::Ptr coefficients, CloudPtr cloud)
{
  pcl::PointXYZ min_p = cloud->points[0];
  pcl::PointXYZ max_p = cloud->points[0];

  // Find extremes of the points
  for (auto point : cloud->points)
  {
    // ! Fazer a comparação só com x pode dar ruim
    if ((point.x < min_p.x))
    {
      min_p = point;
    }
    else if ((point.x > max_p.x))
    {
      max_p = point;
    }
  }

  float x_1 = coefficients->values[0];
  float y_1 = coefficients->values[1];
  float x_2 = x_1 + coefficients->values[3];
  float y_2 = y_1 + coefficients->values[4];

  const Line line(PointType(x_1, y_1, 0), PointType(x_2, y_2, 0));

  return Line(line.point_proj(max_p), line.point_proj(min_p));
}

// Find one line from a cloud point using sac segmentation, this line can be
// discontinuous or made of points that are not close and doesn't really make
// sense.
Thing get_one_line(CloudPtr cloud)
{

  pcl::ModelCoefficients::Ptr coefficients(new pcl::ModelCoefficients());
  pcl::PointIndices::Ptr inliers(new pcl::PointIndices());

  // Create the segmentation object
  pcl::SACSegmentation<pcl::PointXYZ> seg;
  seg.setOptimizeCoefficients(true);
  seg.setModelType(pcl::SACMODEL_LINE);
  seg.setMethodType(pcl::SAC_RANSAC);
  seg.setMaxIterations(1000);
  // seg.setDistanceThreshold(0.01);
  seg.setDistanceThreshold(0.02);
  // TODO is this really useful?
  seg.setOptimizeCoefficients(true);
  // seg.setOptimizeCoefficients(false);

  seg.setInputCloud(cloud);

  seg.setEpsAngle(0.01);

  // Obtain the line coefficients
  seg.segment(*inliers, *coefficients);

  auto cloud_line = pcl::make_shared<Cloud>(*cloud, inliers->indices);

  auto line = get_line(coefficients, cloud_line);

  return Thing{cloud_line, line};
}

CloudPtr downsample(CloudPtr cloud)
{
  // Preprocessing: Downsample the point cloud
  pcl::VoxelGrid<pcl::PointXYZ> voxel_grid;
  voxel_grid.setInputCloud(cloud);
  voxel_grid.setLeafSize(0.01f, 0.01f, 0.01f);
  CloudPtr cloud_filtered(new Cloud);
  voxel_grid.filter(*cloud_filtered);
  return cloud_filtered;
}

uint32_t global_id = 0;

std::unique_ptr<std::vector<pcl::PointIndices>> segment_line(CloudPtr cloud)
{
  pcl::search::KdTree<PointType>::Ptr tree(new pcl::search::KdTree<PointType>);

  tree->setInputCloud(cloud);
  std::vector<pcl::PointIndices> cluster_indices;
  pcl::EuclideanClusterExtraction<PointType> ec;
  ec.setClusterTolerance(0.2);
  ec.setMinClusterSize(15);
  ec.setMaxClusterSize(1000);
  ec.setSearchMethod(tree);
  ec.setInputCloud(cloud);
  ec.extract(cluster_indices);

  auto cluster_indices_ptr =
      std::make_unique<std::vector<pcl::PointIndices>>(cluster_indices);

  return cluster_indices_ptr;
}

// Segments a cloud into all the lines that forms it.
std::unique_ptr<std::vector<Thing>> into_lines(CloudPtr cloud)
{
  auto things = std::make_unique<std::vector<Thing>>();

  auto old_cloud = cloud;

  while (true)
  {
    if (old_cloud->points.size() < 5)
    {
      std::cout << "Low on points, bailing" << std::endl;
      break;
    }
    auto new_cloud = old_cloud;

    auto inlier_cloud = get_one_line(old_cloud).cloud;
    auto clusters = segment_line(inlier_cloud);

    // Remove all points that form a line from the new cloud
    for (auto cluster : *clusters)
    {
      auto temp_cloud = pcl::make_shared<Cloud>(*inlier_cloud, cluster.indices);
      things->push_back(get_one_line(temp_cloud));
      new_cloud = my_utils::subtract_cloud(new_cloud, temp_cloud);
    }

    auto old_size = old_cloud->points.size();
    auto new_size = new_cloud->points.size();

    // If no point was removed, no line was found. Remove every point that does
    // not qualify to be part of a line.
    if (old_size == new_size)
    {
      new_cloud = my_utils::subtract_cloud(old_cloud, inlier_cloud);
    }

    old_cloud = new_cloud;
  }

  return things;
}

float dist_p(PointType a, PointType b)
{
  auto p1 = a.getVector3fMap();
  auto p2 = b.getVector3fMap();

  return (p1 - p2).norm();
}

// TODO make this return a option instead of crashing
std::pair<Line, Line> find_corner(const std::vector<Line> lines)
{
  const float eps_ang = DEG2RAD(10);
  const float tgt_ang = DEG2RAD(90);
  const float dist_threshold = 0.2;

  // Line line_1, line_2;
  if (lines.size() < 2)
  {
    std::cerr << "Not enough lines to find a corner" << std::endl;
    throw std::system_error(EFAULT, std::generic_category());
  }
  for (auto i{0}; i < lines.size(); i++)
  {
    for (auto j{i}; j < lines.size(); j++)
    {
      if (j == i)
      {
        continue;
      }
      auto line_1 = lines[i];
      auto line_2 = lines[j];
      auto ang = line_1.smallest_angle_between(line_2);

      if (((ang > tgt_ang + eps_ang) || (ang < tgt_ang - eps_ang)))
      {
        // Outside of angle range
        continue;
      }
      auto p1_1 = line_1.m_p1;
      auto p1_2 = line_1.m_p2;
      auto p2_1 = line_2.m_p1;
      auto p2_2 = line_2.m_p2;

      if (dist_p(p1_1, p2_1) < dist_threshold ||
          dist_p(p1_1, p2_2) < dist_threshold ||
          dist_p(p1_2, p2_1) < dist_threshold ||
          dist_p(p1_2, p2_2) < dist_threshold)
      {
        return std::make_pair(line_1, line_2);
      }
    }
  }

  std::cerr << "Didn't find any corner" << std::endl;

  throw std::system_error(EFAULT, std::generic_category());
}