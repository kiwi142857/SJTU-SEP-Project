#include "Expression.h"
#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <assert.h>

void testExpRPN()
{
    try {
        Expression exp1(string("1+2*3"));
        Expression exp2(string("1+2*3+4"));
        Expression exp3(string("1+2*3+4*5"));
        Expression exp4(string("(1+2*3+4*5+6**7)MOD8"));
        Expression exp5(string("1+2*3+4*5+6**7"));
        if (&exp1 == NULL) {
            qDebug() << "Expression is NULL";
        } else {
            // exp1.printRPN();
            assert(exp1.ReversePolishNotation("1+2*3") == "1 2 3 * +");
            // exp2.printRPN();
            assert(exp2.ReversePolishNotation("1+2*3+4") == "1 2 3 * 4 + +");
            // exp3.printRPN();
            assert(exp3.ReversePolishNotation("1+2*3+4*5") == "1 2 3 * 4 5 * + +");
            // exp4.printRPN();
            assert(exp4.ReversePolishNotation("(1+2*3+4*5+6**7)MOD8") == "1 2 3 * 4 5 * 6 7 ** + + + 8 MOD");
            // exp5.printRPN();
            assert(exp5.ReversePolishNotation("1+2*3+4*5+6**7") == "1 2 3 * 4 5 * 6 7 ** + + +");
        }
    } catch (const std::exception &e) {
        qDebug() << "Exception caught: " << e.what();
    }
}

void testExpPN()
{
    try {
        Expression exp1(string("1+2*3"));
        Expression exp2(string("1+2*3+4"));
        Expression exp3(string("1+2*3+4*5"));
        Expression exp4(string("(1+2*3+4*5+6**7)MOD8"));
        Expression exp5(string("1+2*3+4*5+6**7"));
        if (&exp1 == NULL) {
            qDebug() << "Expression is NULL";
        } else {
            // exp1.printPN();
            assert(exp1.PolishNotation("1+2*3") == "+ 1 * 2 3");
            // exp2.printPN();
            assert(exp2.PolishNotation("1+2*3+4") == "+ + 1 * 2 3 4");
            // exp3.printPN();
            assert(exp3.PolishNotation("1+2*3+4*5") == "+ + 1 * 2 3 * 4 5");
            // exp4.printPN();
            assert(exp4.PolishNotation("(1+2*3+4*5+6**7)MOD8") == "MOD + + + 1 * 2 3 * 4 5 ** 6 7 8");
            // exp5.printPN();
            assert(exp5.PolishNotation("1+2*3+4*5+6**7") == "+ + + 1 * 2 3 * 4 5 ** 6 7");
        }
    } catch (const std::exception &e) {
        qDebug() << "Exception caught: " << e.what();
    }
}

void testProcessLet()
{
    string exp1 = "LET a=1+2*3";
    string exp2 = "LET b=1+2*3+4";
    string exp3 = "LET c=1+2*3+4*5";
    string exp4 = "LET d=(1+2*3+4*5+6**7)MOD8";
    string exp5 = "LET e=1+2*3+4*5+6**7";
    string exp6 = "LET f=1+2*3+4*5+6**7+8";
    try {
        Expression exp(exp1);
        // exp.printRPN();
        // exp.printPN();
        exp.processLet();
        // assert(exp.getVar("a") == 7);
        // exp.printRPN();
        // exp.printPN();
        exp = Expression(exp2);
        exp.processLet();
        // assert(exp.getVar("b") == 11);
        // exp.printRPN();
        // exp.printPN();
        exp = Expression(exp3);
        exp.processLet();
        // assert(exp.getVar("c") == 29);
        // exp.printRPN();
        // exp.printPN();
        exp = Expression(exp4);
        exp.processLet();
        // assert(exp.getVar("d") == 7);
        // exp.printRPN();
        // exp.printPN();
        exp = Expression(exp5);
        exp.processLet();
        // assert(exp.getVar("e") == 279);
        // exp.printRPN();
        // exp.printPN();
        exp = Expression(exp6);
        exp.processLet();
        // assert(exp.getVar("f") == 287);
        // exp.printRPN();
        // exp.printPN();
    } catch (const std::exception &e) {
        qDebug() << "Exception caught: " << e.what();
    }
}

