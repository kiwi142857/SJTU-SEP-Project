#pragma once

#include <QLabel>
#include <QWidget>

#include "linktest.h"
#include "mypushbutton.h"

// 开局的初始化界面，用来展示可玩菜单
class StartScene : public QWidget{

    Q_OBJECT
    friend class LinkTest;

  public:
    explicit StartScene(QWidget *parent = nullptr);

    // void keyPressEvent(QKeyEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

    MyPushButton *btnNewGame;    // 新游戏
    MyPushButton *btnLoadGame;   // 加载游戏
    MyPushButton *btnExit;       // 退出游戏
    MyPushButton *btnSingleMode; // 单人模式选项
    MyPushButton *btnDoubleMode; // 双人模式选项

    int mode;           // 游戏人数
    bool setbackground; // 是否设置背景

    QPixmap *background; // 背景图
    QLabel *labelbg;     // 放置gif背景图
    QMovie *moviebg;     // gif背景图

    // 开局选项
    QPixmap *newGame;
    QPixmap *loadGame;
    QPixmap *exit;
    QPixmap *backgroundPng;
    QPixmap *singleMode;
    QPixmap *doubleMode;
  signals:

  private:
};

