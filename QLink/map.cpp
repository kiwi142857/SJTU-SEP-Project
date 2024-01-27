#include "map.h"

#include <mypushbutton.h>
#include <qcolor.h>
#include <windows.h>

#include <QColor>
#include <QDebug>
#include <QImage>
#include <QLCDNumber>
#include <QLabel>
#include <QMetaObject>
#include <QMovie>
#include <QPainter>
#include <QPixmap>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QTimer>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "ui_map.h"

int target1X = -1, target2X = -1, target1Y = -1,
    target2Y = -1; // 选中第一个方块置零
int target1x = -1, target2x = -1, target1y = -1,
    target2y = -1; // 选中第二个方块置零
int Role = 0;      // 选中角色

const int pixNum = 12; // 图案类型数量

// Map类构造函数，用于初始化游戏界面
Map::Map(QWidget *parent) : QWidget(parent), ui(new Ui::Map)
{
    ui->setupUi(this);    // ui画面设置
    srand(time(nullptr)); // 随机数初始化

    // 初始化
    installEventFilter(this);               // 事件过滤器设置
    setFocusPolicy(Qt::StrongFocus);        // foucusPolicy设置为强
    hintx1 = hintx2 = hinty1 = hinty2 = -1; // 提示方格置零
    isHint = 0;                             // 是否hint状态
    Score1 = Score2 = 0;                    // 积分初始置零
    movable1 = 1;                           // role1角色可移动
    if (Mode == 1)
        movable2 = 0;
    else
        movable2 = 1;          // 若单人模式，则role2设置无法移动
    isDisszy1 = isDisszy2 = 0; // 是否disszy状态
    mouseControl = 0;          // 是否鼠标控制
    Role = 0;                  // 选中人物初始化
    movespped = 15;            // 角色移动速度
    bgType = rand() % 5;       // 游戏背景图

    // 角色初始位置设置
    role1 = new QPoint(0, 0);
    role2 = new QPoint(width() - 10, 0);
    // 地图设置 mapType=0 1 2
    // 3分别为全满地图，SJTU地图，随机地图（80格），随机地图（100格）
    mapType = rand() % mapTypeNum;
    setMap(mapType);

    // 返回键暂停键设置
    setBack_Stop();

    // 菜单栏等设置
    setMenu();

    // 时间&&进度条
    setTime_pb();

    // 导入emoji
    downloadEmoji();

    // 道具随机掉落设置 6000ms掉落一次
    timer2 = new QTimer(this);
    timer2->start(1000);
    propNum = 0;
    connect(timer2, &QTimer::timeout, [=]() { createProp(); });
}

// 读档失败处理
void Map::readDataError()
{
    m1 = movable1; // 保存movable1,movable2值
    m2 = movable2; // 保存movable1,movable2值
    downloadError.show(); // 读档失败图像显示
    movable1 = movable2 = 0;    // 读档失败，角色不可移动
    timer->stop();  // 时钟停止
    timer2->stop(); // 时钟停止
}

// 读档失败处理完毕
void Map::afterError()
{
    movable1 = m1;
    movable2 = m2;
    downLoad.hide(); // 隐藏读档按钮
    timer->start(1000);  // 时钟重置
    timer2->start(6000); // 时钟重启
}

// 存档
void Map::recordData()
{
    QFile file("./gameData.txt"); // 打开文件

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) // 如果无管理员权限创建文件则在D盘创建
        file.setFileName("D:\\gameData.txt");
    else
        file.close();

    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream stream(&file);

        // 写入角色位置
        stream << role1->rx() << "  " << role1->ry() << '\n' << role2->rx() << "  " << role2->ry() << '\n';

        // 写入游戏人数,连接类型，连接类型，地图方块个数，角色分数，连接点坐标，道具坐标，hint坐标
        stream << mapType << "  " << Mode << "  " << LinkType << "  " << propNum << "  " << blockNum << "  " << Score1
               << "  " << Score2 << '\n';
        stream << linkPoint1x << "  " << linkPoint1y << "  " << linkPoint2x << "  " << linkPoint2y << '\n';
        stream << propx1 << "  " << propx2 << "  " << propy1 << "  " << propy2 << '\n';
        stream << hintx1 << "  " << hintx2 << "  " << hinty1 << "  " << hinty2 << '\n';

        // 写入isHint，movable1,movable2,isDisszy1,isDisszy2,mousecontrol;
        stream << isHint << "  " << movable1 << "  " << movable2 << "  " << isDisszy1 << "  " << isDisszy2 << "  "
               << mouseControl << '\n';

        // 写入blockE
        for (int i = 0; i < ydivide; ++i) {
            for (int j = 0; j < xdivide; ++j)
                stream << blockE[i][j] << " ";
        }
        stream << '\n';

        // 写入时间记录，时间总值，状态
        stream << timeRecord << " " << timeMax << " " << state << '\n';

        // 写入target1x等
        stream << target1X << "  " << target2X << "  " << target1Y << "  " << target2Y << "  " << target1x << "  "
               << target2x << "  " << target1y << "  " << target2y << "  " << Role << '\n';

        file.close(); // 关闭文件
    }

    else
        throw QString("open error"); // 如果打开失败，则抛出异常，调用recordError
}

// 读档
void Map::readData()
{
    QFile file("./gameData.txt"); // 打开文件
    if (!file.open(QFile::ReadOnly)) // 如果无管理员权限打开文件则在D盘打开
        file.setFileName("D:\\gameData.txt"); // 若文件打不开则打开D盘处文件
    else
        file.close();

    if (file.open(QFile::ReadOnly)) { // 如果打开成功
        initial(); // 初始化
        QTextStream stream(&file); // 读取文件
        QString str;           // 读取字符串

        // 读入角色位置
        stream >> role1->rx() >> role1->ry() >> role2->rx() >> role2->ry();

        // 读入游戏人数,连接类型，连接类型，地图方块个数，角色分数，连接点坐标，道具坐标，hint坐标
        stream >> mapType >> Mode >> LinkType >> propNum >> blockNum >> Score1 >> Score2;
        stream >> linkPoint1x >> linkPoint1y >> linkPoint2x >> linkPoint2y;
        stream >> propx1 >> propx2 >> propy1 >> propy2;
        stream >> hintx1 >> hintx2 >> hinty1 >> hinty2;
        role1Score.display(Score1);
        role2Score.display(Score2);

        // 写入isHint，movable1,movable2,isDisszy1,isDisszy2,mousecontrol;
        int tempIsHint, tempMovable1, tempMovable2, tempIsDisszy1, tempIsDisszy2, tempMouseControl;
        stream >> tempIsHint >> tempMovable1 >> tempMovable2 >> tempIsDisszy1 >> tempIsDisszy2 >> tempMouseControl;
        isHint = tempIsHint;
        movable1 = tempMovable1;
        movable2 = tempMovable2;
        isDisszy1 = tempIsDisszy1;
        isDisszy2 = tempIsDisszy2;
        mouseControl = tempMouseControl;

        // 读入blockE
        for (int i = 0; i < ydivide; ++i) {
            for (int j = 0; j < xdivide; ++j)
                stream >> blockE[i][j];
        }

        // 读入时间记录，时间总值，状态
        stream >> timeRecord >> timeMax >> state;
        pb->setRange(0, timeMax);

        // 读入target1x等
        stream >> target1X >> target2X >> target1Y >> target2Y >> target1x >> target2x >> target1y >> target2y >> Role;

        setPropTime();  // 设置道具时间
        timer->stop();  // 时钟停止
        timer2->stop(); // 时钟停止
        timer->start(1000); // 时钟重置
        timer2->start(6000);    // 时钟重启
        continueGame.hide();    // 隐藏继续游戏按钮
        stopGame.show();    // 显示暂停游戏按钮

        update();   // 更新界面
        file.close();   // 关闭文件
    } else {
        throw QString("file open error"); // 打开失败则抛出异常
    }
}

// 检查相同y值时，两x值之间是否是通路
bool Map::isXconnected(int x1, int x2, int y)
{
    int min = x1 > x2 ? x2 : x1;
    int max = x1 > x2 ? x1 : x2; // 取大值，小值
    min++;
    if (min < 0 || max >= xdivide || y < 0 || y >= ydivide)
        return 0; // 不可连接
    for (; min < max; ++min) {
        if (blockE[y][min] != -1)
            return 0; // 不可连接
    }
    return 1; // 可连接
}

// 检查相同x值时，两y值之间是否是通路
bool Map::isYconnected(int y1, int y2, int x)
{
    int min = y1 > y2 ? y2 : y1;
    int max = y1 > y2 ? y1 : y2; // 取大值/小值
    min++;
    if (min < 0 || max >= ydivide || x < 0 || x >= xdivide)
        return 0; // 不可连接
    for (; min < max; ++min) {
        if (blockE[min][x] != -1)
            return 0; // 不可连接
    }
    return 1; // 可连接
}

