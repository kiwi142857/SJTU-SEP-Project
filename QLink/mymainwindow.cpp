#include "mymainwindow.h"

#include <QMovie>
#include <iostream>

#include "mypushbutton.h"
#include "startscence.h"

// 主窗口，用于展示游戏界面，包括开始界面和游戏界面
MyMainWindow::MyMainWindow(QWidget *parent) : QMainWindow(parent)
{
    this->setWindowIcon(QIcon(":/new/prefix2/rec/emoji/icon.png"));
    m = new Map(this); // 地图创建
    s = new StartScene(this); // 开始界面创建
    m->musicPlay(); // 设置音乐
    Introduction.setParent(this);
    Introduction.setimage(QPixmap(":/new/prefix1/rec/background/introduction3.png"));
    // 按下introduction表示结束
    connect(&Introduction, &QPushButton::clicked, [=]() {
        Introduction.hide();// 隐藏介绍
        s->show(); // 显示开始界面
        QObject::connect(s->moviebg, &QMovie::frameChanged, [=](int frameNumber) {
            // GIF 动画执行一次就结束
            if (frameNumber == s->moviebg->frameCount() - 1) {
                s->moviebg->stop();
                s->labelbg->close();
                s->setbackground = 1;
            }
        });
        s->moviebg->start(); // 2. 启动动画
        s->labelbg->show();  // 显示gif
    });
    // 按下doubleMode，开启双人模式
    connect(s->btnDoubleMode, &MyPushButton::clicked, [=]() { setMode(2); });
    // 按下singleMode，开启单人模式
    connect(s->btnSingleMode, &MyPushButton::clicked, [=]() { setMode(1); });
    // 按下exit，退出游戏
    connect(s->btnExit, &MyPushButton::clicked, [=]() {
        s->close();
        this->close();
    });
    // 载入游戏
    connect(s->btnLoadGame, &MyPushButton::clicked, [=]() { setMode(0); });// 读档
    connect(s->btnNewGame, &MyPushButton::clicked, [=]() { // 开始新游戏
        s->btnDoubleMode->show();
        s->btnSingleMode->show();
    });
    // 设置父窗口
    s->setParent(this);
    // 填充屏幕
    this->showMaximized();
    s->hide();
    Introduction.show(); // 显示介绍
}

// 重写paintEvent事件，设置背景
void MyMainWindow::paintEvent(QPaintEvent *)
{
    this->setWindowTitle("EmojiLink");
    s->resize(this->width(), this->height());
    m->resize(this->width(), this->height());
    Introduction.resize(this->width(), this->height());
}

// 设置游戏人数模式
void MyMainWindow::setMode(int mode)
{
    if (m != nullptr) {
        if (m->music)
            m->music->stop();
        delete m;
    }
    m = new Map(this);
    m->musicPlay(); // 设置音乐
    m->show();      // 打开map界面
    s->close();     // 关闭Startscene界面
    if (mode == 0) {
        try {
            m->readData(); // 读档
        } catch (QString s) {
            m->readDataError();
        }
    } else
        m->setMode(mode); // 设置双人模式
    m->setInit = 0;       // 重置初始化为0
    m->initial();
    m->setInit = 0; // 重置初始化为0
    s->btnDoubleMode->hide();
    s->btnSingleMode->hide();

    // 按下backStart,返回菜单栏
    connect(&m->backstart, &QPushButton::clicked, [=]() {
        m->hide();
        s->show();
    });
    connect(&m->clickToContinue, &QPushButton::clicked, [=]() { // 按下继续游戏
        m->close();
        s->show();
    });
    connect(&m->stopGame, &QPushButton::clicked, [=]() { // 按下停止游戏
        m->timer->stop();
        m->timer2->stop();
        m->setMovable(0, 0);
        m->stopGame.hide();
        m->continueGame.show();
    });
    connect(&m->continueGame, &QPushButton::clicked,
            [=]() { // 按下继续游戏（暂停）
                m->timer->start(1000);
                m->timer2->start(6000);
                m->setMovable(1, 1);
                m->continueGame.hide();
                m->stopGame.show();
            });
    m->downloadError.setParent(this);
    connect(&m->downloadError, &QPushButton::clicked, [=]() { // 存档失败处理
        m->downloadError.hide();
        m->afterError();
    });
}
