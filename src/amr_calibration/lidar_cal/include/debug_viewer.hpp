#pragma once

#include <thread>
#include <iostream>
#include <queue>

#include <pcl/visualization/pcl_visualizer.h>

#include "my_utils.hpp"
#include "rgb.hpp"

class ViewerSingleton
{
public:
private:
    pcl::visualization::PCLVisualizer::Ptr viewer_;
    std::queue<std::function<void()>> command_queue_;

public:
    ViewerSingleton(ViewerSingleton &other) = delete;
    void operator=(const ViewerSingleton &) = delete;
    static ViewerSingleton &GetInstance();

    void
    add_cloud_with_color(
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud,
        const RGB &color, const std::string &label = "");

    void add_line_with_color(
        const Line &line, const RGB &color,
        const std::string &label = "");

    void add_circle(float x,
                    float y, float r, const std::string &label = "");

    void clear();

private:
    static void spin(pcl::visualization::PCLVisualizer::Ptr &viewer, std::atomic_bool &viewer_built, std::queue<std::function<void()>> &command_queue);

    ViewerSingleton();

    ~ViewerSingleton();
};