// 连接两点（三条直线内）
void Map::linkPoints(int type, int &targetX, int &targetY, int &targetx, int &targety, int link1x = 0, int link1y = 0,
                     int link2x = 0,
                     int link2y = 0) // type表示拐点数量 0-2
{
    if (targetX == -1 || targetY == -1 || targetx == -1 || targety == -1) // 若访问地址越界，则直接返回
        return;

    // 根据不同连接类型设置连接点
    if (type == 0) // 0个拐点
        LinkType = 0;
    else if (type == 1) {   // 1个拐点
        LinkType = 1;
        linkPoint1x = link1x;   // 连接点1
        linkPoint1y = link1y;   // 连接点1  
    } else if (type == 2) {  // 2个拐点
        LinkType = 2;
        if (link1x == targetX || link1y == targetY) {    // 连接点1
            linkPoint1x = link1x;
            linkPoint1y = link1y;
            linkPoint2x = link2x;
            linkPoint2y = link2y;
        } else {   // 连接点2
            linkPoint1x = link2x;
            linkPoint1y = link2y;
            linkPoint2x = link1x;
            linkPoint2y = link1y;
        }
    }
    // 连接后50ms消除连接线和方格
    QTimer::singleShot(50, [&]() {
        LinkType = -1;                      // 连接类型重新置零
        if (targetX != -1 && targetx != -1) // 方块置零操作
        {
            blockE[targetY][targetX] = blockE[targety][targetx] = -1;
            // 如果触发到对方选择的方块，则把对方选择的值置零
            if (Mode == 2 && Role == 1)
                if ((targetX == target2X && targetY == target2Y) || (targetx == target2X && targety == target2Y)) {
                    target2X = target2Y = -1;
                }
            if (Mode == 2 && Role == 2)
                if ((targetX == target1X && targetY == target1Y) || (targetx == target1X && targety == target1Y)) {
                    target1X = target1Y = -1;
                }
            blockNum -= 2; // 现存方块数-2
            targetX = targetY = targetx = targety = -1;
            // 加分处理
            if (Role == 1)
                Score1 += 5;
            else
                Score2 += 5;
            role1Score.display(Score1); // 分数显示
            role2Score.display(Score2);
            Role = 0;
            if (blockNum == 0 || blockNum == 1) // 如果方块全部清除或剩一（按照文档要求）
                win();                          // 触发胜利操作
        }
    });
}

// 判断是否能够连接
bool Map::islinked(int &targetX, int &targetY, int &targetx, int &targety, bool islink)
{
    if (targetX == -1 || targetY == -1 || targetx == -1 || targety == -1)
        return 0;

    // 是否相同图案
    if (blockE[targetY][targetX] != blockE[targety][targetx]) { // 不同图案处理
        if (islink) {
            targetX = targetx;
            targetY = targety;
            targety = targetx = -1;
        }
        return 0;
    }
    // 当连接点分别为0,1,2时
    if (islinked0(targetX, targetY, targetx, targety, islink))
        return 1;

    if (islinked1(targetX, targetY, targetx, targety, islink))
        return 1;

    if (islinked2(targetX, targetY, targetx, targety, islink))
        return 1;

    // 若连接失败则返回0
    return 0;
}

// 同一条直线
bool Map::islinked0(int &targetX, int &targetY, int &targetx, int &targety, bool islink)
{
    if (targetX == targetx) {   // 同一条直线
        // 零个拐点
        if (isYconnected(targetY, targety, targetX)) {
            if (islink)
                linkPoints(0, targetX, targetY, targetx, targety, targetX, targetY, targetx, targety);
            return 1;
        }
        // 两个拐点
        for (int i = targetX - 1; i >= 0; --i) {
            if ((blockE[targety][i] == -1 && blockE[targetY][i] == -1))
                if ((isYconnected(targetY, targety, i)) && isXconnected(i, targetX, targetY) &&
                    isXconnected(i, targetX, targety)) {
                    if (islink)
                        linkPoints(2, targetX, targetY, targetx, targety, i, targetY, i, targety);
                    return 1;
                }
        }

        for (int i = targetX + 1; i < xdivide; ++i) {   // 两个拐点
            if ((blockE[targety][i] == -1 && blockE[targetY][i] == -1))
                if ((isYconnected(targetY, targety, i)) && isXconnected(i, targetX, targetY) &&
                    isXconnected(i, targetX, targety)) {
                    if (islink)
                        linkPoints(2, targetX, targetY, targetx, targety, i, targetY, i, targety);
                    return 1;
                }
        }
        if ((blockE[targetY][0] == -1 && blockE[targety][0] == -1) ||
            (blockE[targetY][xdivide - 1] == -1 && blockE[targety][xdivide - 1] == -1) || targetX == 0 ||
            targetx == xdivide - 1) {
            if (targetX == 0) {
                int m = -1;
                if (islink)
                    linkPoints(2, targetX, targetY, targetx, targety, m, targetY, m, targety);
                return 1;
            }
            if (targetX == xdivide - 1) {   // 两个拐点
                int m = xdivide;
                if (islink)
                    linkPoints(2, targetX, targetY, targetx, targety, m, targetY, m, targety);
                return 1;
            }
            if (isXconnected(0, targetx, targety) && isXconnected(0, targetX, targetY) &&
                (blockE[targetY][0] == -1 && blockE[targety][0] == -1)) {
                int m = -1;
                if (islink)
                    linkPoints(2, targetX, targetY, targetx, targety, m, targetY, m, targety);
                return 1;
            }
            if (isXconnected(xdivide - 1, targetx, targety) && isXconnected(xdivide - 1, targetX, targetY)) {
                int m = xdivide;
                if (islink)
                    linkPoints(2, targetX, targetY, targetx, targety, m, targetY, m, targety);
                return 1;
            }
        }
        if (islink) {   // 若连接成功则将targetX，targetY，targetx，targety置零
            targetX = targetx;
            targetY = targety;
            targetx = targety = -1;
        }
        return 0;
    }

    if (targetY == targety) {
        // 零个拐点
        if (isXconnected(targetx, targetX, targety)) {
            if (islink)
                linkPoints(0, targetX, targetY, targetx, targety, targetX, targetY, targetx, targety);
            return 1;
        }
        // 两个拐点
        for (int i = targety - 1; i >= 0; --i) {
            if ((blockE[i][targetX] == -1 && blockE[i][targetx] == -1))
                if ((isXconnected(targetX, targetx, i)) && isYconnected(i, targetY, targetx) &&
                    isYconnected(i, targetY, targetX)) {
                    if (islink)
                        linkPoints(2, targetX, targetY, targetx, targety, targetX, i, targetx, i);
                    return 1;
                }
        }
        for (int i = targety + 1; i < ydivide; ++i) {
            if ((blockE[i][targetX] == -1 && blockE[i][targetx] == -1))
                if ((isXconnected(targetX, targetx, i)) && isYconnected(i, targetY, targetx) &&
                    isYconnected(i, targetY, targetX)) {
                    if (islink)
                        linkPoints(2, targetX, targetY, targetx, targety, targetX, i, targetx, i);
                    return 1;
                }
        }
        if ((blockE[0][targetX] == -1 && blockE[0][targetx] == -1) ||
            (blockE[ydivide - 1][targetX] == -1 && blockE[ydivide - 1][targetx] == -1) || targetY == 0 ||
            targetY == ydivide - 1) {
            if (targetY == 0) {
                int m = -1;
                if (islink)
                    linkPoints(2, targetX, targetY, targetx, targety, targetX, m, targetx, m);
                return 1;
            }
            if (targetY == ydivide - 1) {
                int m = ydivide;
                if (islink)
                    linkPoints(2, targetX, targetY, targetx, targety, targetX, m, targetx, m);
                return 1;
            }
            if (isYconnected(0, targetY, targetx) && isYconnected(0, targetY, targetX) &&
                (blockE[0][targetX] == -1 && blockE[0][targetx] == -1)) {
                int m = -1;
                if (islink)
                    linkPoints(2, targetX, targetY, targetx, targety, targetX, m, targetx, m);
                return 1;
            }
            if (isYconnected(ydivide - 1, targetY, targetx) && isYconnected(ydivide - 1, targetY, targetX) &&
                (blockE[ydivide - 1][targetX] == -1 && blockE[ydivide - 1][targetx] == -1)) {
                int m = ydivide;
                if (islink)
                    linkPoints(2, targetX, targetY, targetx, targety, targetX, m, targetx, m);
                return 1;
            }
        }
        if (islink) {   // 若连接成功则将targetX，targetY，targetx，targety置零
            targetX = targetx;
            targetY = targety;
            targetx = targety = -1;
        }
        return 0;
    }
    return 0;
}

// 一个拐点
bool Map::islinked1(int &targetX, int &targetY, int &targetx, int &targety, bool islink)
{
    // 一个拐点
    if (blockE[targety][targetX] == -1) {
        if (isXconnected(targetX, targetx, targety) && isYconnected(targetY, targety, targetX)) {
            if (islink)
                linkPoints(1, targetX, targetY, targetx, targety, targetX, targety, targetX, targety);
            return 1;
        }
    }
    if (blockE[targetY][targetx] == -1) {
        if (isXconnected(targetX, targetx, targetY) && isYconnected(targetY, targety, targetx)) {
            if (islink)
                linkPoints(1, targetX, targetY, targetx, targety, targetx, targetY, targetX, targety);
            return 1;
        }
    }
    return 0;
}

