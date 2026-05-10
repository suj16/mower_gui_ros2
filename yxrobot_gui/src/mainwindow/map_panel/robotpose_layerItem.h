#ifndef ROBOTPOSE_LAYERITEM_H
#define ROBOTPOSE_LAYERITEM_H
#include <iostream>
#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneWheelEvent>
#include "common/common.h"
#include "mainwindow/map_panel/map_layeritem_virtual.h"

class RobotPoseItem : public MapLayerItemVirtual
{
    Q_OBJECT
public:
    RobotPoseItem(const QString& id,const QString& name,const int& z,QGraphicsItem* parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    QRectF boundingRect() const override;

public slots:
    void updateMap(const OccupancyMap& map);
    void updatePose(RobotPose pose);

private:
    QPixmap m_robotImg;
    RobotPose m_currRobotPose;
    OccupancyMap m_map;
};

#endif // ROBOTPOSE_LAYERITEM_H
