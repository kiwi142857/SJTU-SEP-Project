#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QAction>
#include "mainwindow.h"
class QGraphicsScene;
class QKeyEvent;

class Snake;
class Food;
class Wall;

class GameController : public QObject
{
    Q_OBJECT
public:
    GameController(QGraphicsScene &scene, QObject *parent = 0);
    ~GameController();

    void snakeAteFood(Food *food);
//    void snakeHitWall(Snake *snake, Wall *wall);
    void snakeAteItself();
    QAction *getResmueAction(){ return resumeAction;}
    void setResumeAction(QAction* r){ resumeAction = r; }
    void setPauseAction(QAction* p){ pauseAction = p; }
    void setPause(bool p){ isPause = p; }
    void easy();
    void normal();
    void hard();
public slots:
    void pause();
    void resume();
    void gameOver();
protected:
     bool eventFilter(QObject *object, QEvent *event);

private:
    void handleKeyPressed(QKeyEvent *event);
    void addNewFood();
    void setResume();
    void setPause();
    QAction * resumeAction;
    QAction * pauseAction;
    QTimer timer;
    QGraphicsScene &scene;
    Snake *snake;
    bool isPause;
};

#endif // GAMECONTROLLER_H
