#pragma once

#include <thread>
#include <iostream>
#include <queue>
#include <mutex>

#include <pcl/visualization/pcl_visualizer.h>

#include "my_utils.hpp"
#include "rgb.hpp"

class ViewerSingleton
{
public:
private:
    pcl::visualization::PCLVisualizer::Ptr viewer_;
    std::queue<std::function<void()>> command_queue_;
    std::mutex command_queue_mutex_;

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
    void spin(std::atomic_bool &viewer_built);

    void enqueue_viewer_command(std::function<void()> f);

    ViewerSingleton();

    ~ViewerSingleton();
};
