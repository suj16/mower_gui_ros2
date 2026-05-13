#include "mainwindow/map_panel/map_graphicsview.h"
#include <QPainter>

MapGraphicsView::MapGraphicsView(QWidget* parent) :
    QGraphicsView(parent),
    rclcomm_(std::make_shared<rclcomm>())
{
    //初始化地图场景类
    m_qGraphicScene = new QGraphicsScene();
    this->scale(1,-1);//view沿x轴对称翻转，对其世界坐标系
    this->setScene(m_qGraphicScene);

    //初始化Item
    m_occMapItem = new OccMapItem("map.occMap", "occMap", 0);
    m_qGraphicScene->addItem(m_occMapItem);

    m_globalCostMapItem = new CostMapItem("map.globalCostMap", "globalCostMap", 10);
    m_qGraphicScene->addItem(m_globalCostMapItem);

    m_robotPoseItem = new RobotPoseItem("localization.robot", "robot", 15);
    m_qGraphicScene->addItem(m_robotPoseItem);

    m_laserScanItem = new LaserItem("scan.laser","laser",20);
    m_qGraphicScene->addItem(m_laserScanItem);


    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // // 设置视角交互
    // this->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate); // 更改为智能刷新，提升性能
    // this->setDragMode(QGraphicsView::ScrollHandDrag);                // 允许鼠标中键/左键拖拽平移
    this->setRenderHint(QPainter::Antialiasing);                     // 开启抗锯齿，让轨迹和机器人更平滑
    // this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);  // 滚轮缩放以鼠标为中心

    connect(rclcomm_.get(),&rclcomm::emitUpdateMap,m_occMapItem,&OccMapItem::updateMap);
    connect(rclcomm_.get(),&rclcomm::emitUpdateMap,m_robotPoseItem,&RobotPoseItem::updateMap);
    connect(rclcomm_.get(),&rclcomm::emitUpdateMap,this,&MapGraphicsView::updateMap);
    connect(rclcomm_.get(),&rclcomm::emitUpdateGlobalCostMap,m_globalCostMapItem,&CostMapItem::updateMap);
    connect(rclcomm_.get(),&rclcomm::emitUpdateRobotPose,m_robotPoseItem,&RobotPoseItem::updatePose);
    connect(rclcomm_.get(),&rclcomm::emitUpdateLaserScan,m_laserScanItem,&LaserItem::UpdateLaserData);
}

void MapGraphicsView::updateMap(const OccupancyMap& map)
{
    if(map.isNULL()) return;

    // 直接使用像素尺寸作为 Scene 的边界！
    QRectF mapSceneRect(0, 0, map.width(), map.height());

    m_qGraphicScene->setSceneRect(mapSceneRect);

    focusOnRect(mapSceneRect);
}

void MapGraphicsView::focusOnRect(const QRectF& targetRect)
{
    if(targetRect.isNull() || targetRect.isEmpty()) {
        return;
    }

    // 1. 创建基础矩形并计算 Padding (留出 5% 空白边缘)
    double paddingX = targetRect.width() * 0.05;
    double paddingY = targetRect.height() * 0.05;

    // 2. 扩展矩形
    QRectF paddedRect = targetRect.adjusted(-paddingX, -paddingY, paddingX, paddingY);

    // 3. 视角自适应 (保持比例不变)
    this->fitInView(paddedRect, Qt::KeepAspectRatio);
}

// 鼠标按下：记录起始点，开启拖拽状态
void MapGraphicsView::mousePressEvent(QMouseEvent *event)
{
    // 习惯上：左键或中键都可以拖拽地图 (类似 RViz)
    if (event->button() == Qt::LeftButton || event->button() == Qt::MiddleButton) {
        m_lastMousePos = event->pos();
        m_isDragging = true;
        setCursor(Qt::ClosedHandCursor); // 变成抓取的“小手”图标
        event->accept();
        return;
    }
    // 其他按键交给父类处理
    QGraphicsView::mousePressEvent(event);
}

// 鼠标移动：计算差值，移动滚动条
void MapGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging) {
        // 计算鼠标移动的像素差值
        QPoint delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();

        // 核心：反向操作滚动条来实现画布的平移
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());

        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

// 鼠标释放：恢复状态
void MapGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton || event->button() == Qt::MiddleButton) {
        m_isDragging = false;
        setCursor(Qt::ArrowCursor); // 恢复正常鼠标图标
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

// 进阶优化：自己接管滚轮缩放（比内置的更平滑，且可以限制缩放比例）
void MapGraphicsView::wheelEvent(QWheelEvent *event)
{
    // 获取当前缩放矩阵的值（防止无限缩小导致崩溃）
    double currentScale = transform().m11();

    double scaleFactor = 1.15; // 每次滚动放大/缩小 15%

    if (event->angleDelta().y() > 0) {
        // 向上滚，放大
        if (currentScale < 20.0) { // 最大放大倍数限制
            scale(scaleFactor, scaleFactor);
        }
    } else {
        // 向下滚，缩小
        if (currentScale > 0.01) { // 最小缩小倍数限制
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
    }

    QRect viewportRect = this->viewport()->rect();

    // 将屏幕的四个角映射到物理世界 (Scene) 坐标系中
    QPointF viewTL = this->mapToScene(viewportRect.topLeft());
    QPointF viewTR = this->mapToScene(viewportRect.topRight());
    QPointF viewBL = this->mapToScene(viewportRect.bottomLeft());
    QPointF viewBR = this->mapToScene(viewportRect.bottomRight());

    // ==========================================
    // 🎯 激光图层 (boundingRect) 的四个角坐标获取
    // ==========================================
    // 假设你在 View 中有 m_laserItem 的指针，获取它在全局世界里的结界框
    // (如果你想看 OccMapItem，替换成 m_occMapItem 即可)
    QRectF itemRect = m_laserScanItem->sceneBoundingRect();

    QPointF itemTL = itemRect.topLeft();
    QPointF itemTR = itemRect.topRight();
    QPointF itemBL = itemRect.bottomLeft();
    QPointF itemBR = itemRect.bottomRight();

    // ==========================================
    // 🖨️ 格式化打印输出
    // ==========================================
    qDebug() << "================= 坐标四角 Debug =================";
    qDebug() << "[ 屏幕视角 (View) 在世界中的范围 ]:";
    qDebug() << "左上角:" << viewTL << " | " << "右上角:" << viewTR;
    qDebug() << "左下角:" << viewBL << " | " << "右下角:" << viewBR;
    qDebug() << "--------------------------------------------------";
    qDebug() << "[ 图层结界 (boundingRect) 的范围 ]:";
    qDebug() << "左上角:" << itemTL << " | " << "右上角:" << itemTR;
    qDebug() << "左下角:" << itemBL << " | " << "右下角:" << itemBR;
    qDebug() << "==================================================\n";

    QGraphicsView::wheelEvent(event);
}

