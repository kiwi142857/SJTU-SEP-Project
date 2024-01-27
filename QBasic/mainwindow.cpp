#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);  // 设置UI
    ui->CodeDisplay->clear(); // 清空代码区
    ui->textBrowser->clear(); // 清空结果区
    ui->treeDisplay->clear(); // 清空语法树区
    ui->cmdLineEdit->setPlaceholderText(">");
    isRunning = false; // 设置运行状态为false
    inputMode = false;  // 设置输入模式为false
    nextLine = 0;   // 设置下一行为0
    // 设置字号
    QFont font;
    font.setPointSize(15);
    ui->treeDisplay->setFont(font);
    ui->CodeDisplay->setFont(font);
    ui->textBrowser->setFont(font);
}

MainWindow::~MainWindow()
{
    delete ui;  // 删除UI
}

void MainWindow::on_cmdLineEdit_returnPressed()
{
    // 判断是否是输入模式
    if (inputMode) {
        // 如果是输入模式，将输入的内容加入变量
        // CodeVector.addVariable(ui->cmdLineEdit->text().toStdString(), 0);
        QString cmdRemovespace = ui->cmdLineEdit->text();
        bool ok;
        cmdRemovespace.remove(" ");
        if (cmdRemovespace == "") {
            ui->cmdLineEdit->clear();
            return;
        }
        int num = cmdRemovespace.toInt(&ok);
        if (ok == 0) {
            QMessageBox::warning(this, "Error", "Invalid input");
            ui->cmdLineEdit->clear();
            return;
        }
        inputMode = false;
        ui->cmdLineEdit->setPlaceholderText(">");
        // ui->cmdLineEdit->setFocus();
        CodeVector.varMap[inputVar].getValue() = num;
        ui->cmdLineEdit->clear();
        // 把输入的内容打印到结果区 ？ 为了和输入区分开，加了问号一个空格
        ui->textBrowser->append("? " + QString::number(num));
        if (isRunning)
            runContinue();  // 继续运行
        return;
    }

    QString cmd = ui->cmdLineEdit->text();  // 获取输入的内容
    // 删除前置空格
    while (cmd.startsWith(" ")) {
        cmd.remove(0, 1);
    }
    if (cmd == "") {
        return;
    }
    int line = -1;
    try {
        line = isLineNumber(cmd);
    } catch (string msg) {
        QMessageBox::warning(this, "Error", msg.c_str());
        ui->cmdLineEdit->clear();
        return;
    }
    if (line != -1) {
        // 如果是行号加内容
        // 将内容加入CodeVector
        // 如果内容删去空格后为空，删除该行
        QString cmdRemovespace = cmd;
        cmdRemovespace.remove(" ");
        if (cmdRemovespace == "") {
            ui->cmdLineEdit->clear();
            return;
        }
        // 如果内容为一个数字，删除该行
        if (cmdRemovespace.toInt() != 0) {
            CodeVector.codeVector.erase(line);
            displayCode();
            ui->cmdLineEdit->clear();
            return;
        }
        try {
            CodeVector.addExpression(line, Expression(cmd.right(cmd.length() - cmd.indexOf(" ")).toStdString(), line));
        } catch (string msg) {
            CodeVector.addExpression(
                line, Expression(ExpType::ERROR, cmd.right(cmd.length() - cmd.indexOf(" ")).toStdString(), line));
        }
        // CodeVector.printCodeVector();
        displayCode();
    } else {
        // 如果不是行号加内容
        if (cmd == "HELP") {
            QMessageBox::information(this, "Help", helpText);
            ui->cmdLineEdit->clear();
            return;
        } else if (cmd == "QUIT") {
            exit(0);
        } else if (cmd == "CLEAR") {
            ui->CodeDisplay->clear();
            ui->textBrowser->clear();
            ui->treeDisplay->clear();
            CodeVector.clear();
            ui->cmdLineEdit->clear();
            isRunning = false;
            inputMode = false;
            ui->cmdLineEdit->setPlaceholderText(">");
            return;
        } else if (cmd == "LIST") {
            displayCode();
            ui->cmdLineEdit->clear();
            return;
        } else if (cmd == "RUN") {
            CodeVector.varMap.clear();
            CodeVector.resetCallTimes();
            isRunning = 1;
            runContinue();
            ui->cmdLineEdit->clear();
            return;
        }
        else if(cmd== "LOAD"){
            on_btnLoadCode_clicked();
            ui->cmdLineEdit->clear();
            return;
        }
        Expression exp;
        try {
            exp = Expression(cmd.toStdString());
        } catch (string msg) {
            QMessageBox::warning(this, "Error", msg.c_str());
            ui->cmdLineEdit->clear();
            return;
        }
        try {
            if (exp.type == PRINT) {
                int ans = exp.executeExp();
                ui->textBrowser->append(QString::number(ans));
            } else if (exp.type == INPUT) {
                exp.executeExp();
                inputMode = true;
                isRunning = false;
                inputVar = exp.root->child->exp;
                ui->cmdLineEdit->clear();
                ui->cmdLineEdit->setPlaceholderText("?");
                ui->cmdLineEdit->setFocus();
                return;
            } else if (exp.getExpression() == "RUN") {
                CodeVector.varMap.clear();
                CodeVector.resetCallTimes();
                isRunning = 1;
                runContinue();
            } else if (exp.getExpression() == "CLEAR") {
                ui->CodeDisplay->clear();
                ui->textBrowser->clear();
                ui->treeDisplay->clear();
                CodeVector.clear();
                isRunning = false;
                ui->cmdLineEdit->clear();
                ui->cmdLineEdit->setPlaceholderText(">");
                inputMode = false;
            } else if (exp.getExpression() == "LIST") {
                displayCode();
            } else if (exp.getExpression() == "HELP") {
                QMessageBox::information(this, "Help", helpText);
            } else if (exp.getExpression() == "QUIT") {
                exit(0);
            } else {
                exp.executeExp();
            }
        } catch (string msg) {
            QMessageBox::warning(this, "Error", msg.c_str());
        }
    }
    ui->cmdLineEdit->setText("");
}

