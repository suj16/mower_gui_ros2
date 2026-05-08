#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsItem>
#include "rclcomm.h"
#include "robotitem.h"
#include "display_robotpose.h"

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
    void updateRobotPose(RobotPose pose);

    void on_pushButton_clicked();

    void on_map_checkbox_stateChanged(int arg1);

    void on_robot_checkbox_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    std::shared_ptr<rclcomm> rclcomm_;

    std::shared_ptr<QGraphicsScene> qGraphicScene_;
    std::shared_ptr<robotItem> robotItem_;
    std::shared_ptr<displayRobotPose> robotPoseItem_;
};
#endif // MAINWINDOW_H