// 两个拐点
bool Map::islinked2(int &targetX, int &targetY, int &targetx, int &targety, bool islink)
{
    // 两个拐点
    // 第一条  水平方向  targetY
    for (int i = 0; i < xdivide; ++i) {
        if (blockE[targetY][i] != -1 || blockE[targety][i] != -1)
            continue;
        if (isYconnected(targetY, targety, i) && isXconnected(i, targetX, targetY) &&
            isXconnected(i, targetx, targety)) {
            if (islink)
                linkPoints(2, targetX, targetY, targetx, targety, i, targetY, i, targety);
            return 1;
        }
    }

    // 第二条  竖直方向 targetX
    for (int i = 0; i < ydivide; ++i) {
        if (blockE[i][targetX] != -1 || blockE[i][targetx] != -1)
            continue;
        if (isXconnected(targetX, targetx, i) && isYconnected(i, targety, targetx) &&
            isYconnected(i, targetY, targetX)) {
            if (islink)
                linkPoints(2, targetX, targetY, targetx, targety, targetX, i, targetx, i);
            return 1;
        }
    }
    // 拐点在方格区外
    if (((blockE[targetY][0] == -1 && isXconnected(0, targetX, targetY)) || targetX == 0) &&
        ((blockE[targety][0] == -1 && isXconnected(0, targetx, targety)) || targetx == 0)) {
        int m = -1;
        if (islink)
            linkPoints(2, targetX, targetY, targetx, targety, m, targetY, m, targety);
        return 1;
    }
    if (((blockE[targetY][xdivide - 1] == -1 && isXconnected(xdivide - 1, targetX, targetY)) ||
         targetX == xdivide - 1) &&
        ((blockE[targety][xdivide - 1] == -1 && isXconnected(xdivide - 1, targetx, targety)) ||
         targetx == xdivide - 1)) {
        int m = xdivide;
        if (islink)
            linkPoints(2, targetX, targetY, targetx, targety, m, targetY, m, targety);
        return 1;
    }
    if (((blockE[0][targetX] == -1 && isYconnected(0, targetY, targetX)) || targetY == 0) &&
        ((blockE[0][targetx] == -1 && isYconnected(0, targety, targetx)) || targety == 0)) {
        int m = -1;
        if (islink)
            linkPoints(2, targetX, targetY, targetx, targety, targetX, m, targetx, m);
        return 1;
    }
    if (((blockE[ydivide - 1][targetX] == -1 && isYconnected(ydivide - 1, targetY, targetX)) ||
         targetY == ydivide - 1) &&
        ((blockE[ydivide - 1][targetx] == -1 && isYconnected(ydivide - 1, targety, targetx)) ||
         targety == ydivide - 1)) {
        int m = ydivide;
        if (islink)
            linkPoints(2, targetX, targetY, targetx, targety, targetX, m, targetx, m);
        return 1;
    }
    if (islink) {   // 若连接成功则将targetX，targetY，targetx，targety置零
        targetX = targetx;
        targetY = targety;
        targetx = targety = -1;
    }
    return 0;
}

// 判断是否能够通过
bool Map::checkPass(QPoint *role, int dire, int &targetX, int &targetY, int &targetx, int &targety)
{
    if (role == role1)  // 判断角色
        Role = 1;    // 1表示角色1
    else
        Role = 2;   // 2表示角色2

    // dire表示方向，1、2、3、4分别表示上下左右
    if (dire == 1) {
        // 向上无物体
        if (role->ry() <= 5)
            return 0;
        // 非图案区
        if ((role->rx() + 0.5 * deltx < xbegin || role->rx() > locx[xdivide - 1] + deltx) &&
            role->y() < columnHeight * 0.84)
            return 1;

        if (role->ry() <= locy[0] || role->ry() - 5 > locy[ydivide - 1] + delty) {
            return 1;
        }

        // 计算role之上的方格  locx[j]=columnWidth/2+j*deltx;
        int frontY = (role->ry() - ybegin - 5) / delty;
        int frontX = (role->rx() - xbegin + deltx / 3) / deltx;
        if (frontX < 0 || frontX >= xdivide || frontY < 0 || frontY >= ydivide)     // 越界
            return 1;
        if (blockE[frontY][frontX] == -1)
            return 1;
        if (targetX == -1) {    // 若targetX为-1，则表示第一次访问
            targetX = frontX;
            targetY = frontY;
            if (blockE[frontY][frontX] >= pixNum) {
                isProp(targetX, targetY, Role);
                return 0;
            }
        } else {    // 若targetX不为-1，则表示第二次访问
            targetx = frontX;
            targety = frontY;
            if (targetx == targetX && targety == targetY) {  // 若两次访问的方格相同，则将targetx，targety置零
                targetx = targety = -1;
            } else if (blockE[frontY][frontX] >= pixNum) {  // 若两次访问的方格不同，且为道具，则触发道具
                isProp(targetx, targety, Role);
                return 0;
            }
            islinked(targetX, targetY, targetx, targety, 1);    // 连接两次访问的方格
        }
        return 0;
    }

    else if (dire == 2) {
        // 向下无物体
        if (role->y() >= columnHeight * 0.84)
            return 0;

        if ((role->rx() + deltx * 2 / 3 < xbegin || role->rx() > locx[xdivide - 1] + deltx) && role->y() >= 0)
            return 1;

        if (role->ry() + delty < locy[0] || role->ry() >= locy[ydivide - 1] + delty) {
            return 1;
        }

        // 计算role之下的方格  locx[j]=columnWidth/2+j*deltx;

        int frontY = (role->ry() - ybegin + delty) / delty; // 计算role之下的方格

        int frontX = (role->rx() - xbegin + deltx / 3) / deltx; // 计算role之下的方格
        if (frontX < 0 || frontX >= xdivide || frontY < 0 || frontY >= ydivide)
            return 1;
        if (blockE[frontY][frontX] == -1)   // 若role之下的方格为-1，则表示不可通过
            return 1;
        if (targetX == -1) {    // 若targetX为-1，则表示第一次访问
            targetX = frontX;
            targetY = frontY;
            if (blockE[frontY][frontX] >= pixNum) {
                isProp(targetX, targetY, Role); // 若role之下的方格为道具，则触发道具
                return 0;
            }
        } else {
            targetx = frontX;   // 若targetX不为-1，则表示第二次访问
            targety = frontY;   // 若targetX不为-1，则表示第二次访问
            if (targetx == targetX && targety == targetY) {
                targetx = targety = -1;
            } else if (blockE[frontY][frontX] >= pixNum) {
                isProp(targetx, targety, Role); // 若role之下的方格为道具，则触发道具   
                return 0;
            }
            islinked(targetX, targetY, targetx, targety, 1);    // 连接两次访问的方格
        }
        return 0;
    }

    else if (dire == 3) {
        // 向左无物体
        if (role->rx() <= 2)
            return 0;
        if ((role->ry() + delty * 3 / 5 < ybegin || role->ry() > locy[ydivide - 1] + delty - 5)) {
            return 1;
        }
        if ((role->rx() + delty < xbegin || role->rx() > locx[xdivide - 1] + delty)) {
            return 1;
        }
        // 计算role之下的方格  locx[j]=columnWidth/2+j*deltx;
        int frontY = (role->ry() - ybegin + delty / 3) / delty;
        int frontX = (role->rx() - xbegin) / deltx;
        if (frontX < 0 || frontX >= xdivide || frontY < 0 || frontY >= ydivide)
            return 1;
        if (blockE[frontY][frontX] == -1)
            return 1;
        if (targetX == -1) {
            targetX = frontX;
            targetY = frontY;
            if (blockE[frontY][frontX] >= pixNum) {
                isProp(targetX, targetY, Role);
                return 0;
            }
        } else {
            targetx = frontX;
            targety = frontY;   // 若targetX不为-1，则表示第二次访问
            if (targetx == targetX && targety == targetY) {
                targetx = targety = -1;
            } else if (blockE[frontY][frontX] >= pixNum) {
                isProp(targetx, targety, Role);
                return 0;
            }
            islinked(targetX, targetY, targetx, targety, 1);    // 连接两次访问的方格
        }
        return 0;
    }

    else if (dire == 4) {
        // 向右无物体
        int rangeX = width() - 2 * columnWidth - (width() - columnWidth * 3) / xdivide * 2 / 3 + delty * 2 / 3;

        if (role->rx() + delty * 2 / 3 >= rangeX)
            return 0;

        if ((role->ry() + delty * 3 / 5 < ybegin || role->ry() > locy[ydivide - 1] + delty - 5)) {
            return 1;
        }

        if ((role->rx() + delty < xbegin || role->rx() > locx[xdivide - 1] + delty)) {
            return 1;
        }
        // 计算role之下的方格  locx[j]=columnWidth/2+j*deltx;
        int frontY = (role->ry() - ybegin + delty / 3) / delty;
        int frontX = (role->rx() - xbegin + deltx * 2 / 3) / deltx;
        if (frontX < 0 || frontX >= xdivide || frontY < 0 || frontY >= ydivide)
            return 1;
        if (blockE[frontY][frontX] == -1)
            return 1;
        if (targetX == -1) {
            targetX = frontX;
            targetY = frontY;
            if (blockE[frontY][frontX] >= pixNum) {
                isProp(targetX, targetY, Role);
                return 0;
            }
        } else {
            targetx = frontX;
            targety = frontY;
            if (targetx == targetX && targety == targetY) {
                targetx = targety = -1;
            } else if (blockE[frontY][frontX] >= pixNum) {
                isProp(targetx, targety, Role);
                return 0;
            }
            islinked(targetX, targetY, targetx, targety, 1);
        }
        return 0;
    }
    // 若无障碍可通行
    return 1;
}