int MainWindow::isLineNumber(QString cmd)
{
    // 找到第一个空格
    int spaceIndex = cmd.indexOf(" ");
    // 如果没有空格，检查是不是纯数字
    if (spaceIndex == -1) {
        // 如果是纯数字，返回行号
        if (cmd.toInt() < 0 || cmd.toInt() > 1000000)
            throw string("Line number out of range");
        return cmd.toInt()==0 ? -1 : cmd.toInt();
    }
    if (spaceIndex == -1)
        return -1;
    // 如果空格前面不是数字，返回-1
    if (!cmd.left(spaceIndex).toInt()) {
        return -1;
        throw string("Line number out of range or invalid input.");
    }
    // 如果数字超出范围
    if (cmd.left(spaceIndex).toInt() > 1000000 || cmd.left(spaceIndex).toInt() < 0)
        throw string("Line number out of range");
    // 如果合法
    return cmd.left(spaceIndex).toInt();
}

void MainWindow::treeDisplay()
{
    ui->treeDisplay->clear();

    // 展示语法树
    for (int i = 0; i < CodeVector.expressionTree.size(); i++) {
        if (CodeVector.expressionTree[i] == "!BULE!") {
            ui->treeDisplay->setTextColor(QColor(0, 0, 255));
            continue;
        }
        if (CodeVector.expressionTree[i] == "!ERROR!") {
            ui->treeDisplay->setTextColor(QColor(255, 0, 0));
            continue;
        }
        ui->treeDisplay->append(QString::fromStdString(CodeVector.expressionTree[i]));
        ui->treeDisplay->setTextColor(QColor(0, 0, 0));
    }
}

void MainWindow::on_btnRunCode_clicked()
{
    // 如果不是INPUT模式，清空结果区和语法树区

    ui->textBrowser->clear();
    ui->treeDisplay->clear();
    CodeVector.printResult.clear();
    CodeVector.expressionTree.clear();
    nextLine = 0;
    isRunning = true;
    CodeVector.varMap.clear();
    CodeVector.resetCallTimes();
    /*
    isRunning = CodeVector.run(); // 运行程序
    inputMode = CodeVector.isInputMode;
    if (inputMode) {
        // 展示问号提示
        ui->cmdLineEdit->clear();
        ui->cmdLineEdit->setPlaceholderText("?");
        ui->cmdLineEdit->setFocus();
        return;
    }
    // build expression tree
    CodeVector.buildExpressionTree();
    treeDisplay();

    // 运行结果区
    for (int i = 0; i < CodeVector.printResult.size(); i++) {
        ui->textBrowser->append(QString::number(CodeVector.printResult[i]));
    }
    */
    runContinue();
    // 展示语法树
    CodeVector.buildExpressionTree();
    treeDisplay();
    CodeVector.expressionTree.clear();
}

