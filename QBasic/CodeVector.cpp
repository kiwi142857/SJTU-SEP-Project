#include "CodeVector.h"

CodeVector::CodeVector(/* args */)
{
    // 初始化变量
    init = new Expression("REM INIT", &varMap);
    runLine = 0;
    isRunning = false;
    isInputMode = false;
}

void CodeVector::addExpression(int line, Expression exp)
{
    codeVector[line] = exp;
}

void CodeVector::addVariable(string var0)
{
    var v = var(var0, 0);
    varMap.insert(pair<string, var>(var0, (var0,v)));
    varList.push_back(var0);
}

void CodeVector::addVariable(string var0, int value)
{
    var v = var(var0, value);
    varMap.insert(pair<string, var>(var0, (var0,v)));
    varList.push_back(var0);
}

void CodeVector::printCodeVector()
{
    for (auto it = codeVector.begin(); it != codeVector.end(); it++) {
        cout << it->first << ": " << it->second.getExpression() << endl;
    }
}

bool CodeVector::run(int input)
{
    // 执行codeVector

    // 迭代器指向第一行
    // 如果codeVector为空，返回true
    if (codeVector.empty()) {
        return true;
    }
    auto it = codeVector.begin();
    // 如果runLine不为0，迭代器指向runLine
    if (runLine != 0) {
        it = codeVector.find(runLine);
        runLine = 0;
    }
    while (it != codeVector.end()) {
        // 判断当前行的语句类型
        switch (it->second.type) {
        case INPUT:
            it->second.executeExp();
            it++;
            runLine = it->first;
            isInputMode = true;
            inputVar = it->second.root->child->exp;
            return false;
        case GOTO:
            it = codeVector.find(it->second.executeExp());
            break;
        case IF: {
            int nextline = it->second.executeExp();
            if (nextline > 0) {
                it = codeVector.find(nextline);
            } else {
                it++;
            }
            break;
        }
        case END:
        // 代码运行次数加一
        it->second.runTimes++;
        // 如果是END语句，返回true
            return true;
        case PRINT:
            printResult.push_back(it->second.executeExp());
            it++;
            break;
        default:
            it->second.executeExp();
            it++;
            break;
        }
    }
}

void CodeVector::buildExpressionTree()
{
    expressionTree.clear();
    // 从codeVector中提取表达式
    for (auto it = codeVector.begin(); it != codeVector.end(); it++) {
        // 生成表达式树
        it->second.storeExpressionTree();
        // 第一句加入颜色绿色标志
        expressionTree.push_back("!BULE!");
        for (int i = 0; i < it->second.expressionTree.size(); i++) {
            // 将表达式加入expressionTree
            string temp = it->second.expressionTree[i];
            expressionTree.push_back(it->second.expressionTree[i]);
        }
    }
}

void CodeVector::clear()
{
    codeVector.clear();
    varMap.clear();
    varList.clear();
    printResult.clear();
    expressionTree.clear();
    runLine = 0;
}

void CodeVector::clearVar()
{
    varMap.clear();
    varList.clear();
    printResult.clear();
    expressionTree.clear();
    runLine = 0;
}

void CodeVector::resetCallTimes()
{
    for (auto it = codeVector.begin(); it != codeVector.end(); it++) {
        it->second.runTimes = 0;
        it->second.IfTimes = 0;
    }
}