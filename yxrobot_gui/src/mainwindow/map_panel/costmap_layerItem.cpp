#include "mainwindow/map_panel/costmap_layerItem.h"

CostMapItem::CostMapItem(const QString& id,
                        const QString& name,
                        const int& z,
                        QGraphicsItem* parent)
    :MapLayerItemVirtual(id,name,"map",parent)
{
    setZValue(z);
}

void CostMapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing,true);
    painter->drawImage(0,0,m_map_image);
}

QRectF CostMapItem::boundingRect() const
{
    return QRectF(0,0,m_map_image.width(),m_map_image.height());
}

void CostMapItem::updateMap(const OccupancyMap& map)
{
    m_map = map;
    Eigen::Matrix<Eigen::Vector4i, Eigen::Dynamic, Eigen::Dynamic> cost_map =
        m_map.GetCostMapData();
    m_map_image = QImage(m_map.Cols(), m_map.Rows(),
                        QImage::Format_ARGB32);
    // map_image_.save("./test.png");
    for (int i = 0; i < cost_map.cols(); i++)
        for (int j = 0; j < cost_map.rows(); j++) {
            Eigen::Vector4i color_data = cost_map(j, i);
            QColor color;
            color.setRgb(color_data[0], color_data[1], color_data[2]);
            color.setAlpha(color_data[3]);
            m_map_image.setPixelColor(i, j, color);
        }
    update();
}


