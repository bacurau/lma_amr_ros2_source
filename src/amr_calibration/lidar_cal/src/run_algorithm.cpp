#include <iostream>

#include "rclcpp/rclcpp.hpp"

#include <pcl/common/transforms.h>

#include "algorithm.hpp"
#include "debug_viewer.hpp"
#include "my_utils.hpp"

// Keeping this in a separate translation unit makes incremental compilation
// substantially faster.
int main_2(
    rclcpp::Logger logger,
    pcl::PointCloud<pcl::PointXYZ> &cloud_back,
    pcl::PointCloud<pcl::PointXYZ> &cloud_front)
{

    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_back_ptr = cloud_back.makeShared();
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_front_ptr = cloud_front.makeShared();

    ViewerSingleton &viewer = ViewerSingleton::GetInstance();

    viewer.clear();

    // my_utils::add_cloud_with_color(viewer, cloud_front_ptr, RGB(255, 0, 0),
    //                                "cloud front");
    // my_utils::add_cloud_with_color(viewer, cloud_back_ptr, RGB(0, 255, 0),
    //                                "cloud back");

    RCLCPP_DEBUG(logger, "Debug 1");

    // First cloud
    RCLCPP_DEBUG(logger, "Downsampling cloud 1");
    auto cloud_filtered_1 = downsample(cloud_back_ptr);

    std::vector<Line> lines_back;
    RCLCPP_DEBUG(logger, "Decomposing downsampled cloud 1 into lines");
    auto things = into_lines(cloud_filtered_1);
    for (const auto &thing : *things)
    {
        lines_back.push_back(thing.line);

        // For plotting
        auto color = RGB::random();
        viewer.add_cloud_with_color(thing.cloud, color);
        viewer.add_line_with_color(thing.line, color);
    }
    RCLCPP_DEBUG(logger, "Number of lines obtained was: %zu", things->size());

    RCLCPP_DEBUG(logger, "Debug 2");

    // Second cloud
    RCLCPP_DEBUG(logger, "Downsampling cloud 2");
    auto cloud_filtered_2 = downsample(cloud_front_ptr);

    RCLCPP_DEBUG(logger, "decomposing downsampled cloud 2 into lines");
    things = into_lines(cloud_filtered_2);
    RCLCPP_DEBUG(logger, "Number of lines obtained was: %zu", things->size());

    std::vector<Line> lines_front;
    for (const auto &thing : *things)
    {
        lines_front.push_back(thing.line);

        // For plotting
        auto color = RGB::random();
        viewer.add_cloud_with_color(thing.cloud, color);
        viewer.add_line_with_color(thing.line, color);
    }

    viewer.add_cloud_with_color(cloud_filtered_1, RGB(255, 0, 0),
                                "cloud front");
    viewer.add_cloud_with_color(cloud_filtered_2, RGB(0, 255, 0),
                                "cloud back");

    RCLCPP_DEBUG(logger, "Debug 3");

    auto b_corner_lines = find_corner(lines_back);
    if (!b_corner_lines)
    {
        // TODO get logger in function
        // RCLCPP_WARN(this->get_logger(), "Not all cloud msgs have been heard yet, skipping this iteration");
        RCLCPP_DEBUG(logger, "Could not find any corner, bailing");
        return -1;
    }
    auto b_line_1 = b_corner_lines->first;
    auto b_line_2 = b_corner_lines->second;
    auto b_corner = b_line_1.intersection_with(b_line_2);

    RCLCPP_DEBUG(logger, "Debug 3.5");

    auto f_corner_lines = find_corner(lines_front);
    if (!f_corner_lines)
    {
        RCLCPP_DEBUG(logger, "Could not find any corner, bailing");
        return -1;
    }

    auto f_line_1 = f_corner_lines->first;
    auto f_line_2 = f_corner_lines->second;
    auto f_corner = f_line_1.intersection_with(f_line_2);

    viewer.add_circle(b_corner.x, b_corner.y, 0.05);
    viewer.add_circle(f_corner.x, f_corner.y, 0.05);

    RCLCPP_DEBUG(logger, "Debug 4");

    auto T_pb = my_utils::homogeneous(b_line_1.angle() + M_PI, b_corner);
    auto T_pf = my_utils::homogeneous(f_line_2.angle(), f_corner);

    auto H_fb = T_pb * T_pf.inverse();

    RCLCPP_DEBUG(logger, "Computed the transform between bases");

    auto transformed_cloud = pcl::make_shared<Cloud>();
    pcl::transformPointCloud(*cloud_front_ptr, *transformed_cloud, H_fb);

    viewer.add_cloud_with_color(transformed_cloud, RGB(0, 255, 255));

    RCLCPP_DEBUG(logger, "Reached end of algorithm");

    // using namespace std::chrono_literals;
    // std::this_thread::sleep_for(2s);

    return 0;
}
