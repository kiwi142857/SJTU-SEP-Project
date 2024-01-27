#pragma once

// 格式规范 ： Microsotf (modified accroding to the requirement of this project)
#include <mypushbutton.h>

#include <QKeyEvent>
#include <QLCDNumber>
#include <QLabel>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMouseEvent>
#include <QPainter>
#include <QPoint>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>
#include <QWidget>

#include "linktest.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class Map;
}
QT_END_NAMESPACE

const int xdivide = 20, ydivide = 12; // x,y轴划分为几个格

// 游戏界面，用于展示游戏界面
class Map : public QWidget{

    Q_OBJECT
    friend class LinkTest;

  public:
    Map(QWidget *parent = nullptr);
    ~Map();

    MyPushButton backstart;       // 返回菜单按钮
    MyPushButton clickToContinue; // 游戏结束后按钮
    MyPushButton stopGame;        // 暂停游戏
    MyPushButton continueGame;    // 继续游戏
    MyPushButton upLoad;          // 存档
    MyPushButton downLoad;        // 读档
    MyPushButton menu;            // 菜单栏
    MyPushButton downloadError;   // 未上传，无存档处理
    MyPushButton Introduction;    // 简介

    QTimer *timer;                     // 计时器1，用于倒计时
    QTimer *timer2;                    // 计时器2，用于道具掉落
    bool setInit;                      // 是否初始化
    bool m1, m2;                       // 临时保存movable1,2值
    void setRole1(int x, int y);       // 设置role1位置
    void setRole2(int x, int y);       // 设置role2位置
    void setTime(int v);               // 设置倒计时时间
    void setMapType(int mapType);      // 设置地图类型
    void setMode(int mode);            // 设置单人双人模式
    void setMovable(bool m1, bool m2); // 设置是否可移动
    void readData();                   // 读档
    void readDataError();              // 读档失败处理
    void afterError();                 // 读档失败处理完毕
    void musicPlay();                  // 音乐控制
    // 初始化
    void initial();                  // 初始化
  public slots:
    void endGame();                // 游戏结束

  protected:
    void paintEvent(QPaintEvent *event) override; // 重写paintEvent事件，设置背景

  private:
    Ui::Map *ui; // ui

    QProgressBar *pb; // 进度条（倒计时）

    QLCDNumber role1Score, role2Score;                      // 角色分数显示
    int columnWidth, columnHeight;                          // 边框高度，宽度
    int Mode;                                               // 单人双人模式
    int LinkType;                                           // 连接类型
    int propNum;                                            // 道具总数
    int blockNum;                                           // 地图方块个数
    int Score1, Score2;                                     // 角色分数
    int linkPoint1x, linkPoint1y, linkPoint2x, linkPoint2y; // 连接点坐标
    int propx1, propx2, propy1, propy2;                     // 道具坐标
    int delty, deltx;                                       // 单个方块大小
    int timeRecord;                                         // 计时
    int timeMax;                                            // 总时长
    int state;                          // 是否游戏进行完毕 0表示失败 1表示正在进行 2表示胜利
    int blockE[ydivide][xdivide];       // 图标
    bool isHint;                        // 是否hint
    int hintx1, hintx2, hinty1, hinty2; // 提示方块坐标
    bool movable1, movable2;            // 角色是否可移动
    bool isDisszy1, isDisszy2;          // 角色是否disszy
    bool mouseControl;                  // 是否可鼠标控制
    int mapType;                        // 地图类型
    int xbegin, ybegin;                 // 方块起始坐标
    int bgType;                         // 游戏背景类型
    const int mapTypeNum = 4;           // 地图类型总数

    int locx[xdivide]; // 方格对应位置
    int locy[xdivide];
    int movespped; // 角色移动速度
  public:
    QMediaPlayer *music;   // 音乐

  private:
    QMediaPlaylist *playlist; // 音乐播放列表

    MyPushButton louder;    // 音量加
    MyPushButton lower;   // 音量减
    MyPushButton musicOff;  // 音乐开关
    MyPushButton musicOn;   // 音乐开关

    int volume; // 音量
    bool drawIntro; // 是否画出介绍

    // emoji
    QPixmap *laughter;
    QPixmap *smile;
    QPixmap *winking;
    QPixmap *clown;
    QPixmap *enraged;
    QPixmap *savoring;
    QPixmap *grining;
    QPixmap *heart;
    QPixmap *crying;
    QPixmap *nerd;
    QPixmap *partying;
    QPixmap *pleading;
    QPixmap *disszy;
    QPixmap *reverse;
    QPixmap *timePlus;
    QPixmap *cold;
    QPixmap *hint;
    QPixmap *flash;
    QPixmap *failure;
    QPixmap *winning;
    // role
    QPixmap *detective; // 侦探，人物1
    QPixmap *mage;     // 法师，人物2
    // 游戏介绍
    QPixmap intro; // 介绍
    QPixmap scoreSide; // 分数栏
    QPixmap title; // 标题
    // 角色坐标
    QPoint *role1;  // 人物1
    QPoint *role2;  // 人物2

    void keyPressEvent(QKeyEvent *event) override;
    // 鼠标状态下连接
    void mouseLink(int roleX, int roleY);
    // 鼠标状态下移动
    void mousePressEvent(QMouseEvent *event) override;
    // 鼠标状态下移动
    void keyReleaseEvent(QKeyEvent *event) override;
    // 判断是否能够通过
    bool checkPass(QPoint *role, int dire, int &targetX, int &targetY, int &targetx, int &targety);
    // 判断是否能够连接
    bool islinked(int &targetX, int &targetY, int &targetx, int &targety, bool islink);
    bool islinked0(int &targetX, int &targetY, int &targetx, int &targety, bool islink);
    bool islinked1(int &targetX, int &targetY, int &targetx, int &targety, bool islink);
    bool islinked2(int &targetX, int &targetY, int &targetx, int &targety, bool islink);
    // 检查相同y值时，两x值之间是否是通路
    bool isXconnected(int x1, int x2, int y);
    // 检查相同x值时，两y值之间是否是通路
    bool isYconnected(int y1, int y2, int x);
    // 触发道具分配
    void setProp(int item, int role); // 放置道具 ，item表示道具类型
    // 地图设置
    void setMap(int mapType);
    // 标注Hint方块
    bool hintBlock();
    // 画hint方格的边框
    void drawHint(QPainter &painter);
    // 连接两点（三条直线内）
    void linkPoints(int type, int &targetX, int &targetY, int &targetx, int &targety, int link1x, int link1y,
                    int link2x,
                    int link2y); // type表示拐点数量 0-2
    // 随机生成道具
    void createProp();
    // 触发道具的处理
    void isProp(int &frontX, int &frontY, int role);
    // 失败处理
    void fail();
    // 获胜处理
    void win();
    // 画图标
    void drawEmoji(QPainter &painter);
    // 画连线
    void drawLines(QPainter &painter);
    // 画选中方块
    void drawBlock(QPainter &painter);
    // 存档
    void recordData();
    // 读档道具处理
    void setPropTime();
    // 导入emoji
    void downloadEmoji();
    // 菜单栏等位置设置
    void setMenu();
    // 设置时间等
    void setTime_pb();
    // 返回键等按键设置
    void setBack_Stop();
    // 画边框
    void drawMargin(QPainter &painter);
};
