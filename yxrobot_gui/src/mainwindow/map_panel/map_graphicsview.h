#ifndef MAP_GRAPHICSVIEW_H
#define MAP_GRAPHICSVIEW_H
#include <QGraphicsView>
#include "mainwindow/map_panel/occmap_layerItem.h"
#include "mainwindow/map_panel/costmap_layerItem.h"
#include "mainwindow/map_panel/robotpose_layerItem.h"
#include "channel/ros2/rclcomm.h"

class MapGraphicsView: public QGraphicsView
{
    Q_OBJECT
public:
    MapGraphicsView(QWidget* parent = nullptr);

private:
    std::shared_ptr<rclcomm> rclcomm_;

    std::shared_ptr<QGraphicsScene> qGraphicScene_;
    std::shared_ptr<OccMapItem> m_occMapItem;
    std::shared_ptr<CostMapItem> m_globalCostMapItem;
    std::shared_ptr<RobotPoseItem> m_robotPoseItem;
};

#endif // MAP_GRAPHICSVIEW_H
