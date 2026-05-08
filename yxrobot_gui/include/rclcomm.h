#ifndef RCLCOMM_H
#define RCLCOMM_H

#include <QObject>
#include <thread>
#include <iostream>
//ROS2
#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/occupancy_grid.hpp>
#include <QImage>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_listener.h"
#include "tf2/LinearMath/Quaternion.h"

#include "common.h"


class rclcomm:public QObject
{
    Q_OBJECT
public:
    rclcomm();
    ~rclcomm();
    bool Start();
    void Process();
    bool Init();
    void ShutDown();

    double getMapResolution() const { return resolution_;}

private:
    void map_callback(const nav_msgs::msg::OccupancyGrid::SharedPtr msg);
    void localCostMapCallback(const nav_msgs::msg::OccupancyGrid::SharedPtr msg);
    void globalCostMapCallback(const nav_msgs::msg::OccupancyGrid::SharedPtr msg);
    void getRobotPose();

    QImage rotateMapWithY(QImage map);
    QPointF transWorldPoint2Scene(QPointF point);
    RobotPose getTransform(const std::string& from,const std::string& to);

signals:
    void emitUpdateMap(QImage img);
    void emitUpdateGlobalCostMap(QImage img);
    void emitUpdateRobotPose(RobotPose pose);

private:
    std::thread process_thread_;
    std::atomic<bool> run_flag_{false};
    int loop_rate_{30};

    rclcpp::executors::MultiThreadedExecutor::SharedPtr executor_;
    std::shared_ptr<rclcpp::Node> node_;
    rclcpp::CallbackGroup::SharedPtr callback_group_laser_;
    rclcpp::CallbackGroup::SharedPtr callback_group_other_;

    rclcpp::Subscription<nav_msgs::msg::OccupancyGrid>::SharedPtr globalMap_sub_;
    rclcpp::Subscription<nav_msgs::msg::OccupancyGrid>::SharedPtr globalCostMap_sub_;

    std::unique_ptr<tf2_ros::Buffer> tf_buffer_;
    std::shared_ptr<tf2_ros::TransformListener> transform_listener_;

    RobotPose currPose_;
    double resolution_;
    QPointF worldOrigin_;
};

#endif // RCLCOMM_H
