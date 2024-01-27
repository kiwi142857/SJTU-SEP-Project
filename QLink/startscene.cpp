#include "startscene.h"

#include <QMovie>
#include <QPainter>

StartScene::StartScene(QWidget *parent) : QWidget(parent)
{
    setbackground = 0;
    // this->showMaximized();

    labelbg = new QLabel(this);

    moviebg = new QMovie(":/new/prefix1/rec/background/backgroundGif-.gif");

    newGame = new QPixmap(":/new/prefix2/rec/background/newGame.jpg");
    loadGame = new QPixmap(":/new/prefix2/rec/background/loadGame.jpg");
    exit = new QPixmap(":/new/prefix2/rec/background/exit.jpg");
    backgroundPng = new QPixmap(":/new/prefix2/rec/background/backGround.png");
    singleMode = new QPixmap(":/new/prefix2/rec/background/singleMode.jpg");
    doubleMode = new QPixmap(":/new/prefix2/rec/background/doubleMode.jpg");

    labelbg->setMovie(moviebg); // 1. 设置要显示的 GIF 动画图片
    labelbg->resize(this->width(), this->height());
    /*QObject::connect(moviebg, &QMovie::frameChanged, [=](int frameNumber) {
         // GIF 动画执行一次就结束
         if (frameNumber == moviebg->frameCount() - 1) {
             moviebg->stop();
             labelbg->close();
             setbackground=1;
         }
     });
     moviebg->start();                        // 2. 启动动画
     labelbg->show();*/

    // 设置三个选项按钮
    btnNewGame = new MyPushButton(this);
    btnNewGame->setimage(*newGame);
    btnLoadGame = new MyPushButton(this);
    btnLoadGame->setimage(*loadGame);
    btnExit = new MyPushButton(this);
    btnExit->setimage(*exit);
    btnSingleMode = new MyPushButton(this);
    btnSingleMode->setimage(*singleMode);
    btnDoubleMode = new MyPushButton(this);
    btnDoubleMode->setimage(*doubleMode);

    btnSingleMode->hide();  // 开始时隐藏
    btnDoubleMode->hide();  // 开始时隐藏

    connect(btnSingleMode, &MyPushButton::clicked, [=]() {
        this->hide(); // 按下按钮单人模式
        mode = 1;
    });
    connect(btnSingleMode, &MyPushButton::clicked, [=]() {
        this->hide(); // 按下按钮双人模式
        mode = 2;
    });
}

void StartScene::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    labelbg->resize(this->width(), this->height()); // 标签，播放gif用
    QSize moviesize(this->width(), this->height());
    moviebg->resized(moviesize);

    if (setbackground)
        painter.drawPixmap(0, 0, this->width(), height(),
                           *backgroundPng); // 是否画完背景图，若画完gif，则画出png背景图

    // 画选项
    const int buttonwidth = width() / 10, buttonheight = height() / 10;

    btnNewGame->setGeometry(width() / 12, height() / 10 * 8.5, buttonwidth,
                            buttonheight); // 开始按钮位置设置
    btnLoadGame->setGeometry(3 * width() / 12, height() / 10 * 8.5, buttonwidth,
                             buttonheight); // 载入按钮位置设置
    btnExit->setGeometry(5 * width() / 12, height() / 10 * 8.5, buttonwidth,
                         buttonheight); // 退出按钮位置设置
    btnSingleMode->setGeometry(width() / 12, height() / 10 * 8.5 - buttonheight, buttonwidth,
                               buttonheight); // 单人模式按钮位置设置
    btnDoubleMode->setGeometry(width() / 12, height() / 10 * 8.5 - 2 * buttonheight, buttonwidth,
                               buttonheight); // 双人模式按钮位置设置

    if (setbackground) {     // gif背景图播放完成后
        btnNewGame->show();  // 显示开始
        btnLoadGame->show(); // 显示载入
        btnExit->show();     // 显示退出
    }
}
