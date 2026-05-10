#include "mainwindow/map_panel/map_graphicsview.h"
#include <QPainter>

MapGraphicsView::MapGraphicsView(QWidget* parent) :
    QGraphicsView(parent),
    rclcomm_(std::make_shared<rclcomm>())
{
    //初始化地图场景类
    qGraphicScene_ = std::make_shared<QGraphicsScene>();
    qGraphicScene_->clear();

    //初始化Item
    m_occMapItem = std::make_shared<OccMapItem>("map.occMap","occMap",0);
    qGraphicScene_->addItem(m_occMapItem.get());

    m_globalCostMapItem = std::make_shared<CostMapItem>("map.globalCostMap","globalCostMap",10);
    qGraphicScene_->addItem(m_globalCostMapItem.get());

    m_robotPoseItem = std::make_shared<RobotPoseItem>("localization.robot","robot",5);
    qGraphicScene_->addItem(m_robotPoseItem.get());

    this->setScene(qGraphicScene_.get());
    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // 设置视角交互
    this->setDragMode(QGraphicsView::ScrollHandDrag);  // 鼠标拖拽平移
    this->setRenderHint(QPainter::Antialiasing);       // 抗锯齿
    this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);  // 滚轮缩放以鼠标为中心 

    connect(rclcomm_.get(),&rclcomm::emitUpdateMap,m_occMapItem.get(),&OccMapItem::updateMap);
    connect(rclcomm_.get(),&rclcomm::emitUpdateMap,m_robotPoseItem.get(),&RobotPoseItem::updateMap);
    connect(rclcomm_.get(),&rclcomm::emitUpdateGlobalCostMap,m_globalCostMapItem.get(),&CostMapItem::updateMap);
    connect(rclcomm_.get(),&rclcomm::emitUpdateRobotPose,m_robotPoseItem.get(),&RobotPoseItem::updatePose);

    fitInView(qGraphicScene_->sceneRect(), Qt::KeepAspectRatio);
    centerOn(m_robotPoseItem.get());  // 居中到机器人位置
}

