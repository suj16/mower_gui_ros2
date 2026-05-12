#include "occmap_layerItem.h"

OccMapItem::OccMapItem(const QString& id,const QString& name,const int& z,QGraphicsItem* parent)
    :MapLayerItemVirtual(id,name,"map",parent)
{
    setZValue(z);
    setAcceptHoverEvents(true);
}

void OccMapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawImage(0,0,m_map_image);
}

QRectF OccMapItem::boundingRect() const
{
    return QRectF(0,0,m_map_image.width(),m_map_image.height());
}

void OccMapItem::updateMap(const OccupancyMap& map)
{
    m_map = map;
    prepareGeometryChange();
    m_map_image = QImage(m_map.Cols(), m_map.Rows(), QImage::Format_ARGB32);
    for (int i = 0; i < m_map.Cols(); i++) {
        for (int j = 0; j < m_map.Rows(); j++) {
            double map_value = m_map(j, i);
            QColor color;
            if (map_value > 0) {
                // 将 map_value 从 0-100 映射到透明度 0-255 范围
                int alpha = static_cast<int>(std::clamp(map_value * 2.55, 0.0, 255.0));
                color = QColor(0, 0, 0, alpha);  // 黑色, 透明度根据占据值动态调整
            } else if (map_value == 0 ) {
                // 自由区域和未知区域都设为白色
                color = Qt::white;
            } else if (map_value == -1) {
                color = Qt::gray;
            } else {
                color = Qt::white;  // 默认白色
            }
            m_map_image.setPixel(i,j,color.rgba());
        }
    }
    update();
}


