#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QGraphicsScene;
class QGraphicsView;

class GameController;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void adjustViewSize();
	void newGame();
	void gameHelp();
	void about();

private:
	void createActions();
	void createMenus();

    void initScene();
    void initSceneBackground();


    QGraphicsScene *scene;
    QGraphicsView *view;

    GameController *game;

	QAction *newGameAction;
	QAction *pauseAction;
	QAction *resumeAction;
	QAction *exitAction;
	QAction *gameHelpAction;
	QAction *aboutAction;
	QAction *aboutQtAction;
	QAction *easyAction;
	QAction *normalAction;
	QAction *hardAction;
};

#endif // MAINWINDOW_H
