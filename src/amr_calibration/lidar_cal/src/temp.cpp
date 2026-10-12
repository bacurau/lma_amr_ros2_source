#include <iostream>

#include "rclcpp/rclcpp.hpp"

#include "algorithm.hpp"
#include "my_utils.hpp"

#include <pcl/common/transforms.h>
#include <pcl/visualization/pcl_visualizer.h>

// Keeping this in a separate translation unit makes incremental compilation
// substantially faster.

int main_2(
    pcl::PointCloud<pcl::PointXYZ> &cloud_back,
    pcl::PointCloud<pcl::PointXYZ> &cloud_front)
{

    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_back_ptr = cloud_back.makeShared();
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_front_ptr = cloud_front.makeShared();

    // auto cloud_back_ptr = my_utils::load_cloud("data/Sick_Cloud_Back.pcd");
    // auto cloud_front_ptr = my_utils::load_cloud("data/Sick_Cloud_Front.pcd");

    pcl::visualization::PCLVisualizer::Ptr viewer(
        new pcl::visualization::PCLVisualizer("3D Viewer"));
    viewer->setBackgroundColor(0, 0, 0);
    // my_utils::add_cloud_with_color(viewer, cloud_front_ptr, RGB(255, 0, 0),
    //                                "cloud front");
    // my_utils::add_cloud_with_color(viewer, cloud_back_ptr, RGB(0, 255, 0),
    //                                "cloud back");

    std::cout << "Debug 1" << std::endl;

    // First cloud
    std::cout << "Downsampling cloud 1" << std::endl;
    auto cloud_filtered_1 = downsample(cloud_back_ptr);

    std::vector<Line> lines_back;
    std::cout << "decomposing downsampled cloud 1 into lines" << std::endl;
    auto things = into_lines(cloud_filtered_1);
    for (const auto &thing : *things)
    {
        lines_back.push_back(thing.line);

        // For plotting
        auto color = RGB::random();
        my_utils::add_cloud_with_color(viewer, thing.cloud, color);
        my_utils::add_line_with_color(viewer, thing.line, color);
    }
    std::cout << "Number of lines obtained was: " << things->size() << std::endl;

    std::cout << "Debug 2" << std::endl;

    // Second cloud
    std::cout << "Downsampling cloud 2" << std::endl;
    auto cloud_filtered_2 = downsample(cloud_front_ptr);

    std::cout << "decomposing downsampled cloud 2 into lines" << std::endl;
    things = into_lines(cloud_filtered_2);
    std::cout << "Number of lines obtained was: " << things->size() << std::endl;

    std::vector<Line> lines_front;
    for (const auto &thing : *things)
    {
        lines_front.push_back(thing.line);

        // For plotting
        auto color = RGB::random();
        my_utils::add_cloud_with_color(viewer, thing.cloud, color);
        my_utils::add_line_with_color(viewer, thing.line, color);
    }

    // my_utils::add_cloud_with_color(viewer, cloud_filtered_1, RGB(255, 0, 0),
    //                                "cloud front");
    // my_utils::add_cloud_with_color(viewer, cloud_filtered_2, RGB(0, 255, 0),
    //                                "cloud back");

    // while (!viewer->wasStopped())
    // {
    //     viewer->spinOnce(100);
    // }

    std::cout << "Debug 3" << std::endl;

    auto b_corner_lines = find_corner(lines_back);
    if (!b_corner_lines)
    {
        // TODO get logger in function
        // RCLCPP_WARN(this->get_logger(), "Not all cloud msgs have been heard yet, skipping this iteration");
        std::cout << "Could not find any corner, bailing" << std::endl;
        return -1;
    }
    auto b_line_1 = b_corner_lines->first;
    auto b_line_2 = b_corner_lines->second;
    auto b_corner = b_line_1.intersection_with(b_line_2);

    std::cout << "Debug 3.5" << std::endl;

    auto f_corner_lines = find_corner(lines_front);
    if (!f_corner_lines)
    {
        std::cout << "Could not find any corner, bailing" << std::endl;
        return -1;
    }
    auto f_line_1 = f_corner_lines->first;
    auto f_line_2 = f_corner_lines->second;
    auto f_corner = f_line_1.intersection_with(f_line_2);

    my_utils::add_circle(viewer, b_corner.x, b_corner.y, 0.05);
    my_utils::add_circle(viewer, f_corner.x, f_corner.y, 0.05);

    std::cout << "Debug 4" << std::endl;

    auto T_pb = my_utils::homogeneous(b_line_1.angle() + M_PI, b_corner);
    auto T_pf = my_utils::homogeneous(f_line_2.angle(), f_corner);

    auto H_fb = T_pb * T_pf.inverse();

    std::cout << "Debug 5" << std::endl;

    auto transformed_cloud = pcl::make_shared<Cloud>();
    pcl::transformPointCloud(*cloud_front_ptr, *transformed_cloud, H_fb);
    // my_utils::add_cloud_with_color(viewer, transformed_cloud, RGB(255, 0, 0));

    while (!viewer->wasStopped())
    {
        viewer->spinOnce(100);
    }

    return 0;
}
