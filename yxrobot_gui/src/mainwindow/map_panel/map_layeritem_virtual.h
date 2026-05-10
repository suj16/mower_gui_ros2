#ifndef MAP_LAYERITEM_VIRTUAL_H
#define MAP_LAYERITEM_VIRTUAL_H
#include <QGraphicsObject>

class MapLayerItemVirtual : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit MapLayerItemVirtual(const QString& layerId,
                                 const QString& layerName,
                                 const QString& layerGroup,
                                 QGraphicsItem* parent = nullptr);

    QString getLayerId() const{return m_layerId;}
    QString getLayerName() const{return m_layerName;}
    QString getLayerGroup() const{return m_layerGroup;}

private:
    QString m_layerId;
    QString m_layerGroup;
    QString m_layerName;
};

#endif // MAP_LAYERITEM_VIRTUAL_H
