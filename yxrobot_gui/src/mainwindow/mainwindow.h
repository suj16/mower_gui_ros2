#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsItem>
#include "channel/ros2/rclcomm.h"
#include "mainwindow/map_panel/occmap_layerItem.h"
#include "mainwindow/map_panel/robotpose_layerItem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_hide_right_btn_clicked();
    void on_hide_left_btn_clicked();

private:
    Ui::MainWindow *ui;


    int m_hideLeftIndex = 0;
    int m_hideRightIndex = 0;
};
#endif // MAINWINDOW_H
