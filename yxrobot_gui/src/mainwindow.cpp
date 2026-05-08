#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , rclcomm_(std::make_shared<rclcomm>())
{
    ui->setupUi(this);

    //初始化地图场景类
    qGraphicScene_ = std::make_shared<QGraphicsScene>();
    qGraphicScene_->clear();

    //初始化Item
    robotItem_ = std::make_shared<robotItem>();
    robotItem_->setZValue(1);//设置图层优先级,优先级越高显示越外面
    qGraphicScene_->addItem(robotItem_.get());

    robotPoseItem_ = std::make_shared<displayRobotPose>();
    robotPoseItem_->setZValue(10);//设置图层优先级,优先级越高显示越外面
    qGraphicScene_->addItem(robotPoseItem_.get());

    ui->graphicsView->setScene(qGraphicScene_.get());
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    connect(rclcomm_.get(),&rclcomm::emitUpdateMap,robotItem_.get(),&robotItem::updateGlobalMap);
    connect(rclcomm_.get(),&rclcomm::emitUpdateGlobalCostMap,robotItem_.get(),&robotItem::updateGlobalCostMap);
    connect(rclcomm_.get(),&rclcomm::emitUpdateRobotPose,this,&MainWindow::updateRobotPose);
}

void MainWindow::updateRobotPose(RobotPose pose)
{
    robotItem_->updateRobotPose(pose);//RobotPose全局更新
    QPointF pos;
    pos.setX(pose.x);
    pos.setY(pose.y);
    QPointF scenePose = robotItem_->mapToScene(pos);//图层坐标转换map->场景坐标
    robotPoseItem_->updatePose(pose);
    robotPoseItem_->setPos(scenePose.x(), scenePose.y());//设置场景坐标系下坐标
}

MainWindow::~MainWindow()
{
    delete ui;
}

