#pragma once

#include <map.h>
#include <startscene.h>

#include <QFile>
#include <QMainWindow>
#include <QPaintEvent>

class Map;
class StartScene;

// 主窗口，用于展示游戏界面，包括开始界面和游戏界面
class MyMainWindow : public QMainWindow{

    Q_OBJECT
    friend class LinkTest;

  public:
    explicit MyMainWindow(QWidget *parent = nullptr);

  signals:

  private:
    Map *m;        // 地图
    StartScene *s; // 开始主菜单

    MyPushButton Introduction; // 游戏玩法介绍
    void paintEvent(QPaintEvent *) override;
    void setMode(int mode); // 设置游戏人数模式
};