void MainWindow::on_btnClearCode_clicked()
{
    ui->CodeDisplay->clear();
    ui->textBrowser->clear();
    ui->treeDisplay->clear();
    CodeVector.clear();
    isRunning = false;
    ui->cmdLineEdit->clear();
    inputMode = false;
    ui->cmdLineEdit->setPlaceholderText(">");
}

void MainWindow::displayCode()
{
    ui->CodeDisplay->clear();
    // 删除空白行
    for (auto it = CodeVector.codeVector.begin(); it != CodeVector.codeVector.end();) {
        if (it->second.getExpression() == "") {
            it = CodeVector.codeVector.erase(it);
        } else {
            ++it;
        }
    }
    for (auto it = CodeVector.codeVector.begin(); it != CodeVector.codeVector.end(); it++) {
        // ui->CodeDisplay->append(QString::number(it->first) + "  " +
        // QString::fromStdString(it->second.getExpression()));
        //  行号用绿色字体，并且不换行，内容用黑色字体
        ui->CodeDisplay->setTextColor(QColor(46, 139, 87));
        ui->CodeDisplay->insertPlainText(QString::number(it->first) + "  ");
        ui->CodeDisplay->setTextColor(QColor(0, 0, 0));
        ui->CodeDisplay->insertPlainText(QString::fromStdString(it->second.getExpression()) + "\n");
    }
}

void MainWindow::on_btnLoadCode_clicked()
{
    // 打开文件.txt,.bas,.txt
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Basic Files (*.bas *.txt)"));
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Warning"), tr("Cannot open file:\n%1").arg(fileName));
        return;
    }
    // 清除原有代码
    ui->CodeDisplay->clear();
    ui->textBrowser->clear();
    ui->treeDisplay->clear();
    CodeVector.clear();
    isRunning = false;
    ui->cmdLineEdit->clear();
    inputMode = false;
    ui->cmdLineEdit->setPlaceholderText(">");
    // 读取文件
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        int lineNum = -1;
        try {
            lineNum = isLineNumber(line);
        } catch (string msg) {
            QMessageBox::warning(this, "Error", msg.c_str());
            ui->cmdLineEdit->clear();
            return;
        }
        if (lineNum != -1) {
            // 如果是行号加内容
            // 将内容加入CodeVector
            // 如果内容为空，删除该行
            QString lineRemovespace = line;
            lineRemovespace.remove(" ");
            if (lineRemovespace == "") {
                continue;
            }
            // 如果内容为一个数字，删除该行
            if (lineRemovespace.toInt() != 0) {
                CodeVector.codeVector.erase(lineNum);
                continue;
            }
            try {
                CodeVector.addExpression(
                    lineNum, Expression(line.right(line.length() - line.indexOf(" ")).toStdString(), lineNum));
            } catch (string msg) {
                CodeVector.addExpression(
                    lineNum,
                    Expression(ExpType::ERROR, line.right(line.length() - line.indexOf(" ")).toStdString(), lineNum));
            }
        } else {
            // 如果不是行号加内容
            if (line == "HELP") {
                QMessageBox::information(this, "Help", helpText);
                continue;
            } else if (line == "QUIT") {
                exit(0);
            } else if (line == "CLEAR") {
                ui->CodeDisplay->clear();
                ui->textBrowser->clear();
                ui->treeDisplay->clear();
                CodeVector.clear();
                isRunning = false;
                continue;
            } else if (line == "LIST") {
                displayCode();
                continue;
            } else if (line == "RUN") {
                CodeVector.varMap.clear();
                CodeVector.resetCallTimes();
                isRunning = 1;
                runContinue();
                continue;
            }
            Expression exp;
            try {
                exp = Expression(line.toStdString());
            } catch (string msg) {
                QMessageBox::warning(this, "Error", msg.c_str());
                ui->cmdLineEdit->clear();
                continue;
            }
            try {
                if (exp.type == PRINT) {
                    int ans = exp.executeExp();
                    ui->textBrowser->append(QString::number(ans));
                } else if (exp.type == INPUT) {
                    exp.executeExp();
                    inputMode = true;
                    inputVar = exp.root->child->exp;
                    ui->cmdLineEdit->clear();
                    ui->cmdLineEdit->setPlaceholderText("?");
                    ui->cmdLineEdit->setFocus();
                    return;
                } else if (exp.getExpression() == "RUN") {
                    CodeVector.varMap.clear();
                    CodeVector.resetCallTimes();
                    runContinue();
                } else if (exp.getExpression() == "CLEAR") {
                    ui->CodeDisplay->clear();
                    ui->textBrowser->clear();
                    ui->treeDisplay->clear();
                    CodeVector.clear();
                    isRunning = false;
                } else if (exp.getExpression() == "LIST") {
                    displayCode();
                } else if (exp.getExpression() == "HELP") {
                    QMessageBox::information(this, "Help", helpText);
                } else if (exp.getExpression() == "QUIT") {
                    exit(0);
                } else {
                    exp.executeExp();
                }
            } catch (string msg) {
                QMessageBox::warning(this, "Error", msg.c_str());
            }
        }
    }

    // 展示代码
    displayCode();
    file.close();
}

