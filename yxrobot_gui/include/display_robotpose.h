#ifndef DISPLAY_ROBOTPOSE_H
#define DISPLAY_ROBOTPOSE_H
#include <iostream>
#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneWheelEvent>
#include "common.h"

class displayRobotPose : public QObject,public QGraphicsItem
{
    Q_OBJECT
public:
    displayRobotPose();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    QRectF boundingRect() const override;
    void updatePose(RobotPose pose);

private:
    QPixmap robotImg_;
    RobotPose currRobotPose_;
};

#endif // DISPLAY_ROBOTPOSE_H
