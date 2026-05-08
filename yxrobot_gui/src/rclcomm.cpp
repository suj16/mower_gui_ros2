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

    QImage map_image(width,height,QImage::Format_RGB32);
    for(size_t i=0;i<msg->data.size();i++){
        int x= i%width;
        int y = int(i/width);
        QColor color;
        if(msg->data[i]==100){
            color = Qt::black;
        }else if(msg->data[i]==0){
            color = Qt::white;
        }else{
            color = Qt::gray;
        }
        map_image.setPixel(x,y,qRgb(color.red(),color.green(),color.blue()));
    }

    //延y翻转地图 因为解析到的栅格地图的坐标系原点为左下角
    //但是图元坐标系为左上角度
    map_image=rotateMapWithY(map_image);
    emit emitUpdateMap(map_image);

    //计算图元坐标系原点在世界坐标系下的坐标（反转后的栅格地图坐标原点在世界坐标系下的坐标）
    double trans_origin_x = originX_;
    double trans_origin_y = originY_ + height*resolution_;
    worldOrigin_.setX(-trans_origin_x/resolution_);
    worldOrigin_.setY(trans_origin_y/resolution_);

}

void rclcomm::globalCostMapCallback(const nav_msgs::msg::OccupancyGrid::SharedPtr msg)
{
    int width = msg->info.width;
    int height = msg->info.height;
    QImage map_image(width, height, QImage::Format_ARGB32);
    for (size_t i = 0; i < msg->data.size(); i++) {
        int x = i % width;
        int y = (int)i / width;
        QColor color;
        int data = msg->data[i];
        if (data >= 100) {
            color.setRgb(0xff, 0x00, 0xff);
            color.setAlpha(50);
        } else if (data >= 90 && data < 100) {
            color.setRgb(0x66, 0xff, 0xff);
            color.setAlpha(50);
        } else if (data >= 70 && data <= 90) {
            color.setRgb(0xff, 0x00, 0x33);
            color.setAlpha(50);
        } else if (data >= 60 && data <= 70) {
            color.setRgb(0xbe, 0x28, 0x1a);
            color.setAlpha(50);
        } else if (data >= 50 && data < 60) {
            color.setRgb(0xBE, 0x1F, 0x58);
            color.setAlpha(50);
        } else if (data >= 40 && data < 50) {
            color.setRgb(0xBE, 0x25, 0x76);
            color.setAlpha(50);
        } else if (data >= 30 && data < 40) {
            color.setRgb(0xBE, 0x2A, 0x99);
            color.setAlpha(50);
        } else if (data >= 20 && data < 30) {
            color.setRgb(0xBE, 0x35, 0xB3);
            color.setAlpha(50);
        } else if (data >= 10 && data < 20) {
            color.setRgb(0xB0, 0x3C, 0xbE);
            color.setAlpha(50);
        } else {
            color = Qt::transparent;
        }
        map_image.setPixelColor(x, y, color);
    }
    map_image = rotateMapWithY(map_image);
    emit emitUpdateGlobalCostMap(map_image);
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
    emit emitUpdateGlobalCostMap(map_image);
}

void rclcomm::getRobotPose()
{
    auto pose = getTransform("base_link","map");
    std::cout<<"getPose:"<<pose.x<<","<<pose.y<<std::endl;
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
        // x y
        double x = transform.transform.translation.x;
        double y = transform.transform.translation.y;
        auto transPose = transWorldPoint2Scene(QPointF(x,y));
        pose.x = transPose.x();
        pose.y = transPose.y();
        pose.theta = yaw;
    }
    catch (tf2::TransformException &ex) {

    }

    return pose;
}

