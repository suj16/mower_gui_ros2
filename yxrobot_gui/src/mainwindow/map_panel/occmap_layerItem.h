#ifndef OCCMAP_LAYERITEM_H
#define OCCMAP_LAYERITEM_H

#include <iostream>
#include <QPainter>
#include "mainwindow/map_panel/map_layeritem_virtual.h"
#include "common/common.h"

class OccMapItem : public MapLayerItemVirtual
{
    Q_OBJECT
public:
    OccMapItem(const QString& id,const QString& name,const int& z,QGraphicsItem* parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;
    QRectF boundingRect() const override;



public slots:
    void updateMap(const OccupancyMap& img);

private:
    QImage m_map_image;
    OccupancyMap m_map;
};

#endif // OCCMAP_LAYERITEM_H
