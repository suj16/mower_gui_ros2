#include "rclcomm.h"

rclcomm::rclcomm() {
    if(Init())
    {
        std::cout<<"初始化成功";
    }
}

rclcomm::~rclcomm()
{
    ShutDown();
}

bool rclcomm::Start()
{
    rclcpp::init(0,nullptr);
    executor_ = std::make_shared<rclcpp::executors::MultiThreadedExecutor>();
    node_ = std::make_shared<rclcpp::Node>("yxrobot_gui");
    executor_->add_node(node_);
    callback_group_laser_ =
        node_->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
    callback_group_other_ =
        node_->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);

    auto sub1_obt = rclcpp::SubscriptionOptions();
    sub1_obt.callback_group = callback_group_other_;
    auto sub_laser_obt = rclcpp::SubscriptionOptions();
    sub_laser_obt.callback_group = callback_group_laser_;

    globalMap_sub_ = node_->create_subscription<nav_msgs::msg::OccupancyGrid>(
        "map",
        rclcpp::QoS(rclcpp::KeepLast(1)).reliable().transient_local(),
        std::bind(&rclcomm::map_callback,this,std::placeholders::_1),
        sub1_obt);

    globalCostMap_sub_ = node_->create_subscription<nav_msgs::msg::OccupancyGrid>(
        "global_costmap/costmap",
        rclcpp::QoS(rclcpp::KeepLast(1)).reliable().transient_local(),
        std::bind(&rclcomm::globalCostMapCallback,this,std::placeholders::_1),
        sub1_obt);

    tf_buffer_ = std::make_unique<tf2_ros::Buffer>(node_->get_clock(), std::chrono::seconds(10));
    transform_listener_ =
        std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

    return true;
}

/**
 * @brief rclcomm::Process
 *  ros2执行函数
 */
void rclcomm::Process()
{
    if(rclcpp::ok())
    {
        getRobotPose();
        executor_->spin_some();
    }
}

bool rclcomm::Init()
{
    if(Start())
    {
        std::cout<<"start ros2 channel success"<<std::endl;
        run_flag_ = true;
        process_thread_ = std::thread([this]()
        {
            while(run_flag_)
            {
                Process();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000/loop_rate_));
            }
        });
        return true;
    }

    return false;
}

void rclcomm::ShutDown()
{
    run_flag_ = false;
    rclcpp::shutdown();
    process_thread_.join();
}

void rclcomm::map_callback(const nav_msgs::msg::OccupancyGrid::SharedPtr msg)
{
    std::cout<<"收到订阅全局地图数据"<<std::endl;
    int width = msg->info.width;
    int height = msg->info.height;
    originX_ = msg->info.origin.position.x;
    originY_ = msg->info.origin.position.y;
    resolution_ = msg->info.resolution;

    OccupancyMap map_data(height,width,resolution_,Eigen::Vector3d(originX_,originY_,0));
    for(size_t i=0;i<msg->data.size();i++){
        int x= int(i/width);
        int y = i%width;
        map_data(x,y) = msg->data[i];
    }
    map_data.SetFlip();
    m_globalMap=std::move(map_data);

    emit emitUpdateMap(m_globalMap);
}

void rclcomm::globalCostMapCallback(const nav_msgs::msg::OccupancyGrid::SharedPtr msg)
{
    int width = msg->info.width;
    int height = msg->info.height;
    double origin_x = msg->info.origin.position.x;
    double origin_y = msg->info.origin.position.y;
    OccupancyMap cost_map(height, width,msg->info.resolution,Eigen::Vector3d(origin_x, origin_y, 0));
    for (int i = 0; i < msg->data.size(); i++) {
        int x = int(i / width);
        int y = i % width;
        cost_map(x, y) = msg->data[i];
    }
    cost_map.SetFlip();
    m_globalCostMap=std::move(cost_map);
    emit emitUpdateGlobalCostMap(m_globalCostMap);
}

void rclcomm::localCostMapCallback(const nav_msgs::msg::OccupancyGrid::SharedPtr msg)
{
    int width = msg->info.width;
    int height = msg->info.height;
    double origin_x = msg->info.origin.position.x;
    double origin_y = msg->info.origin.position.y;
    tf2::Quaternion q;
    tf2::fromMsg(msg->info.origin.orientation,q);
    tf2::Matrix3x3 mat(q);
    double roll,pitch,yaw;
    mat.getEulerYPR(yaw,pitch,roll);
    double origin_theta = yaw;
    QImage map_image(width, height, QImage::Format_ARGB32);
    // emit emitUpdateGlobalCostMap(map_image);
}

void rclcomm::getRobotPose()
{
    auto pose = getTransform("base_link","map");
    emit emitUpdateRobotPose(pose);
}

QImage rclcomm::rotateMapWithY(QImage map)
{
    QImage res=map;
    for(int x=0;x<map.width();x++){
        for(int y=0;y<map.height();y++)
        {
            res.setPixelColor(x,map.height()-y-1,map.pixelColor(x,y));
        }
    }
    return res;
}

QPointF rclcomm::transWorldPoint2Scene(QPointF point)
{
    QPointF ret;
    ret.setX(worldOrigin_.x() + point.x()/resolution_);
    ret.setY(worldOrigin_.y() - point.y()/resolution_);
    return ret;
}

RobotPose rclcomm::getTransform(const std::string& from,const std::string& to)
{
    RobotPose pose;
    try {
        if(!tf_buffer_->canTransform(to,from,tf2::TimePointZero,std::chrono::milliseconds(100)))
            return pose;

        geometry_msgs::msg::TransformStamped transform =
            tf_buffer_->lookupTransform(to, from, tf2::TimePointZero, std::chrono::milliseconds(100));
        geometry_msgs::msg::Quaternion msg_quat = transform.transform.rotation;
        // 转换类型
        tf2::Quaternion q;
        tf2::fromMsg(msg_quat, q);
        tf2::Matrix3x3 mat(q);
        double roll, pitch, yaw;
        mat.getRPY(roll, pitch, yaw);

        pose.x = transform.transform.translation.x;
        pose.y = transform.transform.translation.y;
        pose.theta = yaw;
    }
    catch (tf2::TransformException &ex) {

    }

    return pose;
}

