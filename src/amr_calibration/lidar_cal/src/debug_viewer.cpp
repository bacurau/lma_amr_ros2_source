#include "debug_viewer.hpp"

#include <thread>
#include <iostream>

#include <pcl/visualization/pcl_visualizer.h>

#include "my_utils.hpp"
#include "rgb.hpp"

ViewerSingleton &ViewerSingleton::GetInstance()
{
    static ViewerSingleton instance;
    return instance;
}

void ViewerSingleton::spin(std::atomic_bool &viewer_built)
{
    // The visualizer must be created in the same thread as it is spun,
    // otherwise there are problems with OpenGL context sharing.
    viewer_ = pcl::visualization::PCLVisualizer::Ptr(new pcl::visualization::PCLVisualizer("3D Viewer"));
    viewer_->setBackgroundColor(0, 0, 0);

    viewer_built = true;

    while (!viewer_->wasStopped())
    {
        // TODO add mutex
        std::function<void()> command;

        while (!command_queue_.empty())
        {
            {
                const std::lock_guard<std::mutex> lock(command_queue_mutex_);
                command = std::move(command_queue_.front());
                command_queue_.pop();
            }

            if (command)
                command();
        }
        int fps = 60;

        viewer_->spinOnce(1000 / fps);
    }

    std::cout << "I'm out" << std::endl;
    std::abort();
}

ViewerSingleton::ViewerSingleton() : viewer_(), command_queue_()
{
    using namespace std::chrono_literals;

    std::atomic_bool viewer_built = false;

    static std::thread spinner_thread_ = std::thread(&ViewerSingleton::spin, this, std::ref(viewer_built));

    while (!viewer_built)
    {
        // Spin while waiting for the viewer to be built, as it is really
        // slow, not doing this would generate race conditions.
    }
}
ViewerSingleton::~ViewerSingleton() {}

void ViewerSingleton::enqueue_viewer_command(std::function<void()> f)
{
    const std::lock_guard<std::mutex> lock(command_queue_mutex_);
    command_queue_.emplace(f);
}

void ViewerSingleton::add_cloud_with_color(
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud,
    const RGB &color, const std::string &label)
{
    // TODO maybe change all ids, from static to a member variable
    static uint32_t id;
    auto cloud_label =
        label.empty() ? "auto_cloud_" + std::to_string(id++) : label;

    enqueue_viewer_command(
        [=]()
        {
            viewer_->addPointCloud<pcl::PointXYZRGB>(
                my_utils::cloudXYZ2XYZRGB(cloud, color.r, color.g, color.b),
                cloud_label);

            viewer_->setPointCloudRenderingProperties(
                pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 5, cloud_label);
        });
}

void ViewerSingleton::add_line_with_color(
    const Line &line, const RGB &color,
    const std::string &label)
{
    static uint32_t id;
    enqueue_viewer_command(
        [=]()
        {
            auto line_label =
                label.empty() ? "auto_line_" + std::to_string(id++) : label;

            viewer_->addLine(line.m_p1, line.m_p2, color.r / 255.0, color.g / 255.0,
                             color.b / 255.0, line_label);
        });
}

void ViewerSingleton::add_circle(float x,
                                 float y, float r, const std::string &label)
{
    static uint32_t id;
    enqueue_viewer_command(
        [=]()
        {
            auto circle_label =
                label.empty() ? "auto_circle_" + std::to_string(id++) : label;

            pcl::ModelCoefficients circle_coeff;
            circle_coeff.values.resize(3); // We need 3 values
            circle_coeff.values[0] = x;
            circle_coeff.values[1] = y;
            circle_coeff.values[2] = r;

            viewer_->addCircle(circle_coeff, circle_label);

            viewer_->setShapeRenderingProperties(
                pcl::visualization::PCL_VISUALIZER_LINE_WIDTH, 8000, circle_label);
        });
}

void ViewerSingleton::clear()
{
    // TODO add mutex
    enqueue_viewer_command(
        [=]()
        {
            viewer_->removeAllPointClouds();
            viewer_->removeAllShapes();
            viewer_->removeAllCoordinateSystems();
        });
}