QSet<int> pressedKeys; // 单键
void Map::keyPressEvent(QKeyEvent *event)
{
    // 将按下的按键添加到集合中
    pressedKeys.insert(event->key());

    if (pressedKeys.contains(Qt::Key_Escape)) { // esc退出
        this->close();
    }
    if (movable1) {
        // 根据集合中的元素执行操作
        if (pressedKeys.contains(Qt::Key_W)) {
            // 向上移动角色
            if (!isDisszy1) {   // 若角色不处于眩晕状态
                if (checkPass(role1, 1, target1X, target1Y, target1x, target1y))
                    role1->ry() -= movespped;
                update();
            } else {    // 若角色处于眩晕状态
                if (checkPass(role1, 2, target1X, target1Y, target1x, target1y))
                    role1->ry() += movespped;
                update();
            }
        }
        if (pressedKeys.contains(Qt::Key_A)) {
            // 向左移动角色
            if (!isDisszy1) {   // 若角色不处于眩晕状态
                if (checkPass(role1, 3, target1X, target1Y, target1x, target1y))
                    role1->rx() -= movespped;
                update();
            } else {    // 若角色处于眩晕状态
                if (checkPass(role1, 4, target1X, target1Y, target1x, target1y))
                    role1->rx() += movespped;
                update();
            }
        }
        if (pressedKeys.contains(Qt::Key_S)) {
            // 向下移动角色
            if (!isDisszy1) {   // 若角色不处于眩晕状态
                if (checkPass(role1, 2, target1X, target1Y, target1x, target1y))
                    role1->ry() += movespped;
                update();
            } else {    // 若角色处于眩晕状态
                if (checkPass(role1, 1, target1X, target1Y, target1x, target1y))
                    role1->ry() -= movespped;
                update();
            }
        }
        if (pressedKeys.contains(Qt::Key_D)) {
            // 向右移动角色
            if (!isDisszy1) {   // 若角色不处于眩晕状态
                if (checkPass(role1, 4, target1X, target1Y, target1x, target1y))
                    role1->rx() += movespped;
                update();
            } else {    // 若角色处于眩晕状态
                if (checkPass(role1, 3, target1X, target1Y, target1x, target1y))
                    role1->rx() -= movespped;
                update();
            }
        }
    }
    if (movable2) { // 若角色2可移动
        if (pressedKeys.contains(Qt::Key_Up)) {
            // 向上移动角色
            if (!isDisszy2) {   // 若角色不处于眩晕状态
                if (checkPass(role2, 1, target2X, target2Y, target2x, target2y))
                    role2->ry() -= movespped;
                update();
            } else {
                if (checkPass(role2, 2, target2X, target2Y, target2x, target2y))
                    role2->ry() += movespped;
                update();
            }
        }
        if (pressedKeys.contains(Qt::Key_Left)) {
            // 向左移动角色
            if (!isDisszy2) {
                if (checkPass(role2, 3, target2X, target2Y, target2x, target2y))
                    role2->rx() -= movespped;
                update();
            } else {
                if (checkPass(role2, 4, target2X, target2Y, target2x, target2y))
                    role2->rx() += movespped;
                update();
            }
        }
        if (pressedKeys.contains(Qt::Key_Down)) {
            // 向下移动角色
            if (!isDisszy2) {
                if (checkPass(role2, 2, target2X, target2Y, target2x, target2y))
                    role2->ry() += movespped;
                update();
            } else {
                if (checkPass(role2, 1, target2X, target2Y, target2x, target2y))
                    role2->ry() -= movespped;
                update();
            }
        }
        if (pressedKeys.contains(Qt::Key_Right)) {
            // 向右移动角色
            if (!isDisszy2) {
                if (checkPass(role2, 4, target2X, target2Y, target2x, target2y))
                    role2->rx() += movespped;
                update();
            } else {
                if (checkPass(role2, 3, target2X, target2Y, target2x, target2y))
                    role2->rx() -= movespped;
                update();
            }
        }
    }
}

void Map::keyReleaseEvent(QKeyEvent *event)
{
    // 将释放的按键从集合中移除
    pressedKeys.remove(event->key());
}

void Map::paintEvent(QPaintEvent *event)
{
    // 初始化内容
    if (!setInit)
        initial();
    // 绘图初始化
    QPainter painter(this); // 创建QPainter一个对象
    drawMargin(painter);    // 画边框
    painter.setPen(QColor(255, 0, 0));  // 设置画笔颜色

    painter.setPen(QColor(0, 146, 255));    // 设置画笔颜色
    QFont font("Candara Light", 50, QFont::Bold, 0); // 字体，大小，粗体，斜体
    // font.setLetterSpacing(QFont::AbsoluteSpacing, 10); //设置字符间的间距
    painter.setFont(font);
    painter.drawText(QRectF(locx[8] + xbegin, 5, deltx * (xdivide - 14), delty * 1.5), Qt::AlignHCenter,
                     "Emoji Link"); // 字体水平居中

    // 坐标轴平移
    painter.translate(columnWidth, height() / 10);
    // 画hint的图标方格
    if (isHint) {
        if (hintx1 == -1 || blockE[hinty1][hintx1] == -1 || blockE[hinty2][hintx2] == -1)
            if (!hintBlock()) {
                std::cout << "无解";
            }
        if (hintx1 != -1 && blockE[hinty1][hintx1] >= 0 && blockE[hinty1][hintx1] < pixNum)
            drawHint(painter);
    }
    // 画选中方块
    drawBlock(painter);
    // 画emoji图标
    drawEmoji(painter);
    // 画连线
    drawLines(painter);
    // 画角色位置
    painter.drawPixmap(role1->x(), role1->y(), deltx * 2 / 3, delty * 2 / 3, *detective);
    if (Mode == 2)
        painter.drawPixmap(role2->x(), role2->y(), deltx * 2 / 3, delty * 2 / 3, *mage);
    // 失败处理
    if (state == 0) {
        painter.drawPixmap(locx[3], locy[1], locx[xdivide - 6], locy[ydivide - 6], *failure);
    }
    // 成功处理
    else if (state == 2)
        painter.drawPixmap(locx[3], locy[1], locx[xdivide - 6], locy[ydivide - 6], *winning);
    // 画介绍
    if (drawIntro)
        painter.drawPixmap(0, 0, width() - 2 * columnWidth, columnHeight - 2 * ybegin, intro);
    // 分数边框
    painter.drawPixmap(0.3 * deltx - columnWidth, locy[ydivide / 2] - 0.2 * delty - height() / 10, 2.9 * deltx,
                       2.4 * delty, scoreSide);
    if (Mode == 2)
        painter.drawPixmap(width() - 3.2 * deltx - columnWidth, locy[ydivide / 2] - 0.2 * delty - height() / 10,
                           2.9 * deltx, 2.4 * delty, scoreSide);
}

// 初始化
void Map::initial()
{
    setInit = 1;
    LinkType = -1;
    state = 1;                                          // 游戏状态设置
    columnWidth = width() / 9;                          // 边框宽度
    columnHeight = height() / 10 * 9;                   // 边框高度
    delty = (height() - height() / 10.0 * 3) / ydivide; // 单个方格高度
    deltx = (width() - columnWidth * 3) / xdivide;      // 单个方格高度
    xbegin = 0.5 * columnWidth;                         // 画图初始位置
    ybegin = height() / 10.0 * 1.5 - height() / 10;
    setFocusPolicy(Qt::StrongFocus);                            // foucusPolicy设置为强
    linkPoint1x = linkPoint1y = linkPoint2x = linkPoint2y = -1; // 连接点初始化
    int shift = rand() % 500;
    role2->rx() = width() - 2 * columnWidth - (width() - columnWidth * 3) / xdivide * 2 / 3 - shift; // role2位置初始化
    role1->rx() = shift;
    pb->resize(width() / 15 * 12, height() / 30); // 设置进度条geometry
    pb->move(width() / 15, height() / 12 * 11);
    movable1 = 1;
    if (Mode == 1)
        movable2 = 0;
    else
        movable2 = 1;                       // 若单人模式，则role2设置无法移动
    propx1 = propx2 = propy1 = propy2 = -1; // 道具初始位置置零

    // 音乐按钮位置设置
    musicOff.setGeometry(width() - columnWidth + 5, 0, 50, 50);
    musicOn.setGeometry(width() - columnWidth + 20, 0, 35, 50);
    louder.setGeometry(width() - columnWidth + 75, 0, 50, 50);
    lower.setGeometry(width() - columnWidth + 145, 0, 50, 50);

    // 初始化坐标位置
    for (int j = 0; j < xdivide; ++j) {
        locx[j] = columnWidth / 2 + j * deltx;
    }
    for (int i = 0; i < ydivide; ++i) {
        locy[i] = ybegin + i * delty;
    }
    // 画分数板
    role1Score.setParent(this);
    role2Score.setParent(this);
    role1Score.resize(2.4 * deltx, 2 * delty);
    role2Score.resize(2.4 * deltx, 2 * delty);
    role1Score.move(0.5 * deltx, locy[ydivide / 2]);
    role2Score.move(width() - 3 * deltx, locy[ydivide / 2]);
    role1Score.show();
    if (Mode == 2)
        role2Score.show();

    // 结束标语
    clickToContinue.setParent(this);
    clickToContinue.setGeometry(locx[6], locy[1] + locy[ydivide - 4], locx[xdivide - 6], locy[ydivide - 10]);
    clickToContinue.hide();

    // 分数边框
    scoreSide.load(":/new/prefix3/rec/margin/side.png");
    // 标题
    title.load(":/new/prefix2/rec/background/title.jpg");
}

