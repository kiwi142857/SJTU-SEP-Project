
#pragma once

#include <QObject>
#include <QtTest/QtTest>

#include "mymainwindow.h"

class MyMainWindow;

// 测试类，用于测试游戏的各个功能
class LinkTest : public QObject{

    Q_OBJECT
  public:
    explicit LinkTest(QObject *parent = nullptr);

    MyMainWindow *m;
  signals:
  private:
    void clickToMap(int mode); // 按下初始一些按钮
  private slots:
    // startscence检测
    //  检验LoadGame按键
    void testLoadGame();
    // 检验exit按键
    void testExitGame();
    // 检验newGame按键
    void testNewGame();

    // map检测
    // 检测角色移动功能（无障碍，方块区外）
    /*
    void testMovable1();
    void testMovable2();

    // 检测角色移动功能（无障碍，方块区内）
    void testMovable3();
    void testMovable4();

    // 检测角色移动功能（有障碍， 方块区外）
    void testUnmovable1();
    void testUnmovable2();

    // 检测角色移动功能（有障碍，方块区内）
    void testUnmovable3();
    void testUnmovable4();
   */
    // 检测存档功能
    void testRecordData();

    // 检测读档功能
    void testReadData();

    // 检测连线功能
    void testLinkable();
    void testUnlikable();

    // 检测鼠标点击位置功能
    /*
    void testMouseClick();
*/
    // 检测道具 +30s
    void testTimePlus();

    // 检测道具disszy
    /*
    void testDisszy();

    // 检测道具cold
    void testCold();
    */

    // 检测道具hint
    void testHint();

    // 检测道具shuffle
    void testShuffle();

    // 音乐播放（增大，减小音量,暂停等测试）
    void testMusic();

    // 检测胜利处理
    void testWinOrFail();

    // 角色位置设置
    void testSetRole();
};

