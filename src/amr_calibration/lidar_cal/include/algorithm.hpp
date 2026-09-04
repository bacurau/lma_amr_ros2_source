#include "line.hpp"

#include <pcl/ModelCoefficients.h>
#include <pcl/PointIndices.h>
#include <pcl/point_cloud.h>

using PointType = pcl::PointXYZ;
using PointRGBType = pcl::PointXYZRGB;
using Cloud = pcl::PointCloud<PointType>;
using CloudPtr = pcl::PointCloud<PointType>::Ptr;

struct Thing
{
  CloudPtr cloud;
  Line line;
};

// Computa os pontos iniciais e finais da linha a partir dos model coefficients
// e da nuvem que os gerou.
// Z é ignorado
Line get_line(pcl::ModelCoefficients::Ptr coefficients, CloudPtr cloud);

CloudPtr cloud_only_indices(CloudPtr cloud, pcl::PointIndices::Ptr inliers);

CloudPtr downsample(CloudPtr cloud);

std::unique_ptr<std::vector<pcl::PointIndices>> segment_line(CloudPtr cloud);

std::unique_ptr<std::vector<Thing>> into_lines(CloudPtr cloud);

float dist_p(PointType a, PointType b);

std::pair<Line, Line> find_corner(const std::vector<Line> lines);