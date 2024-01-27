#include "linktest.h"

#include <iostream>

LinkTest::LinkTest(QObject *parent) : QObject(parent)
{
    m = new MyMainWindow;
}

// 初始化按钮
void LinkTest::clickToMap(int mode)
{
    QTest::mouseClick(m, Qt::LeftButton, Qt::NoModifier,
                      QPoint(m->s->btnLoadGame->x() + 5, m->s->btnLoadGame->y() + 5));

    QTest::mouseClick(m->s->btnNewGame, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));

    if (mode == 1) {
        QTest::mouseClick(m->s, Qt::LeftButton, Qt::NoModifier,
                          QPoint(m->s->btnSingleMode->x() + 5, m->s->btnSingleMode->y() + 5));
        QTest::mouseClick(m->s->btnSingleMode, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    } else {
        QTest::mouseClick(m->s, Qt::LeftButton, Qt::NoModifier,
                          QPoint(m->s->btnDoubleMode->x() + 5, m->s->btnDoubleMode->y() + 5));
        QTest::mouseClick(m->s->btnDoubleMode, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    }
    QTest::mouseClick(m->m, Qt::LeftButton, Qt::NoModifier,
                      QPoint(m->s->btnLoadGame->x() + 5, m->s->btnLoadGame->y() + 5));
}

// 检查按下newGame界面显示是否正确
void LinkTest::testNewGame()
{
    QTest::mousePress(m, Qt::LeftButton, Qt::NoModifier, QPoint(m->s->btnNewGame->x() + 5, m->s->btnNewGame->y() + 5));
    QTest::mouseRelease(m, Qt::LeftButton, Qt::NoModifier,
                        QPoint(m->s->btnNewGame->x() + 5, m->s->btnNewGame->y() + 5));
    QVERIFY(m->s->isHidden());
    QVERIFY(m->s->isHidden());
    QVERIFY(!m->m->isHidden());
    QTest::mousePress(m->s, Qt::LeftButton, Qt::NoModifier,
                      QPoint(m->s->btnNewGame->x() + 5, m->s->btnNewGame->y() + 5));
    QTest::mouseRelease(m->s, Qt::LeftButton, Qt::NoModifier,
                        QPoint(m->s->btnNewGame->x() + 5, m->s->btnNewGame->y() + 5));
    QVERIFY(m->s->isHidden());
    QVERIFY(!m->m->isHidden());
}

// 检查load界面显示是否正确
void LinkTest::testLoadGame()
{
    QTest::mousePress(m, Qt::LeftButton, Qt::NoModifier,
                      QPoint(m->s->btnLoadGame->x() + 5, m->s->btnLoadGame->y() + 5));
    QTest::mouseRelease(m, Qt::LeftButton, Qt::NoModifier,
                        QPoint(m->s->btnLoadGame->x() + 5, m->s->btnLoadGame->y() + 5));
    QVERIFY(m->s->isHidden());
    QVERIFY(m->s->isHidden());
    QVERIFY(!m->m->isHidden());
    QTest::mousePress(m->s, Qt::LeftButton, Qt::NoModifier,
                      QPoint(m->s->btnLoadGame->x() + 5, m->s->btnLoadGame->y() + 5));
    QTest::mouseRelease(m->s, Qt::LeftButton, Qt::NoModifier,
                        QPoint(m->s->btnLoadGame->x() + 5, m->s->btnLoadGame->y() + 5));
    QVERIFY(m->s->isHidden());
    QVERIFY(!m->m->isHidden());
}

// 检查exit界面显示是否正确
void LinkTest::testExitGame()
{
    QTest::mousePress(m, Qt::LeftButton, Qt::NoModifier,
                      QPoint(m->s->btnLoadGame->x() + 5, m->s->btnLoadGame->y() + 5));
    QTest::mouseRelease(m, Qt::LeftButton, Qt::NoModifier,
                        QPoint(m->s->btnLoadGame->x() + 5, m->s->btnLoadGame->y() + 5));

    QTest::mousePress(m->s, Qt::LeftButton, Qt::NoModifier, QPoint(m->s->btnExit->x() + 5, m->s->btnExit->y() + 5));
    QTest::mouseRelease(m->s, Qt::LeftButton, Qt::NoModifier, QPoint(m->s->btnExit->x() + 5, m->s->btnExit->y() + 5));
    QVERIFY(!m->isHidden());
}

// 可移动测试1
/*
void LinkTest::testMovable1()
{
    clickToMap(1);

    QTest::keyClick(m->m, Qt::Key_D, Qt::NoModifier);
    QCOMPARE(m->m->role1->rx(), m->m->movespped);
    QTest::keyClick(m->m, Qt::Key_A, Qt::NoModifier);
    QCOMPARE(m->m->role1->rx(), 0);
    QTest::keyClick(m->m, Qt::Key_S, Qt::NoModifier);
    QCOMPARE(m->m->role1->ry(), m->m->movespped);
}

// 可移动测试2
void LinkTest::testMovable2()
{
    clickToMap(2);

    QTest::keyClick(m->m, Qt::Key_Down, Qt::NoModifier);
    QCOMPARE(m->m->role2->ry(), m->m->movespped);
    QTest::keyClick(m->m, Qt::Key_Left, Qt::NoModifier);
    QCOMPARE(m->m->role2->rx(), m->m->width() - m->m->columnWidth * 2 - m->m->deltx * 2 / 3 - m->m->movespped);
    QTest::keyClick(m->m, Qt::Key_Right, Qt::NoModifier);
    QCOMPARE(m->m->role2->rx(), m->m->width() - m->m->columnWidth * 2 - m->m->deltx * 2 / 3);
}

// 可移动测试3
void LinkTest::testMovable3()
{
    clickToMap(1);
    m->m->blockE[0][0] = -1;
    ;
    m->m->setRole1(m->m->locx[0] - m->m->deltx, m->m->locy[0]);
    QTest::keyClick(m->m, Qt::Key_D, Qt::NoModifier);
    QCOMPARE(m->m->role1->rx(), m->m->locx[0] - m->m->deltx + m->m->movespped);
    QTest::keyClick(m->m, Qt::Key_A, Qt::NoModifier);
    QCOMPARE(m->m->role1->rx(), m->m->locx[0] - m->m->deltx);
    QTest::keyClick(m->m, Qt::Key_W, Qt::NoModifier);
    QCOMPARE(m->m->role1->ry(), m->m->locy[0] - m->m->movespped);
}

// 可移动测试4
void LinkTest::testMovable4()
{
    clickToMap(2);

    m->m->blockE[0][ydivide - 1] = -1;
    m->m->setRole2(m->m->locx[ydivide - 1] + m->m->deltx * 2 / 3, m->m->locy[0] - m->m->delty * 2 / 3);
    QTest::keyClick(m->m, Qt::Key_Down, Qt::NoModifier);
    QCOMPARE(m->m->role2->ry(), m->m->locy[0] - m->m->delty * 2 / 3 + m->m->movespped);
    QTest::keyClick(m->m, Qt::Key_Up, Qt::NoModifier);
    QCOMPARE(m->m->role2->ry(), m->m->locy[0] - m->m->delty * 2 / 3);
}

// 不可移动测试3
void LinkTest::testUnmovable3()
{
    clickToMap(1);
    m->m->blockE[0][0] = 1;
    ;
    m->m->setRole1(m->m->locx[0] - m->m->deltx, m->m->locy[0]);
    QTest::keyClick(m->m, Qt::Key_D, Qt::NoModifier);
    QCOMPARE(m->m->role1->rx(), m->m->locx[0] - m->m->deltx);
    QTest::keyClick(m->m, Qt::Key_A, Qt::NoModifier);
    QCOMPARE(m->m->role1->rx(), m->m->locx[0] - m->m->movespped - m->m->deltx);
    QTest::keyClick(m->m, Qt::Key_D, Qt::NoModifier);
    QCOMPARE(m->m->role1->rx(), m->m->locx[0] - m->m->deltx);
    QTest::keyClick(m->m, Qt::Key_W, Qt::NoModifier);
    QCOMPARE(m->m->role1->ry(), m->m->locy[0] - m->m->movespped);
}

// 不可移动测试4
void LinkTest::testUnmovable4()
{
    clickToMap(2);

    m->m->blockE[0][ydivide - 1] = 1;
    m->m->setRole2(m->m->locx[ydivide - 1] + m->m->deltx * 2 / 3, m->m->locy[0] - m->m->delty * 2 / 3);
    QTest::keyClick(m->m, Qt::Key_Down, Qt::NoModifier);
    QCOMPARE(m->m->role2->ry(), m->m->locy[0] - m->m->delty * 2 / 3);
    QTest::keyClick(m->m, Qt::Key_Up, Qt::NoModifier);
    QCOMPARE(m->m->role2->ry(), m->m->locy[0] - m->m->delty * 2 / 3 - m->m->movespped < 0
                                    ? m->m->locy[0] - m->m->delty * 2 / 3
                                    : m->m->locy[0] - m->m->delty * 2 / 3 - m->m->movespped);
}

// 可移动测试1
void LinkTest::testUnmovable1()
{
    clickToMap(1);

    QTest::keyClick(m->m, Qt::Key_A, Qt::NoModifier);
    QCOMPARE(m->m->role1->rx(), 0);
    QTest::keyClick(m->m, Qt::Key_W, Qt::NoModifier);
    QCOMPARE(m->m->role1->ry(), 0);
}

// 可移动测试2
void LinkTest::testUnmovable2()
{
    clickToMap(2);

    QTest::keyClick(m->m, Qt::Key_Up, Qt::NoModifier);
    QCOMPARE(m->m->role2->ry(), 0);
    QTest::keyClick(m->m, Qt::Key_Right, Qt::NoModifier);
    QCOMPARE(m->m->role2->rx(), m->m->width() - m->m->columnWidth * 2 - m->m->deltx * 2 / 3);
}*/

// 读档
void LinkTest::testReadData()
{
    clickToMap(1);

    QTest::mouseClick(&m->m->menu, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QTest::mouseClick(&m->m->downLoad, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QVERIFY(m->m->blockE[5][5] == -1);
    QVERIFY(m->m->blockE[6][6] == -1);
    QVERIFY(m->m->Score1 == 5);
}
// 存档
void LinkTest::testRecordData()
{
    clickToMap(1);
    m->m->blockE[5][5] = m->m->blockE[6][6] = -1;
    m->m->Score1 = 5;
    QTest::mouseClick(&m->m->menu, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QTest::mouseClick(&m->m->upLoad, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));

    QVERIFY(m->m->blockE[5][5] == -1);
    QVERIFY(m->m->blockE[6][6] == -1);
    QVERIFY(m->m->Score1 == 5);
}

// 检测是否能连接
void LinkTest::testLinkable()
{
    clickToMap(1);

    // 相同y值2拐点
    m->m->blockE[0][0] = m->m->blockE[0][xdivide - 1] = 5;
    int temp1 = 0, temp2 = xdivide - 1;
    QVERIFY(m->m->islinked(temp1, temp1, temp2, temp1, temp1));

    // 相同y值无拐点
    for (int i = 1; i < xdivide - 1; ++i)
        m->m->blockE[0][i] = -1;
    QVERIFY(m->m->islinked(temp1, temp1, temp2, temp1, temp1));

    // 相同y值2拐点
    m->m->blockE[0][0] = m->m->blockE[ydivide - 1][0] = 5;
    temp2 = ydivide - 1;
    QVERIFY(m->m->islinked(temp1, temp1, temp1, temp2, temp1));

    // 相同x值无拐点
    for (int i = 1; i < ydivide - 1; ++i)
        m->m->blockE[i][0] = -1;
    QVERIFY(m->m->islinked(temp1, temp1, temp1, temp2, temp1));

    // 单拐点
    // 置满
    for (int i = 0; i < ydivide; ++i) {
        for (int j = 0; j < xdivide; ++j)
            m->m->blockE[i][j] = 1;
    }

    for (int i = 0; i < xdivide - 1; ++i)
        m->m->blockE[1][i] = -1;
    int temp3 = 1;
    temp2 = xdivide - 1;
    QVERIFY(m->m->islinked(temp1, temp1, temp2, temp3, 0));

    // 双拐点
    for (int i = 0; i < ydivide; ++i) {
        for (int j = 0; j < xdivide; ++j)
            m->m->blockE[i][j] = 1;
    }
    for (int i = 1; i < xdivide / 2; ++i)
        m->m->blockE[0][i] = -1;
    for (int i = xdivide / 2 - 1; i < xdivide - 1; ++i)
        m->m->blockE[1][i] = -1;
    QVERIFY(m->m->islinked(temp1, temp1, temp2, temp3, 0));
}

// 检测是否能连接
void LinkTest::testUnlikable()
{
    clickToMap(1);

    // 图标不同
    m->m->blockE[0][0] = 5;
    m->m->blockE[0][xdivide - 1] = 4;
    int temp1 = 0, temp2 = xdivide - 1;
    QVERIFY(!m->m->islinked(temp1, temp1, temp2, temp1, temp1));

    // 三条直线无法连接
    for (int i = 0; i < ydivide; ++i) {
        for (int j = 0; j < xdivide; ++j)
            m->m->blockE[i][j] = 1;
    }
    int temp3 = 1;
    for (int i = 1; i < xdivide / 2; ++i)
        m->m->blockE[0][i] = -1;
    for (int i = xdivide / 2; i < xdivide - 1; ++i)
        m->m->blockE[1][i] = -1;
    QVERIFY(!m->m->islinked(temp1, temp1, temp2, temp3, 0));
    // 三条直线无法连接
    for (int i = 0; i < ydivide; ++i) {
        for (int j = 0; j < xdivide; ++j)
            m->m->blockE[i][j] = 1;
    }
    temp3 = 1;
    for (int i = 1; i < xdivide / 2; ++i)
        m->m->blockE[5][i] = -1;
    for (int i = xdivide / 2; i < xdivide - 1; ++i)
        m->m->blockE[4][i] = -1;
    QVERIFY(!m->m->islinked(temp1, temp1, temp2, temp3, 0));
    // 三条直线无法连接
    for (int i = 0; i < ydivide; ++i) {
        for (int j = 0; j < xdivide; ++j)
            m->m->blockE[i][j] = 1;
    }
    temp3 = 1;
    for (int i = 1; i < xdivide / 2; ++i)
        m->m->blockE[6][i] = -1;
    for (int i = xdivide / 2; i < xdivide - 1; ++i)
        m->m->blockE[7][i] = -1;
    QVERIFY(!m->m->islinked(temp1, temp1, temp2, temp3, 0));
    // 三条直线无法连接
    for (int i = 0; i < ydivide; ++i) {
        for (int j = 0; j < xdivide; ++j)
            m->m->blockE[i][j] = 1;
    }
    temp3 = 1;
    for (int i = 1; i < xdivide / 2; ++i)
        m->m->blockE[7][i] = -1;
    for (int i = xdivide / 2; i < xdivide - 1; ++i)
        m->m->blockE[9][i] = -1;
    QVERIFY(!m->m->islinked(temp1, temp1, temp2, temp3, 0));
}

// 检测鼠标点击功能
/*
void LinkTest::testMouseClick()
{
    clickToMap(1);
    m->m->mouseControl = 1;

    // 空地检测
    QTest::mouseClick(m->m, Qt::LeftButton, Qt::NoModifier, QPoint(10 + m->m->columnWidth, 8 + m->m->height() / 10));
    QCOMPARE(m->m->role1->rx(), 10);
    QCOMPARE(m->m->role1->ry(), 8);

    // 方块区域 左侧
    m->m->blockE[5][4] = -1;
    m->m->blockE[4][5] = m->m->blockE[5][6] = m->m->blockE[5][6] = m->m->blockE[5][5] = 1;
    QTest::mouseClick(m->m, Qt::LeftButton, Qt::NoModifier,
                      QPoint(m->m->locx[5] + m->m->columnWidth + 3, m->m->locy[5] + m->m->height() / 10 + 3));
    QCOMPARE(m->m->role1->rx(), m->m->locx[4]);
    QCOMPARE(m->m->role1->ry(), m->m->locy[5]);

    // 方块区域 右侧
    m->m->blockE[5][6] = -1;
    m->m->blockE[4][5] = m->m->blockE[5][4] = m->m->blockE[6][5] = m->m->blockE[5][5] = 1;
    QTest::mouseClick(m->m, Qt::LeftButton, Qt::NoModifier,
                      QPoint(m->m->locx[5] + m->m->columnWidth + 3, m->m->locy[5] + m->m->height() / 10 + 3));
    QCOMPARE(m->m->role1->ry(), m->m->locy[5]);
    QCOMPARE(m->m->role1->rx(), m->m->locx[6]);

    // 方块区域 上侧
    m->m->blockE[4][5] = -1;
    m->m->blockE[5][4] = m->m->blockE[5][6] = m->m->blockE[6][5] = m->m->blockE[5][5] = 1;
    QTest::mouseClick(m->m, Qt::LeftButton, Qt::NoModifier,
                      QPoint(m->m->locx[5] + m->m->columnWidth + 3, m->m->locy[5] + m->m->height() / 10 + 3));
    QCOMPARE(m->m->role1->ry(), m->m->locy[4]);
    QCOMPARE(m->m->role1->rx(), m->m->locx[5]);

    // 方块区域 下侧
    m->m->blockE[6][5] = -1;
    m->m->blockE[4][5] = m->m->blockE[5][6] = m->m->blockE[5][4] = m->m->blockE[5][5] = 1;
    QTest::mouseClick(m->m, Qt::LeftButton, Qt::NoModifier,
                      QPoint(m->m->locx[5] + m->m->columnWidth + 3, m->m->locy[5] + m->m->height() / 10 + 3));
    QCOMPARE(m->m->role1->ry(), m->m->locy[6]);
    QCOMPARE(m->m->role1->rx(), m->m->locx[5]);

    // 重新定位失败
    clickToMap(1);
    m->m->mouseControl = 1;
    m->m->blockE[6][5] = m->m->blockE[4][5] = m->m->blockE[5][6] = m->m->blockE[5][4] = m->m->blockE[5][5] = 1;
    QTest::mouseClick(m->m, Qt::LeftButton, Qt::NoModifier,
                      QPoint(m->m->locx[5] + m->m->columnWidth + 3, m->m->locy[5] + m->m->height() / 10 + 3));
    QCOMPARE(m->m->role1->ry(), 0);
    QCOMPARE(m->m->role1->rx(), 0);
}*/

// 检测道具 +30s
void LinkTest::testTimePlus()
{
    m->m->timeRecord -= 50;

    int temp = m->m->timeRecord;
    m->m->setProp(0, 1);
    QVERIFY(m->m->timeRecord - temp == 30);
}




// 检测道具Hint
void LinkTest::testHint()
{
    clickToMap(1);

    m->m->setProp(2, 1);
    QVERIFY(m->m->islinked(m->m->hintx1, m->m->hinty1, m->m->hintx2, m->m->hinty2, 0));
}

// 检测道具shuffle
void LinkTest::testShuffle()
{
    clickToMap(1);

    int tempBlock[ydivide][xdivide];
    for (int i = 0; i < ydivide; ++i)
        for (int j = 0; j < xdivide; ++j)
            tempBlock[i][j] = m->m->blockE[i][j];

    m->m->setProp(1, 1);
    bool testResult = 1;
    for (int i = 0; i < ydivide; ++i)
        for (int j = 0; j < xdivide; ++j) {
            testResult = tempBlock[i][j] == m->m->blockE[i][j];
            if (!testResult)
                break;
        }

    QVariant(!testResult);
}

// 音乐播放（增大，减小音量等测试）
void LinkTest::testMusic()
{
    clickToMap(1);

    int tempVolume = m->m->volume;
    // 增大音量
    QTest::mouseClick(&m->m->louder, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QVERIFY(m->m->volume - tempVolume == 10);

    tempVolume = m->m->volume;
    // 减小音量
    QTest::mouseClick(&m->m->lower, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QVERIFY(m->m->volume - tempVolume == -10);

    // 音乐状态
    QCOMPARE(m->m->music->state(), 1);

    // 暂停音乐
    QTest::mouseClick(&m->m->musicOff, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QCOMPARE(m->m->music->state(), 2);

    // 重新音乐
    QTest::mouseClick(&m->m->musicOn, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QCOMPARE(m->m->music->state(), 1);
}

// 检测胜利失败处理
void LinkTest::testWinOrFail()
{
    clickToMap(1);

    m->m->win();
    QTest::mouseClick(&m->m->clickToContinue, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QVERIFY(m->m->isHidden());

    clickToMap(1);
    m->m->fail();
    QTest::mouseClick(&m->m->clickToContinue, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QVERIFY(m->m->isHidden());
}

// 角色位置设置
void LinkTest::testSetRole()
{
    // 单人模式下测试
    clickToMap(1);

    // 改变role1位置
    m->m->setRole1(100, 100);
    QCOMPARE(m->m->role1->rx(), 100);
    QCOMPARE(m->m->role1->ry(), 100);

    // 双人模式下测试
    clickToMap(2);

    m->m->setRole2(100, 100);
    QCOMPARE(m->m->role2->rx(), 100);
    QCOMPARE(m->m->role2->ry(), 100);
}
