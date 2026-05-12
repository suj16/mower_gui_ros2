#ifndef LASER_LAYERITEM_H
#define LASER_LAYERITEM_H
#include "mainwindow/map_panel/map_layeritem_virtual.h"
#include "common/common.h"
#include <QPainter>
#include <QColor>
#include <QGraphicsItem>
#include <QGraphicsSceneWheelEvent>


class LaserItem : public MapLayerItemVirtual
{
public:
    LaserItem(const QString& id,const QString& name,const int& z,QGraphicsItem* parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;
    QRectF boundingRect() const;

    void UpdateLaserData(const LaserScan& scan);

private:
    void Id2Color(int id, int &R, int &G, int &B);
    void drawLaser(QPainter *painter, int id, std::vector<Point>);
    void computeBoundRect(const std::map<int, std::vector<Point>> &laser_scan);

private:
    std::map<int, QColor> location_to_color_;
    std::map<int, std::vector<Point>> laser_data_scene_;
    QRectF bounding_rect_;
};

#endif // LASER_LAYERITEM_H
