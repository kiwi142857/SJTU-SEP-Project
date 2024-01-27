#pragma once
#include "Expression.h"
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <vector>

using std::cout, std::endl, std::map, std::pair, std::string, std::vector, std::stack;

class CodeVector
{
  public:
    map<int, Expression> codeVector; // map of line number and expression
    map<string, var> varMap;         // map of variable name and value
    vector<string> varList;          // list of variable names
    Expression *init;
    stack<string> errorMessage;
    vector<int> printResult;
    bool isRunning;
    bool isInputMode;
    string inputVar;

  public:
    CodeVector(/* args */);
    ~CodeVector(){};
    void printCodeVector();
    void addExpression(int line, Expression exp);
    void addVariable(string var);
    void addVariable(string var, int value);
    Expression *getExpression(int line)
    {
        return &codeVector[line];
    }
    vector<string> expressionTree;
    bool isVariable(string var)
    {
        return varMap.find(var) != varMap.end();
    }
    bool run(int input = 0);
    // void runSingleLine(int line);
    void clear();
    void clearVar();
    void buildExpressionTree();
    // 在运行开始前把每个语句的计数器清零
    void resetCallTimes();
  private:
    int runLine;
};
