#include "mainwindow/map_panel/map_layeritem_virtual.h"

MapLayerItemVirtual::MapLayerItemVirtual(const QString& layerId,
                                         const QString& layerName,
                                         const QString& layerGroup,
                                         QGraphicsItem* parent)
    :QGraphicsObject(parent),
    m_layerId(layerId),
    m_layerName(layerName),
    m_layerGroup(layerGroup)
{
    // setFlag(QGraphicsItem::ItemIsSelectable, false);
    // setFlag(QGraphicsItem::ItemIsMovable, false);
}

