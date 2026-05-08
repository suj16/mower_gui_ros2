#ifndef ROBOTITEM_H
#define ROBOTITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneWheelEvent>//鼠标事件派生类
#include <QCursor>//光标头文件
#include "common.h"

class robotItem : public QObject,public QGraphicsItem
{
    Q_OBJECT
public:
    robotItem();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;
    QRectF boundingRect() const override;

    void wheelEvent(QGraphicsSceneWheelEvent *event) override;//滚轮事件
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;//按下鼠标事件
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;//移动鼠标事件
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;//松开鼠标事件

private:
    void drawMap(QPainter* painter);
    void drawGlobalCostMap(QPainter* painter);

public slots:
    void updateGlobalMap(QImage img);
    void updateGlobalCostMap(QImage img);
    void updateRobotPose(RobotPose pose);

private:
    QImage map_;
    QImage globalCostMap_;
    RobotPose currRobotPose_;

    double scaleValue_=1;//放大缩小比例
    bool isMousePress_ = false;//记录是否被按下
    bool isRotate_ = false;//
    QPointF pressedPose_;//记录按下时的坐标
    QColor laser_color_ = QColor(255,0,0);
    QPointF startPose_;//记录鼠标开始按下的坐标
    QPointF endPose_;//记录鼠标结束按下时的坐标

    //光标对象
    std::shared_ptr<QCursor> currCursor_;
    std::shared_ptr<QCursor> moveCursor_;
    std::shared_ptr<QCursor> set2DPoseCursor_;
    std::shared_ptr<QCursor> set2DGoalCursor_;
    std::shared_ptr<QCursor> clickPointCursor_;
};

#endif // ROBOTITEM_H