// 画图标
void Map::drawEmoji(QPainter &painter)
{
    QPixmap *temp; // 画图的目标
    for (int i = 0; i < ydivide; ++i) {
        for (int j = 0; j < xdivide; ++j) {
            int tempPix = blockE[i][j];
            switch (tempPix) {
            case 0:
                temp = laughter;
                break;
            case 1:
                temp = smile;
                break;
            case 2:
                temp = winking;
                break;
            case 3:
                temp = clown;
                break;
            case 4:
                temp = enraged;
                break;
            case 5:
                temp = savoring;
                break;
            case 6:
                temp = grining;
                break;
            case 7:
                temp = heart;
                break;
            case 8:
                temp = crying;
                break;
            case 9:
                temp = nerd;
                break;
            case 10:
                temp = partying;
                break;
            case 11:
                temp = pleading;
                break;
            case pixNum:
                temp = timePlus;
                break;
            case pixNum + 1:
                temp = reverse;
                break;
            case pixNum + 2:
                temp = hint;
                break;
            case pixNum + 3:
                temp = flash;
                break;
            case pixNum + 4:
                temp = cold;
                break;
            case pixNum + 5:
                temp = disszy;
                break;
            }
            if (tempPix != -1)
                painter.drawPixmap(locx[j], locy[i], deltx, delty, *temp);
        }
    }
}

// 画连线
void Map::drawLines(QPainter &painter)
{
    if (LinkType != -1) {
        // 0个拐点
        int targetX = 0, targetx = 0, targetY = 0, targety = 0;
        if (Role == 1) {
            targetX = target1X;
            targetx = target1x;
            targetY = target1Y;
            targety = target1y;
            QPen pen;
            pen.setWidth(5);
            pen.setColor(QColor(255, 0, 0));
            painter.setPen(pen);
        } else if (Role == 2) {
            targetX = target2X;
            targetx = target2x;
            targetY = target2Y;
            targety = target2y;
            QPen pen;
            pen.setWidth(5);
            pen.setColor(QColor(255, 0, 255));
            painter.setPen(pen);
        }
        if (targetX == -1 || targetY == -1 || targetx == -1 || targety == -1)
            return;
        if (LinkType == 0) {
            painter.drawLine(locx[targetX] + deltx / 2, locy[targetY] + delty / 2, locx[targetx] + deltx / 2,
                             locy[targety] + delty / 2);
        }
        // 1个拐点
        if (LinkType == 1) {
            painter.drawLine(locx[targetx] + deltx / 2, locy[targety] + delty / 2, locx[linkPoint1x] + deltx / 2,
                             locy[linkPoint1y] + delty / 2);
            painter.drawLine(locx[targetX] + deltx / 2, locy[targetY] + delty / 2, locx[linkPoint1x] + deltx / 2,
                             locy[linkPoint1y] + delty / 2);
        }
        // 两个拐点
        if (LinkType == 2) {
            if (linkPoint1x == -1 || linkPoint1y == -1 || linkPoint1x == xdivide || linkPoint1y == ydivide) {
                if (linkPoint1x == -1) {
                    painter.drawLine(locx[targetX] + deltx / 2, locy[targetY] + delty / 2, locx[0] - deltx / 2,
                                     locy[linkPoint1y] + delty / 2);
                    painter.drawLine(locx[0] - deltx / 2, locy[linkPoint2y] + delty / 2, locx[0] - deltx / 2,
                                     locy[linkPoint1y] + delty / 2);
                    painter.drawLine(locx[targetx] + deltx / 2, locy[targety] + delty / 2, locx[0] - deltx / 2,
                                     locy[linkPoint2y] + delty / 2);
                } else if (linkPoint1x == xdivide) {
                    linkPoint1x--;
                    linkPoint2x--;
                    painter.drawLine(locx[targetX] + deltx / 2, locy[targetY] + delty / 2,
                                     locx[linkPoint1x] + deltx * 3 / 2, locy[linkPoint1y] + delty / 2);
                    painter.drawLine(locx[linkPoint2x] + deltx * 3 / 2, locy[linkPoint2y] + delty / 2,
                                     locx[linkPoint1x] + deltx * 3 / 2, locy[linkPoint1y] + delty / 2);
                    painter.drawLine(locx[targetx] + deltx / 2, locy[targety] + delty / 2,
                                     locx[linkPoint2x] + deltx * 3 / 2, locy[linkPoint2y] + delty / 2);
                } else if (linkPoint1y == -1) {
                    linkPoint1y = 0;
                    linkPoint2y = 0;
                    painter.drawLine(locx[targetX] + deltx / 2, locy[targetY] + delty / 2,
                                     locx[linkPoint1x] + deltx / 2, locy[linkPoint1y] - delty / 2);
                    painter.drawLine(locx[linkPoint2x] + deltx / 2, locy[linkPoint2y] - delty / 2,
                                     locx[linkPoint1x] + deltx / 2, locy[linkPoint1y] - delty / 2);
                    painter.drawLine(locx[targetx] + deltx / 2, locy[targety] + delty / 2,
                                     locx[linkPoint2x] + deltx / 2, locy[linkPoint2y] - delty / 2);
                } else {
                    linkPoint1y--;
                    linkPoint2y--;
                    painter.drawLine(locx[targetX] + deltx / 2, locy[targetY] + delty / 2,
                                     locx[linkPoint1x] + deltx / 2, locy[linkPoint1y] + delty * 3 / 2);
                    painter.drawLine(locx[linkPoint2x] + deltx / 2, locy[linkPoint2y] + delty * 3 / 2,
                                     locx[linkPoint1x] + deltx / 2, locy[linkPoint1y] + delty * 3 / 2);
                    painter.drawLine(locx[targetx] + deltx / 2, locy[targety] + delty / 2,
                                     locx[linkPoint2x] + deltx / 2, locy[linkPoint2y] + 3 * delty / 2);
                }
            } else {
                painter.drawLine(locx[targetX] + deltx / 2, locy[targetY] + delty / 2, locx[linkPoint1x] + deltx / 2,
                                 locy[linkPoint1y] + delty / 2);
                painter.drawLine(locx[linkPoint2x] + deltx / 2, locy[linkPoint2y] + delty / 2,
                                 locx[linkPoint1x] + deltx / 2, locy[linkPoint1y] + delty / 2);
                painter.drawLine(locx[targetx] + deltx / 2, locy[targety] + delty / 2, locx[linkPoint2x] + deltx / 2,
                                 locy[linkPoint2y] + delty / 2);
            }
        }
    }
}

// 画选中方块
void Map::drawBlock(QPainter &painter)
{
    // 画出选中方块
    // 如果选中方块在规定范围内，则画出方格
    if (target1X >= 0 && target1X < xdivide && target1Y >= 0 && target1Y < ydivide) {
        QPen pen;
        pen.setWidth(5);
        pen.setColor(QColor(255, 0, 0));
        painter.setPen(pen);
        painter.drawRect(locx[target1X], locy[target1Y], deltx, delty);
    }

    if (target2X >= 0 && target2X < xdivide && target2Y >= 0 && target2Y < ydivide) {
        QPen pen;
        pen.setWidth(5);
        pen.setColor(QColor(255, 0, 255));
        painter.setPen(pen);
        painter.drawRect(locx[target2X], locy[target2Y], deltx, delty);
    }

    if (target1x >= 0 && target1x < xdivide && target1y >= 0 && target1y < ydivide) {
        QPen pen;
        pen.setWidth(5);
        pen.setColor(QColor(255, 0, 0));
        painter.setPen(pen);
        painter.drawRect(locx[target1x], locy[target1y], deltx, delty);
    }

    if (target2x >= 0 && target2x < xdivide && target2y >= 0 && target2y < ydivide) {
        QPen pen;
        pen.setWidth(5);
        pen.setColor(QColor(255, 0, 255));
        painter.setPen(pen);
        painter.drawRect(locx[target2x], locy[target2y], deltx, delty);
    }
    if (target1X >= 0 && target1X < xdivide && target1Y >= 0 && target1Y < ydivide){
        if(target1X == target2X && target1Y == target2Y){  // 两个人选中同一个方块，显示为白色
            QPen pen;
            pen.setWidth(5);
            pen.setColor(QColor(255,255,255));
            painter.setPen(pen);
            painter.drawRect(locx[target1X], locy[target1Y], deltx, delty);
        }
    }
}

Map::~Map()
{
    delete ui;
}

// 设置玩家1位置
void Map::setRole1(int x, int y)
{
    role1->rx() = x; // 把角色1位置放置
    role1->ry() = y;
}

