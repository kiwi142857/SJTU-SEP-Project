#pragma once

#include "ExpNode.h"
#include <QStack>
#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

using std::cout, std::endl, std::string, std::vector, std::map, std::stack, std::stringstream, std::cin;

class var
{
  public:
    string name;

    var(string name, int value)
    {
        this->name = name;
        this->value = value;
        useCount = 0;
    }
    var()
    {
        name = "";
        value = 0;
        useCount = 0;
    }
    int &getValue()
    {
        useCount++;
        return value;
    }
    int getCallTimes()
    {
        return useCount;
    }
    int value;
  private:
    
    int useCount;
};

enum ExpType {
    REM,
    LET,
    PRINT,
    INPUT,
    GOTO,
    IF,
    END,
    // 特殊类型
    RUN,
    LOAD,
    LIST,
    CLEAR,
    HELP,
    QUIT,
    // ERROR
    ERROR,
    EMPTY
};

enum opType {
    op_add,
    op_sub,
    op_mul,
    op_div,
    op_mod,
    op_pow,
    op_minus,
    op_lbracket,
    op_rbracket,
    op_greater,
    op_equal,
    op_less
};

class Expression
{
    // 检测用
    friend void testExpRPN();
    friend void testExpPN();
    friend void testProcessLet();
    friend void testProcessPrint();
    // friend void testProcessInput();
    // friend void testProcessGoto();
    friend void testProcessIf();
    // friend void testProcessEnd();
    friend void testPrint();
    friend void testSplitExp();

  private:
    // 行号
    int line;
    //  表达式字符串
    string exp;
    // 各部分分句
    vector<string> expParts;
    // 各部分类型
    vector<ExpPartType> expPartTypes;
    string part[4];
    // 共享变量数值地址
    static map<string, var> *varMap;

    void setType();
    inline void addOpToRPN(string &rpn, opType op);
    inline void addOpToPN(string &pn, opType op);
    string PolishNotation(string exp);
    void PolishNotation(int begin, int end);
    string ReversePolishNotation(string exp);

    string ExpPartTypeToString(ExpPartType type);
    // 对表达式各个元素直接加空格,将表达式转化为波兰表达式并将各个部分存在vector中
    void splitExp();
    // 区分表达式类型
    void processExp();
    // 处理不同类型的表达式
    void processRem();
    void processLet();
    void processPrint();
    void processInput();
    void processGoto();
    void processIf();
    void processEnd();
    // 读取前缀表达式第一个操作数或操作符
    inline void readFirst(string exp, ExpNodeType &type, string &first, int &pos);
    void createExpTree(string &exp, ExpNode *root);
    void createExpTree(int begin, int end, ExpNode *root);
    void printExpTree(ExpNode *root, int width = 0);
    // 分类型打印
    void printExp();
    void printRem();
    void printLet();
    void printPrint();
    void printInput();
    void printGoto();
    void printIf();
    void printEnd();
    ExpNode *copyExpTree(ExpNode *root);
    // 递归计算表达式
    int calculateExpTree(ExpNode *root);
    // 递归计算表达式树
    // 分类型处理

    int executeRem();
    int executeLet();
    int executePrint();
    int executeInput();
    int executeGoto();
    int executeIf();
    int executeEnd();

    void storeExpressionTree(ExpNode *root);
    void storeExpTree(ExpNode *root, int width = 0);
    void storeExp();
    void storeRem();
    void storeLet();
    void storePrint();
    void storeInput();
    void storeGoto();
    void storeIf();
    void storeEnd();

    // 检查数的各个叶节点是否为变量或常数
    bool checkExpTree(ExpNode *root);
    // 检查IF语句的表达式是否合法
    bool checkIfExpTree(ExpNode *root);

  public:
    ExpType type;
    ExpNode *root;
    int runTimes;
    int IfTimes;
    vector<string> expressionTree;

    Expression(/* args */);
    Expression(string exp, int line = 0);
    Expression(const Expression &exp);
    Expression(ExpType type, string exp, int line = 0);
    Expression(string exp, map<string, var> *varMap);
    void setExp(string exp);
    void print();
    int executeExp();
    string getExpression();
    void storeExpressionTree();
    int calculateExp();
    ~Expression();
    int getLine(){return line;}
};
