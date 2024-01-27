#include <QAction>
#include <QEvent>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QMessageBox>
#include <iostream>

#include "food.h"
#include "gamecontroller.h"
#include "mainwindow.h"
#include "snake.h"

GameController::GameController(QGraphicsScene &scene, QObject *parent)
    : QObject(parent), scene(scene), snake(new Snake(*this)), isPause(false)
{
    timer.start(1000 / 33);

    Food *a1 = new Food(0, -50);
    scene.addItem(a1);

    scene.addItem(snake);
    scene.installEventFilter(this);
    // resume();
    connect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));
    isPause = false;
}

GameController::~GameController()
{
}

void GameController::snakeAteFood(Food *food)
{
    scene.removeItem(food);

    addNewFood();
}

// void GameController::snakeHitWall(Snake *snake, Wall *wall)
//{
// }

void GameController::snakeAteItself()
{
    QTimer::singleShot(0, this, SLOT(gameOver()));
}

void GameController::handleKeyPressed(QKeyEvent *event)
{
    if (!isPause)
        switch (event->key()) {
        case Qt::Key_Left:
        case Qt::Key_A:
            snake->setMoveDirection(Snake::MoveLeft);
            break;
        case Qt::Key_Right:
        case Qt::Key_D:
            snake->setMoveDirection(Snake::MoveRight);
            break;
        case Qt::Key_Up:
        case Qt::Key_W:
            snake->setMoveDirection(Snake::MoveUp);
            break;
        case Qt::Key_Down:
        case Qt::Key_S:
            snake->setMoveDirection(Snake::MoveDown);
            break;
        case Qt::Key_Space:
            pause();
            break;
        }
    else
        resume();
}

void GameController::addNewFood()
{
    int x, y;

    do {
        x = (int)(qrand() % 200) / 10 - 10;
        y = (int)(qrand() % 200) / 10 - 10;

        x *= 10;
        y *= 10;
    } while (snake->shape().contains(snake->mapFromScene(QPointF(x + 5, y + 5))));

    Food *food = new Food(x, y);
    scene.addItem(food);
}

void GameController::gameOver()
{
    disconnect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));
    if (QMessageBox::Yes == QMessageBox::information(NULL, tr("Game Over"), tr("Again?"),
                                                     QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)) {
        connect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));
        scene.clear();

        snake = new Snake(*this);
        scene.addItem(snake);
        addNewFood();
    } else {
        exit(0);
    }
}

void GameController::pause()
{
    disconnect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));
    isPause = true;
    setResume();
    setPause();
}

void GameController::resume()
{
    connect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));
    isPause = false;
    setResume();
    setPause();
}

#include <QAction> // Include the necessary header file

void GameController::setResume()
{
    if (isPause == true) {
        resumeAction->setEnabled(true);
    } else {
        resumeAction->setEnabled(false);
    }
}

void GameController ::setPause()
{
    if (isPause == true) {
        pauseAction->setEnabled(false);
    } else {
        pauseAction->setEnabled(true);
    }
}

bool GameController::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        handleKeyPressed((QKeyEvent *)event);
        return true;
    } else {
        return QObject::eventFilter(object, event);
    }
}

void GameController::easy()
{
    timer.start(1000 / 10);
}

void GameController::normal()
{
    timer.start(1000 / 33);
}

void GameController::hard()
{
    timer.start(1000 / 100);
}