// 设置玩家2位置
void Map::setRole2(int x, int y)
{
    role2->rx() = x; // 把角色2位置放置
    role2->ry() = y;
}

// 设置倒计时时间  public
void Map::setTime(int v)
{
    timeRecord = v; // 时间设置
}

// 触发道具分配
void Map::setProp(int item, int role)
{
    // item表示道具类型
    // item==0: +1s
    //       1：shuffle
    //       2:10s hint
    //       3:flash
    //       4:freeze
    //       5:disszy

    switch (item) {
    case 0: // 时间+30
        timeRecord = timeRecord + 30;
        if (timeRecord > timeMax)
            timeRecord = timeMax;
        break;
    case 1: // 重置地图
        setMap(-2);
        break;
    case 2:
        isHint = 1;
        QTimer::singleShot(10000, [&]() { // 开启提示
            isHint = 0;
        });
        break;
    case 3:
        mouseControl = 1;
        QTimer::singleShot(5000, [&]() { // 开启鼠标
            mouseControl = 0;
        });
        break;
    case 4: // 对方无法移动3s
        if (role == 1) {
            movable2 = 0;
            QTimer::singleShot(3000, [&]() { movable2 = 1; });
        } else {
            movable1 = 0;
            QTimer::singleShot(3000, [&]() { movable1 = 1; });
        }
        break;
    case 5: // 对方disszy
        if (role == 1) {
            isDisszy2 = 1;
            QTimer::singleShot(10000, [&]() { isDisszy2 = 0; });
        } else {
            isDisszy1 = 1;
            QTimer::singleShot(10000, [&]() { isDisszy1 = 0; });
        }
    }
}

