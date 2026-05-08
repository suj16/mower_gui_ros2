#include "robotitem.h"

robotItem::robotItem() {}

void robotItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing,true);
    drawMap(painter);
    drawGlobalCostMap(painter);
}

QRectF robotItem::boundingRect() const
{
    return QRectF(0,0,map_.width(),map_.height());
}

void robotItem::drawMap(QPainter* painter)
{
    painter->drawImage(0,0,map_);
}

void robotItem::drawGlobalCostMap(QPainter* painter)
{
    painter->drawImage(0,0,globalCostMap_);
}

void robotItem::updateGlobalMap(QImage img)
{
    map_ = img;
    update();
}

void robotItem::updateGlobalCostMap(QImage img)
{
    globalCostMap_ = img;
    update();
}

void robotItem::updateRobotPose(RobotPose pose)
{
    currRobotPose_ = pose;
    update();
}

void robotItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    double beforeScaleValue = scaleValue_;
    if(event->delta()>0)
    {
        scaleValue_*=1.1;//每次滚轮放大10%
        //        qDebug()<<"放大"<<m_scaleValue;
    }
    else if(event->delta()<0)
    {
        scaleValue_*=0.9;//每次滚轮缩小10%
        //        qDebug()<<"缩小"<<m_scaleValue;
    }
    setScale(scaleValue_);

    //使放大缩小的效果看起来像是以鼠标中心点进行放大缩小
    if(event->delta()>0)
    {
        moveBy(-event->pos().x()*beforeScaleValue*0.1,-event->pos().x()*beforeScaleValue*0.1);
    }
    else {
        moveBy(event->pos().x()*beforeScaleValue*0.1,event->pos().x()*beforeScaleValue*0.1);
    }
}

void robotItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        //        qDebug()<<"left pressed";
        pressedPose_ = event->pos();
        isMousePress_ = true;
        startPose_ = event->pos();//记录开始按下坐标
    }
    else if(event->button() == Qt::RightButton)
    {
        pressedPose_ = event->pos();
        isMousePress_ = true;
        startPose_ = event->pos();//记录开始按下坐标
        isRotate_ = true;
    }
}

void robotItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (isMousePress_)
    {
        //只有当当前指针为移动指针时，才进行平移（否则会与移动事件冲突）
        if (currCursor_ == moveCursor_)
        {
            QPointF point = (event->pos() - pressedPose_) * scaleValue_;
            moveBy(point.x(), point.y());
        }
        endPose_ = event->pos();//记录结束按下坐标
    }
    update();
}

void robotItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    if(event->button() == Qt::LeftButton)
    {
        pressedPose_ = QPointF();
        isMousePress_ = false;
        // if (currCursor_ == set2DPoseCursor_)//当是定位点时
        // {
        //     emit signalPub2DPose(startPose_, endPose_);//发布位姿起始点坐标和结束点坐标
        //     //鼠标抬起后对指针进行重置
        //     currCursor_ = moveCursor_;
        //     this->setCursor(*currCursor_);
        // }
        // else if (currCursor_ == set2DGoalCursor_)//当是导航目标点时
        // {
        //     emit signalPub2DGoal(startPose_, endPose_);//发布导航目标点起始点和结束点坐标
        //     //鼠标抬起后对指针进行重置
        //     currCursor_ = moveCursor_;
        //     this->setCursor(*moveCursor_);
        // }
        // else if(currCursor_ == clickPointCursor_)//当是点击点时
        // {
        //     emit signalPubClickPoint(startPose_);//发布点击点
        //     //            //鼠标抬起后对指针进行重置
        //     //            currCursor_ = moveCursor_;
        //     //            this->setCursor(*moveCursor_);
        // }

        //鼠标指针置空
        startPose_ = QPointF();
        endPose_ = QPointF();
    }
    else if(event->button() == Qt::RightButton)
    {
        isMousePress_ = false;
        isRotate_ = false;
        pressedPose_ = QPointF();
        //鼠标指针置空
        startPose_ = QPointF();
        endPose_ = QPointF();

    }
    update();
}
