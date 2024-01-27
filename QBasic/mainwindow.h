#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "CodeVector.h"
#include <QMainWindow>
QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private slots:
    void on_cmdLineEdit_returnPressed();
    void on_btnClearCode_clicked();
    void on_btnRunCode_clicked();
    void on_btnLoadCode_clicked();

  private:
    Ui::MainWindow *ui;
    CodeVector CodeVector;
    bool inputMode;
    // 判断输入是否是行号加内容
    int isLineNumber(QString cmd);
    // 布尔值保存程序是否完整运行过
    bool isRunning;
    void displayCode();
    void treeDisplay();
    // 接着上次运行的地方继续运行
    void runContinue();

    // 指向下一条语句的迭代器
    std::map<int, Expression>::iterator it;
    // 单条语句运行
    ExpType runSingleLine();
    string inputVar;
    int nextLine;

    const QString helpText = "Syntax 语法:\n"
                             "REM: 注释\n"
                              "LET: 赋值\n"
                              "PRINT: 打印\n"
                              "INPUT: 输入\n"
                              "GOTO: 跳转\n"
                              "IF THEN: 条件语句\n"
                              "END: 结束\n"
                              "RUN: 运行\n"
                              "LOAD: 加载\n"
                              "LIST: 列出\n"
                              "CLEAR: 清除\n"
                              "HELP: 帮助\n";
        
};
#endif // MAINWINDOW_H