// 地图设置
void Map::setMap(int setMapType)
{
    // 地图类型随机数
    mapType = setMapType;   // 地图类型
    if (setMapType == 2) {
        blockNum = xdivide * ydivide / 6 * 2;
        setMapType = -2;
    } // mapType==2 80个乱序方格
    if (setMapType == 3) {
        blockNum = xdivide * ydivide / 5 * 2;
        setMapType = -2;
    } // mapType==3 100个乱序方格

    target1X = target1Y = target1x = target1y = target2X = target2Y = target2x = target2y = -1; // 清除目标画框位置
    hintx1 = hintx2 = hinty1 = hinty2; // 清除hint目标画框位置

    if (setMapType == -2) {
        role1->rx() = 0;
        role1->ry() = 0;
        role2->rx() = width() - 2 * columnWidth - (width() - columnWidth * 3) / xdivide * 2 / 3;
        role2->ry() = 0;
        int tempPix[xdivide * ydivide / 2]; // 存放一半的pix类型，用于一一成对出现
        // 置零初始化
        for (int i = 0; i < ydivide; ++i) {
            for (int j = 0; j < xdivide; ++j)
                blockE[i][j] = -1;
        }
        propNum = 0;                                   // 道具数量清零
        for (int num = 0; num < blockNum / 2; ++num) { // 对一半的位置进行放置图案
            int i = rand() % ydivide, j = rand() % xdivide;
            while (blockE[i][j] != -1) {
                i = rand() % ydivide;
                j = rand() % xdivide;
            }
            tempPix[num] = blockE[i][j] = rand() % pixNum;
        }
        for (int num = blockNum / 2 - 1; num >= 0; --num) { // 对另一半的位置进行放置图案
            int i = rand() % ydivide, j = rand() % xdivide;
            while (blockE[i][j] != -1) {
                i = rand() % ydivide;
                j = rand() % xdivide;
            }
            blockE[i][j] = tempPix[num];
        }
    }

    // 全充满地图 地图0
    // 填充地图
    if (mapType == 0) {
        blockNum = xdivide * ydivide;
        for (int i = 0; i < ydivide; ++i) {
            for (int j = 0; j < xdivide / 2; ++j) {
                int tempPix = rand() % pixNum;
                blockE[i][j] = tempPix;
                blockE[i][j + xdivide / 2] = tempPix;
            }
        }

        // 随机交换顺序
        for (int i = 0; i < xdivide * ydivide * 2; ++i) {
            int tempPixx1 = rand() % xdivide;
            int tempPixy1 = rand() % ydivide;

            int tempPixx2 = rand() % xdivide;
            int tempPixy2 = rand() % ydivide;

            int temp;
            temp = blockE[tempPixy1][tempPixx1];
            blockE[tempPixy1][tempPixx1] = blockE[tempPixy2][tempPixx2];
            blockE[tempPixy2][tempPixx2] = temp;
        }
    }
    // SJTU 地图 地图1
    else if (mapType == 1) {
        blockNum = 84;
        int tempMap[ydivide][xdivide] = {// 地图形状展示   SJTU图样
                                         {1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1},
                                         {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
                                         {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
                                         {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
                                         {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
                                         {1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
                                         {1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
                                         {0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
                                         {0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
                                         {0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
                                         {0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
                                         {1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1}};
        for (int i = 0; i < ydivide; ++i) { // 地图初始化置零

            for (int j = 0; j < xdivide; ++j) {
                blockE[i][j] = -1;
            }
        }

        int tempPix[xdivide * ydivide / 2];
        int countPix = 0;
        for (int i = 0; i < ydivide; ++i) { // 随机数放置图案

            for (int j = 0; j < xdivide / 2; ++j) {
                if (tempMap[i][j])
                    tempPix[countPix++] = blockE[i][j] = rand() % pixNum;
                else {
                    blockE[i][j] = -1;
                }
            }
        }

        for (int i = 0; i < ydivide; ++i) { // 放置相对应图案，凑为偶数个

            for (int j = xdivide / 2; j < xdivide; j += 1) {
                if (tempMap[i][j])
                    blockE[i][j] = tempPix[--countPix];
                else
                    blockE[i][j] = -1;
            }
        }

        // 调换顺序
        for (int i = 0; i < xdivide * ydivide * 2; ++i) {
            int tempPixy1 = rand() % ydivide;
            int tempPixx1 = rand() % xdivide;

            int tempPixy2 = rand() % ydivide;
            int tempPixx2 = rand() % xdivide;

            while (!(tempMap[tempPixy1][tempPixx1] && tempMap[tempPixy2][tempPixx2])) {
                tempPixy1 = rand() % ydivide;
                tempPixx1 = rand() % xdivide;

                tempPixy2 = rand() % ydivide;
                tempPixx2 = rand() % xdivide;
            }

            int temp;
            temp = blockE[tempPixy1][tempPixx1];
            blockE[tempPixy1][tempPixx1] = blockE[tempPixy2][tempPixx2];
            blockE[tempPixy2][tempPixx2] = temp;
        }
    }
    if (!hintBlock())
        setMap(setMapType);
}

// 标注Hint方块
bool Map::hintBlock()
{
    // 遍历所有位置找到可连接方块
    for (int i = 0; i < ydivide; ++i) {
        for (int j = 0; j < xdivide; ++j) {
            for (int i1 = 0; i1 < ydivide; ++i1) {
                for (int j1 = 0; j1 < xdivide; ++j1) {
                    if (!(i == i1 && j == j1)) {
                        if (blockE[i1][j1] < 0 || blockE[i1][j1] > pixNum)
                            continue;
                        if (islinked(j, i, j1, i1, 0) && blockE[i][j] < pixNum) {
                            hintx1 = j;
                            hintx2 = j1;
                            hinty1 = i;
                            hinty2 = i1;
                            return 1;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

// 画hint方格的边框
void Map::drawHint(QPainter &painter)
{
    QPen pen(QColor(0, 0, 0));
    pen.setWidth(5);
    painter.setPen(pen);
    painter.drawRect(locx[hintx1], locy[hinty1], deltx, delty); // 对应方格画框
    painter.drawRect(locx[hintx2], locy[hinty2], deltx, delty); // 对应方格画框
}

// 随机生成道具
void Map::createProp()
{
    // item表示道具类型
    // item==0: +1s
    //       1：shuffle
    //       2:10s hint
    //       3:flash
    //       4:freeze
    //       5:disszy
    static int lastProp = -1;
    if (propNum >= 2)
        return;
    if (blockNum >= xdivide * ydivide - 10 || blockNum == 0)
        return;                              // 地图太满则不放置道具
    int item = rand() % (Mode == 1 ? 4 : 6); // 单人模式前四种道具，双人六种

    // 确保不会在hint状态下仍然产生hint道具等，且不会前后两次产生道具相同
    while ((item == 2 && isHint) || (mouseControl && item == 3) || (item == 4 && (!movable1 && !movable2)) ||
           (item == 5 && (isDisszy1 && isDisszy2)) || item == lastProp) {
        item = rand() % (Mode == 1 ? 4 : 6);
    }

    if (item == 3 && Mode == 2)
        item = 4; // 双人模式下去掉鼠标道具
    lastProp = item;
    int i = rand() % ydivide, j = rand() % xdivide; // 随机设定放置位置
    while (blockE[i][j] != -1) {
        i = rand() % ydivide;
        j = rand() % xdivide;
    }

    blockE[i][j] = item + pixNum; // 放置位置
    propNum++;                    // 现存道具数++
    if (propx1 == -1) {
        propx1 = j;
        propy1 = i;
    } // 记录道具位置
    else {
        propx2 = j;
        propy2 = i;
    }

    QTimer::singleShot(10000, [=]() { // 道具定时消失
        if (blockE[i][j] > pixNum) {
            blockE[i][j] = -1;
            propNum--;
        }
    });
}

// 触发道具的处理
void Map::isProp(int &frontX, int &frontY, int role)
{
    if (propx1 == frontX && propy1 == frontY)
        propx1 = propy1 = -1;
    else
        propx2 = propy2 = -1; // 记录道具位置
    int type = blockE[frontY][frontX] - pixNum;
    blockE[frontY][frontX] = -1;
    setProp(type, role);  // 触发道具处理
    propNum--;            // 现存道具--
    frontX = frontY = -1; // 当前前置位置为空白
}

// 按下鼠标的操作
void Map::mousePressEvent(QMouseEvent *event)
{
    if (mouseControl) {
        // 获得鼠标位置
        int x = event->x() - columnWidth;
        int y = event->y() - height() / 10;
        // 计算当前方块
        int roleX = (x - xbegin) / deltx;
        int roleY = (y - ybegin) / delty;
        if (x < 0 || x > width() - 2 * columnWidth - deltx * 2 / 3 || y < 0 ||
            y > columnHeight - height() / 10 - delty * 2 / 3)
            return;
        // 方块位置外
        if (roleX < 0 || roleX >= xdivide || roleY < 0 || roleY >= ydivide) {
            role1->rx() = x;
            role1->ry() = y;
        }
        // 空白方块
        if (blockE[roleY][roleX] == -1) {
            role1->rx() = x;
            role1->ry() = y;
        }
        // 边缘方块
        if (roleX == 0) {
            role1->rx() = locx[0] - deltx * 2 / 3;
            role1->ry() = y;
            mouseLink(roleX, roleY);
        } else if (roleX == xdivide - 1) {
            role1->rx() = locx[xdivide - 1] + deltx;
            role1->ry() = y;
            mouseLink(roleX, roleY);
        } else if (roleY == 0) {
            role1->rx() = x;
            role1->ry() = locy[0] - delty / 2;
            mouseLink(roleX, roleY);
        } else if (roleY == ydivide - 1) {
            role1->rx() = x;
            role1->ry() = locy[ydivide - 1] + delty;
            mouseLink(roleX, roleY);
        }
        // 中心方块
        else if (roleX - 1 >= 0 && roleX - 1 < xdivide && blockE[roleY][roleX - 1] == -1) {
            role1->rx() = locx[roleX - 1];
            role1->ry() = locy[roleY];
            mouseLink(roleX, roleY);
        } else if (roleX + 1 >= 0 && roleX + 1 < xdivide && blockE[roleY][roleX + 1] == -1) {
            role1->rx() = locx[roleX + 1];
            role1->ry() = locy[roleY];
            mouseLink(roleX, roleY);
        } else if (roleY - 1 >= 0 && roleY - 1 < ydivide && blockE[roleY - 1][roleX] == -1) {
            role1->rx() = locx[roleX];
            role1->ry() = locy[roleY - 1];
            mouseLink(roleX, roleY);
        } else if (roleY + 1 >= 0 && roleY + 1 < ydivide && blockE[roleY + 1][roleX] == -1) {
            role1->rx() = locx[roleX];
            role1->ry() = locy[roleY + 1];
            mouseLink(roleX, roleY);
        }

        // 边缘处理
        if (role1->rx() < 0)
            role1->rx() = 0;
        if (role1->ry() < 0)
            role1->ry() = 0;
        if (role1->rx() > width() - 2 * columnWidth - deltx * 2 / 3)
            role1->rx() = width() - 2 * columnWidth - deltx;
        if (role1->ry() > columnHeight - height() / 10 - delty * 2 / 3)
            role1->ry() = columnHeight - height() / 10 - delty;
        update();
    }
    if (state == 0 || state == 2) {
        emit endGame(); // 如果结束则触发endGame
    }
}
// 鼠标状态下连接
void Map::mouseLink(int roleX, int roleY)
{
    if (blockE[roleY][roleX] == -1)     // 空白方块
        return;
    Role = 1;                    // 角色1
    if (target1X == -1 || target1Y == -1) { // 如果前置为空白
        target1X = roleX;               // 前置位置为当前位置
        target1Y = roleY;           // 前置位置为当前位置
        if (blockE[roleY][roleX] >= pixNum)
            isProp(target1X, target1Y, 1);
    } else {
        target1x = roleX;
        target1y = roleY;
        if (target1X == -1 || target1x == -1)
            return;
        if (blockE[roleY][roleX] >= pixNum) 
            isProp(target1x, target1y, 1);
        islinked(target1X, target1Y, target1x, target1y, 1);
        update();
    }
}

// 信号
void Map::endGame()
{
}

// 设置地图类型 public
void Map::setMapType(int mapType)
{
    this->mapType = mapType;
}

// 设置玩家个数 public
void Map::setMode(int mode)
{
    Mode = mode;
}

// 失败处理
void Map::fail()
{
    if (Mode == 1) {
        movable1 = 0;
        state = 0;
    } else {
        movable1 = 0;
        state = 0;

    }
    for (int i = 0; i < ydivide; ++i) {
        for (int j = 0; j < xdivide; ++j)
            blockE[i][j] = -1;
    }
    update();
    propNum = blockNum = 0; // 道具数清零
    clickToContinue.setimage(QPixmap(":/new/prefix3/rec/end/click to continue(BLUE).jpg")); // 设置图片
    clickToContinue.show(); // 显示
}
// 成功处理
void Map::win()
{
    if (Mode == 1) {
        movable1 = 0;
        state = 2;
    } else {
        movable1 = movable2 = 0;
        state = 2;
    }
    for (int i = 0; i < ydivide; ++i) {
        for (int j = 0; j < xdivide; ++j)
            blockE[i][j] = -1;
    }
    update();
    propNum = blockNum = 0; // 道具数清零
    clickToContinue.setimage(QPixmap(":/new/prefix3/rec/end/click to continue (RED).jpg"));
    clickToContinue.show();     // 显示
}
// 设置是否可移动
void Map::setMovable(bool m1, bool m2)
{
    movable1 = m1;  // 设置是否可移动
    movable2 = m2;
}
// 音乐控制
void Map::musicPlay()
{
    // 音乐播放
    playlist = new QMediaPlaylist;
    music = new QMediaPlayer;
    playlist->addMedia(QUrl::fromLocalFile("./wedding.mp3"));
    playlist->addMedia(QUrl::fromLocalFile("./merryChirstmasMr.mp3"));
    playlist->addMedia(QUrl::fromLocalFile("./gameOn.mp3"));
    playlist->addMedia(QUrl::fromLocalFile("./endingMusic.mp3"));
    // std::cout<<playlist->currentIndex()<<std::endl;
    // std::cout<<playlist->nextIndex()<<std::endl;
    playlist->setCurrentIndex(0);

    music = new QMediaPlayer;
    music->setPlaylist(playlist);
    music->setPosition(1);
    music->setVolume(50); // 设置音量为50%
    volume = 50;

    music->play();

    // 音乐图标
    louder.setParent(this);
    lower.setParent(this);
    musicOn.setParent(this);
    musicOff.setParent(this);

    // 图标设置
    louder.setimage(QPixmap(":/new/prefix3/rec/margin/musicLouder.png"));
    lower.setimage(QPixmap(":/new/prefix3/rec/margin/musicLower.png"));
    musicOn.setimage(QPixmap(":/new/prefix3/rec/margin/musicOn.png"));
    musicOff.setimage(QPixmap(":/new/prefix3/rec/margin/musicOff.png"));

    // 按钮状态
    musicOn.hide();
    musicOff.show();
    louder.show();
    lower.show();
    update();

    // 按下musicOff，出现MusicOn图标
    connect(&musicOff, &QPushButton::clicked, [=]() {
        music->pause();
        musicOff.hide();
        musicOn.show();
    });
    connect(&musicOn, &QPushButton::clicked, [=]() {
        music->play();
        musicOff.show();
        musicOn.hide();
    });
    connect(&louder, &QPushButton::clicked, [=]() {
        volume += 10;
        if (volume > 100)
            volume = 100;
        music->setVolume(volume);
    });
    connect(&lower, &QPushButton::clicked, [=]() {
        volume -= 10;
        if (volume < 0)
            volume = 0;
        music->setVolume(volume);
    });
}
// 道具读档处理，为正在生效的道具配置消失时刻
void Map::setPropTime()
{
    for (int i = 0; i < ydivide; ++i) {
        for (int j = 0; j < xdivide; ++j) {
            if (blockE[i][j] >= pixNum) {
                int tempx = j, tempy = i;
                QTimer::singleShot(10000,
                                   [=]() { // 检测到图上存在道具，把道具设置限定时间消失
                                       blockE[tempy][tempx] = -1;
                                       propNum--;
                                   });
            }
        }
    }
    if (isHint == 1)
        QTimer::singleShot(10000, [=]() { // hint道具处理
            isHint = 0;
        });
    if (mouseControl == 1)
        QTimer::singleShot(5000, [=]() { // 鼠标道具处理
            mouseControl = 0;
        });
    if (movable1 == 0 && Mode == 2)
        QTimer::singleShot(3000, [=]() { // 冰冻道具处理
            movable1 = 1;
        });
    if (movable2 == 0 && Mode == 2)
        QTimer::singleShot(3000, [=]() { // 冰冻道具处理
            movable2 = 1;
        });
    if (isDisszy1 == 1 && Mode == 2)
        QTimer::singleShot(3000, [=]() { // 反向移动道具处理
            isDisszy1 = 0;
        });
    if (isDisszy2 == 1 && Mode == 2)
        QTimer::singleShot(3000, [=]() { // 反向移动道具处理
            isDisszy2 = 0;
        });
}
// 导入emoji
void Map::downloadEmoji()
{
    laughter = new QPixmap(":/new/prefix2/rec/emoji/laughing.png");
    smile = new QPixmap(":/new/prefix2/rec/emoji/smiling face with hearts.png");
    winking = new QPixmap(":/new/prefix1/rec/emoji/frog.png");
    clown = new QPixmap(":/new/prefix2/rec/emoji/clown face.png");
    enraged = new QPixmap(":/new/prefix2/rec/emoji/enraged face.png");
    savoring = new QPixmap(":/new/prefix2/rec/emoji/face savoring food.png");
    grining = new QPixmap(":/new/prefix2/rec/emoji/grinning face with sweat.png");
    heart = new QPixmap(":/new/prefix2/rec/emoji/heart with ribbon.png");
    crying = new QPixmap(":/new/prefix1/rec/emoji/bear.png");
    nerd = new QPixmap(":/new/prefix2/rec/emoji/nerd face.png");
    partying = new QPixmap(":/new/prefix2/rec/emoji/partying face.png");
    pleading = new QPixmap(":/new/prefix2/rec/emoji/pleading face.png");
    detective = new QPixmap(":/new/prefix2/rec/emoji/detective.png");
    disszy = new QPixmap(":/new/prefix2/rec/emoji/power/dissy.png");
    cold = new QPixmap(":/new/prefix2/rec/emoji/power/cold.png");
    hint = new QPixmap(":/new/prefix2/rec/emoji/power/star.png");
    flash = new QPixmap(":/new/prefix2/rec/emoji/power/mouse.png");
    reverse = new QPixmap(":/new/prefix2/rec/emoji/power/reverse.png");
    timePlus = new QPixmap(":/new/prefix2/rec/emoji/power/timePlus.png");
    mage = new QPixmap(":/new/prefix2/rec/emoji/mage.png");
    failure = new QPixmap(":/new/prefix3/rec/end/failure.png");
    winning = new QPixmap(":/new/prefix3/rec/end/win.png");
}

// 菜单栏等位置设置
void Map::setMenu()
{
    // 存档
    upLoad.setParent(this);
    upLoad.setimage(QPixmap(":/new/prefix3/rec/margin/upload.png"));
    upLoad.hide();
    upLoad.setGeometry(150, 50, 50, 50);
    connect(&upLoad, &QPushButton::clicked, [=]() {
        recordData();
        upLoad.hide();
        downLoad.hide();
        Introduction.hide();
    });

    // 读档
    downLoad.setParent(this);   // 设置父类
    downLoad.setimage(QPixmap(":/new/prefix3/rec/margin/download.png"));
    downloadError.setimage(QPixmap(":/new/prefix1/rec/background/you have not.jpg"));
    downloadError.hide();    // 设置隐藏
    downloadError.setGeometry(locx[0], locy[2], locx[xdivide - 1] - locx[0], locy[2]);
    downLoad.hide();
    downLoad.setGeometry(150, 120, 50, 50);
    connect(&downLoad, &QPushButton::clicked, [=]() {
        try {
            readData();
        } catch (QString s) {
            readDataError();
        }
        upLoad.hide();
        downLoad.hide();
        Introduction.hide();
    });

    // 介绍设置
    Introduction.setimage(QPixmap(":/new/prefix3/rec/margin/Introduction.png"));
    Introduction.setParent(this);
    Introduction.setGeometry(150, 190, 50, 50);
    Introduction.hide();
    intro.load(":/new/prefix1/rec/background/introduction3.1.png");
    drawIntro = 0;
    connect(&Introduction, &QPushButton::clicked, [=]() {
        if (drawIntro) {
            Introduction.hide();
            downLoad.hide();
            upLoad.hide();
            setMovable(m1, m2);
            timer->start(1000);
            timer2->start(6000);
        } else {
            m1 = movable1;
            m2 = movable2;
            setMovable(0, 0);
            timer->stop();
            timer2->stop();
        }
        drawIntro = !drawIntro;
        update();
    });

    // 菜单栏
    menu.setimage(QPixmap(":/new/prefix3/rec/margin/menu.png"));
    menu.setParent(this);
    menu.show();
    menu.setGeometry(150, 0, 50, 50);
    connect(&menu, &QPushButton::clicked, [=]() {
        if (downLoad.isHidden()) {
            downLoad.show();
            upLoad.show();
            Introduction.show();
        } else {
            downLoad.hide();
            upLoad.hide();
            Introduction.hide();
            drawIntro = 0;
            update();
            setMovable(m1, m2);
            timer->start(1000);
            timer2->start(6000);
        }
    });
}

// 设置时间/进度条
void Map::setTime_pb()
{
    timer = new QTimer(this); // 设置计时器

    pb = new QProgressBar(this); // 进度条
    pb->setStyleSheet("QProgressBar{border: none;color: white;text-align: center;background: "
                      "rgb(68, 69, 73);} QProgressBar::chunk{border: none;background: rgb(0, "
                      "160, 230);}");
    if (mapType == 0)
        timeMax = timeRecord = 500; // 时间范围设置，各地图时间不同
    else if (mapType == 1)
        timeMax = timeRecord = 200;
    else if (mapType == 2)
        timeMax = timeRecord = 200;
    else if (mapType == 3)
        timeMax = timeRecord = 250;
    timer->start(1000); // 设定计时器，1s一次
    pb->setFormat((QString::number(timeRecord, 10)));
    pb->setValue(timeMax);        // 进度条置零
    pb->setRange(0, timeMax);     // 进度条设置
    pb->setInvertedAppearance(1); // 逆置
    pb->show();
    connect(timer, &QTimer::timeout, [=]() { // 时间显示
        if (!this->isHidden())
            pb->setValue(--timeRecord);
        pb->setFormat((QString::number(timeRecord, 10)));
        if (timeRecord == 0) {
            timer->stop();
            // 失败处理
            fail();
        }
        // std::cout<<"value  "<<pb->value()<<std::endl;   //时间显示
    });
}

// 返回键按钮设置
void Map::setBack_Stop()
{
    backstart.setParent(this);
    backstart.setText("Back");
    backstart.setimage(QPixmap(":/new/prefix3/rec/margin/back.png"));
    backstart.setGeometry(0, 0, 60, 50);
    backstart.show();

    // 暂停开始键设置
    stopGame.setimage(QPixmap(":/new/prefix3/rec/margin/stop.png"));
    continueGame.setimage(QPixmap(":/new/prefix3/rec/margin/continue.png"));
    stopGame.setParent(this);
    continueGame.setParent(this);
    stopGame.setGeometry(80, 0, 50, 50);
    continueGame.setGeometry(80, 0, 50, 50);
    stopGame.show();
    continueGame.hide();
}

// 画边框
void Map::drawMargin(QPainter &painter)
{
    switch (bgType) { // 背景图种类设置
    case 0:
        painter.drawPixmap(0, 0, this->width(), this->height(),
                           QPixmap(":/new/prefix2/rec/background/gamebackground.jpg"));
        break;
    case 1:
        painter.drawPixmap(0, 0, this->width(), this->height(),
                           QPixmap(":/new/prefix2/rec/background/gamebackground1.jpg"));
        break;
    case 2:
        painter.drawPixmap(0, 0, this->width(), this->height(),
                           QPixmap(":/new/prefix2/rec/background/gamebackground2.jpg"));
        break;
    case 3:
        painter.drawPixmap(0, 0, this->width(), this->height(),
                           QPixmap(":/new/prefix2/rec/background/gamebackground3.jpg"));
        break;
    case 4:
        painter.drawPixmap(0, 0, this->width(), this->height(),
                           QPixmap(":/new/prefix2/rec/background/gamebackground4.png"));
        break;
    }
    QBrush brush(QColor(255, 255, 0), Qt::Dense4Pattern);
    painter.setBrush(brush);

    // painter.setBrush(QColor(220,255,255));
    // painter.drawRect(0,0,width(),height());

    painter.setPen(QColor(0, 255, 0));
    painter.drawLine(columnWidth, 0, columnWidth, columnHeight); // 左侧竖线
    painter.drawLine(width() - columnWidth, 0, width() - columnWidth,
                     columnHeight); // 右侧竖线

    painter.drawLine(0, columnHeight, width(), columnHeight); // 底边线

    painter.drawLine(columnWidth, height() / 10, width() - columnWidth,
                     height() / 10); // 顶边线

    // painter.setBrush(QColor(240,255,255));
    // painter.drawRect(columnWidth,height()/10,width()-2*columnWidth,columnHeight-height()/10);
    // //Map center
}
