#include "mypushbutton.h"

#include <QPainter>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QPushButton>

MyPushButton::MyPushButton(QWidget *parent) : QPushButton(parent)
{
}

void MyPushButton::paintEvent(QPaintEvent *ev)
{
    // 绘制按钮图片
    QPainter painter(this);
    painter.drawPixmap(0, 0, this->width(), this->height(), pixmap);
}

void MyPushButton::setimage(QPixmap pix)
{
    pixmap = pix; // 设置图像
}
