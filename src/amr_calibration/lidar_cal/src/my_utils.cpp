#include "my_utils.hpp"

#include <pcl/io/pcd_io.h>

using std::cout;
using std::endl;

pcl::PointCloud<pcl::PointXYZ>::Ptr
my_utils::load_cloud(std::string file_name)
{
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);

  if (pcl::io::loadPCDFile<pcl::PointXYZ>(file_name, *cloud) ==
      -1) //* load the file
  {
    std::stringstream a;
    a << "Couldn't read file %s \n"
      << file_name;
    PCL_ERROR(a.str().c_str());
    // TODO generate better error.
    throw std::system_error(EFAULT, std::generic_category());
  }

  cout << "Loaded " << cloud->width * cloud->height << " data points"
       << file_name << endl;
  // << "with the following fields : " << std::endl;

  return cloud;
}

pcl::PointCloud<pcl::PointXYZRGB>::Ptr
my_utils::cloudXYZ2XYZRGB(pcl::PointCloud<pcl::PointXYZ>::Ptr mono_cloud,
                          uint8_t r, uint8_t g, uint8_t b)
{
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);

  auto cloud_rgb = pcl::PointCloud<pcl::PointXYZRGB>::Ptr(
      new pcl::PointCloud<pcl::PointXYZRGB>);

  for (auto &point : mono_cloud->points)
  {
    pcl::PointXYZRGB temp;

    // std::cout << point.intensity << std::endl;

    temp.x = point.x;
    temp.y = point.y;
    temp.z = point.z;
    temp.r = r;
    temp.g = g;
    temp.b = b;
    cloud_rgb->push_back(temp);
  }
  return cloud_rgb;
}