void testProcessPrint()
{
    string exp4 = "PRINT (1+2*3+4*5+6**7)MOD8";
    Expression exp(exp4);
    // exp.printRPN();
    // exp.printPN();
}

void testProcessIF()
{
    string exp1 = "IF 1+2*3 > 5 THEN 100";
    string exp2 = "IF (1+2*3+4*5+6**7)MOD8 > a*3 THEN 100";
    Expression exp(exp2);
    // exp.printRPN();
    // exp.printPN();
}

void testPrint()
{
    string exp1 = "REM This is a comment";
    string exp2 = "LET a=(1+2*3)MOD8";
    string exp3 = "PRINT a*b";
    string exp4 = "IF a > 5 THEN 100";
    string exp5 = "GOTO 100";
    string exp6 = "END";
    Expression exp(exp1);
    // exp.printRPN();
    // exp.printPN();
    exp.print();
    exp = Expression(exp2);
    // exp.printRPN();
    // exp.printPN();
    exp.print();
    exp = Expression(exp3);
    // exp.printRPN();
    // exp.printPN();
    exp.print();
    exp = Expression(exp4);
    // exp.printRPN();
    // exp.printPN();
    exp.print();
    exp = Expression(exp5);
    // exp.printRPN();
    // exp.printPN();
    exp.print();
    exp = Expression(exp6);
    // exp.printRPN();
    // exp.printPN();
    exp.print();
}

void testCodeVector()
{
    CodeVector codeVector;
    codeVector.addExpression(1, Expression("REM This is a comment"));
}
/*
int main(int argc, char *argv[])
{
    try {
        QApplication a(argc, argv);
        MainWindow w;
        w.show();
        return a.exec();
    } catch (const std::exception &e) {
        qDebug() << "Exception caught: " << e.what();
    }
    // testCodeVector();
    return 0;
}*/

class MyApplication : public QApplication
{
  public:
    MyApplication(int &argc, char **argv) : QApplication(argc, argv)
    {
    }

    bool notify(QObject *receiver, QEvent *event) override;
};

bool MyApplication::notify(QObject *receiver, QEvent *event)
{
    try {
        // qDebug() << "Sending event type:" << event->type() << "to object of class:" <<
        // receiver->metaObject()->className();
        return QApplication::notify(receiver, event);
    } catch (const std::exception &e) {
        qDebug() << "Exception caught in event type: " << event->type();
        qDebug() << "Exception caught: " << e.what();
    }
    return false;
}

void testSplitExp()
{
    string exp1 = "REM Fibonacci";
    string exp2 = "LET max = 10000";
    string exp3 = "LET n1 = 0";
    string exp4 = "LET n2 = 1";
    string exp5 = "IF n1 > max THEN 190";
    string exp6 = "PRINT n1";
    string exp7 = "LET n3 = n1+n2";
    string exp8 = "LET n1=n2";
    string exp9 = "LET n2=n3";
    string exp10 = "GOTO 140";
    string exp11 = "END";
    // Error sentence
    string exp12 = "LET n1 = 0 1";
    string exp13 = "LET n1 = - 1 ** 2 ** 3 MOD  4";
    string exp14 = "LET n1 = 0 ** 3 ** 4 MOD - 5";

    try {
        Expression exp(exp1);
        exp.splitExp();
        exp = Expression(exp2);
        exp.splitExp();
        exp = Expression(exp3);
        exp.splitExp();
        exp = Expression(exp4);
        exp.splitExp();
        exp = Expression(exp5);
        exp.splitExp();
        exp = Expression(exp6);
        exp.splitExp();
        exp = Expression(exp7);
        exp.splitExp();
        exp = Expression(exp8);
        exp.splitExp();
        exp = Expression(exp9);
        exp.splitExp();
        exp = Expression(exp10);
        exp.splitExp();
        exp = Expression(exp11);
        exp.splitExp();
        exp = Expression(exp12);
        exp.splitExp();
        exp = Expression(exp13);
        exp.splitExp();
        exp = Expression(exp14);
        exp.splitExp();
    }
    catch (const std::string &e) {
        qDebug() << "Exception caught: " << e.c_str();
    }
}

int main(int argc, char *argv[])
{
    // testSplitExp();
    // return 0;
    MyApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}