void MainWindow::runContinue()
{
    /*
    CodeVector.run(); // 运行程序
    inputMode = CodeVector.isInputMode;
    if (inputMode) {
        // 展示问号提示
        ui->cmdLineEdit->clear();
        ui->cmdLineEdit->setPlaceholderText("?");
        ui->cmdLineEdit->setFocus();
        return;
    }
    // build expression tree
    CodeVector.buildExpressionTree();
    treeDisplay();

    // 运行结果区
    for (int i = 0; i < CodeVector.printResult.size(); i++) {
        ui->textBrowser->append(QString::number(CodeVector.printResult[i]));
    }
    */
    if (CodeVector.codeVector.empty()) {
        return;
    }
    it = CodeVector.codeVector.begin();

    // 如果runLine不为0，迭代器指向runLine
    if (nextLine != 0) {
        it = CodeVector.codeVector.find(nextLine);
        nextLine = 0;
    }
    while (it != CodeVector.codeVector.end()) {
        ExpType type;
        try {
            type = runSingleLine();
        } catch (string msg) {
            QMessageBox::warning(this, "Error", msg.c_str());
            // 展示语法树
            CodeVector.buildExpressionTree();
            treeDisplay();

            return;
        }
        if (type == END) {
            break;
        }
        if (type == INPUT) {
            ui->cmdLineEdit->setPlaceholderText("?");
            ui->cmdLineEdit->setFocus();
            ui->treeDisplay->clear();
            CodeVector.buildExpressionTree();
            treeDisplay();
            CodeVector.expressionTree.clear();
            return;
        }
        if (type == PRINT) {
            ui->textBrowser->append(QString::number(CodeVector.printResult.back()));
        }
        if (type == ERROR) {
            QMessageBox::warning(this, "Error", "Syntax Error");
            CodeVector.buildExpressionTree();
            treeDisplay();
            CodeVector.varMap.clear();
            isRunning = false;
            return;
        }
    }
    CodeVector.buildExpressionTree();
    treeDisplay();
    CodeVector.expressionTree.clear();
}

ExpType MainWindow::runSingleLine()
{
    if (CodeVector.codeVector.empty()) {
        isRunning = false;
        return END;
    }
    ExpType type = it->second.type;
    // 判断当前行的语句类型
    switch (it->second.type) {
    case INPUT:
        it->second.executeExp();
        inputMode = true;
        inputVar = it->second.root->child->exp;
        it++;
        nextLine = it->first;
        return INPUT;
    case GOTO:
        it = CodeVector.codeVector.find(it->second.executeExp());
        // 如果跳转行号不存在，报错
        if (it == CodeVector.codeVector.end()) {
            throw string("Line " + std::to_string(it->second.getLine()) + ": GOTO line does not exist");
        }
        return GOTO;
    case IF: {
        int nextline = it->second.executeExp();
        if (nextline > 0) {
            auto temp = CodeVector.codeVector.find(nextline);
            // 如果跳转行号不存在，报错
            if (temp == CodeVector.codeVector.end()) {
                throw string("Line " + std::to_string(it->second.getLine()) + ": GOTO line does not exist");
            }
            it = temp;
        } else {
            it++;
        }
        return IF;
    }
    case END:
        // 代码运行次数加一
        it->second.runTimes++;
        isRunning = false;
        return END;
    case PRINT:
        CodeVector.printResult.push_back(it->second.executeExp());
        it++;
        return PRINT;
    default:
        it->second.executeExp();
        it++;
        return type;
    }
    return type;
}