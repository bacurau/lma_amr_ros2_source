#include <iostream>

#include "algorithm.hpp"
#include "my_utils.hpp"

#include <pcl/common/transforms.h>
#include <pcl/visualization/pcl_visualizer.h>

int main_2()
{

    auto cloud_back = my_utils::load_cloud("data/Sick_Cloud_Back.pcd");
    auto cloud_front = my_utils::load_cloud("data/Sick_Cloud_Front.pcd");

    pcl::visualization::PCLVisualizer::Ptr viewer(
        new pcl::visualization::PCLVisualizer("3D Viewer"));
    viewer->setBackgroundColor(0, 0, 0);
    // my_utils::add_cloud_with_color(viewer, cloud_front, RGB(255, 0, 0),
    //                                "cloud front");
    my_utils::add_cloud_with_color(viewer, cloud_back, RGB(0, 255, 0),
                                   "cloud back");

    // First cloud
    auto cloud_filtered = downsample(cloud_back);

    std::vector<Line> lines_back;
    auto things = into_lines(cloud_filtered);
    for (const auto &thing : *things)
    {
        lines_back.push_back(thing.line);

        // For plotting
        auto color = RGB::random();
        // my_utils::add_cloud_with_color(viewer, thing.cloud, color);
        // my_utils::add_line_with_color(viewer, thing.line, color);
    }

    // Second cloud
    cloud_filtered = downsample(cloud_front);

    things = into_lines(cloud_filtered);
    std::vector<Line> lines_front;
    for (const auto &thing : *things)
    {
        lines_front.push_back(thing.line);

        // For plotting
        auto color = RGB::random();
        // my_utils::add_cloud_with_color(viewer, thing.cloud, color);
        // my_utils::add_line_with_color(viewer, thing.line, color);
    }

    auto b_corner_lines = find_corner(lines_back);
    auto b_line_1 = b_corner_lines.first;
    auto b_line_2 = b_corner_lines.second;
    auto b_corner = b_line_1.intersection_with(b_line_2);

    auto f_corner_lines = find_corner(lines_front);
    auto f_line_1 = f_corner_lines.first;
    auto f_line_2 = f_corner_lines.second;
    auto f_corner = f_line_1.intersection_with(f_line_2);

    // my_utils::add_circle(viewer, b_corner.x, b_corner.y, 0.05);
    // my_utils::add_circle(viewer, f_corner.x, f_corner.y, 0.05);

    auto T_pb = my_utils::homogeneous(b_line_1.angle() + M_PI, b_corner);
    auto T_pf = my_utils::homogeneous(f_line_2.angle(), f_corner);

    auto H_fb = T_pb * T_pf.inverse();

    auto transformed_cloud = pcl::make_shared<Cloud>();
    pcl::transformPointCloud(*cloud_front, *transformed_cloud, H_fb);
    my_utils::add_cloud_with_color(viewer, transformed_cloud, RGB(255, 0, 0));

    while (!viewer->wasStopped())
    {
        viewer->spinOnce(100);
    }

    return 0;
}
