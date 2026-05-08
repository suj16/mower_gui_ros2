#include "display_robotpose.h"

displayRobotPose::displayRobotPose() {
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptDrops(true);
    setFlag(ItemAcceptsInputMethod, true);
    moveBy(0, 0);
    robotImg_.load(":/sweeper.png");
    QMatrix matrix;
    matrix.rotate(90);
    robotImg_ = robotImg_.transformed(matrix, Qt::SmoothTransformation);
    robotImg_ = robotImg_.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QRectF displayRobotPose::boundingRect() const {
    return QRectF(0, 0, robotImg_.width(), robotImg_.height());
}

void displayRobotPose::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget) {
    painter->setRenderHint(QPainter::Antialiasing, true);  //设置反锯齿 反走样
    painter->save();
    painter->rotate(rad2deg(-currRobotPose_.theta));//旋转到机器人坐标-theta
    painter->drawPixmap(-robotImg_.width() / 2, -robotImg_.height() / 2,
                        robotImg_);
    painter->restore();
}

void displayRobotPose::updatePose(RobotPose pose)
{
    currRobotPose_ = pose;
    update();
    std::cout<<"更新位姿("<<currRobotPose_.x<<","<<currRobotPose_.y<<")"<<std::endl;
}
