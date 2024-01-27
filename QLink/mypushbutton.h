#pragma once


#include <QPushButton>
#include <QWidget>

// 自定义按钮类，用于设置按钮图案
class MyPushButton : public QPushButton
{
    Q_OBJECT
  public:
    explicit MyPushButton(QWidget *parent = nullptr);

    void setimage(QPixmap pix); // 设置图案
  protected:
    void paintEvent(QPaintEvent *) override;

  private:
    QPixmap pixmap;

  signals:
};

