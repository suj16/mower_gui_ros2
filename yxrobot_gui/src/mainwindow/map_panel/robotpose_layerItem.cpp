#include "robotpose_layerItem.h"

RobotPoseItem::RobotPoseItem(const QString& id,const QString& name,const int& z,QGraphicsItem* parent)
    :MapLayerItemVirtual(id,name,"localization",parent)
{
    setZValue(z);
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptDrops(true);
    setFlag(ItemAcceptsInputMethod, true);
    moveBy(0, 0);
    m_robotImg.load(":/sweeper.png");
    QMatrix matrix;
    matrix.rotate(90);
    m_robotImg = m_robotImg.transformed(matrix, Qt::SmoothTransformation);
    m_robotImg = m_robotImg.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QRectF RobotPoseItem::boundingRect() const {
    return QRectF(0, 0, m_robotImg.width(), m_robotImg.height());
}

void RobotPoseItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget) {
    painter->setRenderHint(QPainter::Antialiasing, true);  //设置反锯齿 反走样
    painter->save();
    painter->rotate(rad2deg(-m_currRobotPose.theta));//旋转到机器人坐标-theta
    painter->drawPixmap(-m_robotImg.width() / 2, -m_robotImg.height() / 2,
                        m_robotImg);
    painter->restore();
}

void RobotPoseItem::updateMap(const OccupancyMap& map)
{
    m_map = map;
}

void RobotPoseItem::updatePose(RobotPose pose)
{

    QPointF pos;
    pos.setX(pose.x);
    pos.setY(pose.y);
    // QPointF scenePose = occMapItem_->mapToScene(pos);//图层坐标转换map->场景坐标
    m_currRobotPose = pose;
    RobotPose robotScenePose;
    m_map.worldPose2Scene(pose.x,pose.y,robotScenePose.x,robotScenePose.y);
    std::cout<<"robotScenePose:"<<robotScenePose.x<<","<<robotScenePose.y<<std::endl;
    std::cout<<"getPose:"<<pose.x<<","<<pose.y<<std::endl;
    setPos(robotScenePose.x, robotScenePose.y);//设置场景坐标系下坐标
    update();
}
