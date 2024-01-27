#include "Expression.h"
#include <cmath>
map<string, var> *Expression::varMap = nullptr;

string Expression::getExpression()
{
    return exp;
}

void Expression::setType()
{
    // 删去前置空格
    while (exp[0] == ' ') {
        exp = exp.substr(1);
    }
    // 删除后置空格
    while (exp[exp.length() - 1] == ' ') {
        exp = exp.substr(0, exp.length() - 1);
    }
    // 判断类型
    if (exp == "RUN") {
        type = RUN;
        return;
    } else if (exp == "LOAD") {
        type = LOAD;
        return;
    } else if (exp == "LIST") {
        type = LIST;
        return;
    } else if (exp == "CLEAR") {
        type = CLEAR;
        return;
    } else if (exp == "HELP") {
        type = HELP;
        return;
    } else if (exp == "QUIT") {
        type = QUIT;
        return;
    } else if (exp.find("REM") == 0) {
        type = REM;
    } else if (exp.find("LET") == 0) {
        type = LET;
    } else if (exp.find("PRINT") == 0) {
        type = PRINT;
    } else if (exp.find("INPUT") == 0) {
        type = INPUT;
    } else if (exp.find("GOTO") == 0) {
        type = GOTO;
    } else if (exp.find("IF") == 0) {
        type = IF;
    } else if (exp.find("END") == 0) {
        type = END;
    } else {
        type = ERROR;
    }
}

Expression::Expression(string exp, map<string, var> *varMap) : runTimes(0), IfTimes(0)
{
    this->exp = exp;
    // 删除exp前的空格
    while (this->exp[0] == ' ') {
        this->exp = this->exp.substr(1);
    }
    this->varMap = varMap;
    // 判断类型
    setType();
    // 如果是特殊类型
    if (type == RUN || type == LOAD || type == LIST || type == CLEAR || type == HELP || type == QUIT) {
        return;
    }
    // 如果不是特殊类型，则分类型处理
    processExp();
    storeExpressionTree(root);
    // checkExpTree(root);
}

Expression::Expression(/* args */) : runTimes(0), IfTimes(0)
{
}

void Expression::setExp(string exp)
{
    this->exp = exp;
    // 删除exp前的空格
    while (this->exp[0] == ' ') {
        this->exp = this->exp.substr(1);
    }
    // 在exp各个部分之间加入空格
    // 判断类型
    // 如果是特殊类型
    splitExp();
    // 如果不是特殊类型，则分类型处理
    if (type == RUN || type == LOAD || type == LIST || type == CLEAR || type == HELP || type == QUIT) {
        return;
    }
    processExp();
}

Expression::Expression(const Expression &exp) : runTimes(exp.runTimes), IfTimes(exp.IfTimes)
{
    this->exp = exp.exp;
    this->type = exp.type;
    this->root = copyExpTree(exp.root);
}

Expression::Expression(string exp, int line) : runTimes(0), IfTimes(0), line(line)
{
    this->exp = exp;
    // 删除exp前的空格
    while (this->exp[0] == ' ') {
        this->exp = this->exp.substr(1);
    }
    // 在exp各个部分之间加入空格
    // 判断类型
    // 如果是特殊类型
    splitExp();
    // 如果不是特殊类型，则分类型处理
    if (type == RUN || type == LOAD || type == LIST || type == CLEAR || type == HELP || type == QUIT) {
        return;
    }
    processExp();
    // checkExpTree(root);
}

Expression::Expression(ExpType type, string exp, int line) : runTimes(0), IfTimes(0), line(line)
{
    this->exp = exp;
    this->type = type;
}

int Expression::calculateExp()
{
    return calculateExpTree(root);
}

int Expression::calculateExpTree(ExpNode *root)
{
    if (!root)
        return 0;
    if (root->type == exp_var) {
        // 检查变量是否存在
        if (varMap->find(root->exp) == varMap->end()) {
            throw string("Variable not found");
        }
        return (*varMap)[root->exp].getValue();
    } else if (root->type == exp_num) {
        return stoi(root->exp);
    } else if (root->type != exp_neg) {
        int left = calculateExpTree(root->child);
        int right = 0;
        if (root->child && root->child->next) {
            right = calculateExpTree(root->child->next);
        }
        if (root->exp == "+") {
            return left + right;
        } else if (root->exp == "-") {
            return left - right;
        } else if (root->exp == "*") {
            return left * right;
        } else if (root->exp == "/") {
            if (right == 0)
                throw string("Divided by zero");
            return left / right;
        } else if (root->exp == "MOD") {
            int ans = left % right;
            if (right == 0)
                throw string("Moded by zero");
            // ans 正负情况与right相同
            if (right < 0) {
                ans += right;
            }
            if(ans==0) return ans;
            // 如果结果绝对值大于right，则加上right
            if((abs(ans) >= abs(right))&&right<0 && ans < 0){
                ans -= right;
            }
            if((abs(ans) >= abs(right))&&right>0){
                ans += right;
            }
            if(right > 0 && ans < 0) {
                ans += right;
            }
            return ans;
        } else if (root->exp == "**") {
            int ans = 1;
            ans = pow(left, right);
            return ans;
        } else if (root->exp == ">") {
            return left > right;
        } else if (root->exp == "=") {
            return left == right;
        } else if (root->exp == "<") {
            return left < right;
        }
    }
    return -calculateExpTree(root->child);
}

void Expression::addOpToRPN(string &rpn, opType op)
{
    // 加入空格分隔
    if (rpn.length() > 0) {
        rpn += ' ';
    }
    switch (op) {
    case op_add:
        rpn += '+';
        break;
    case op_sub:
        rpn += '-';
        break;
    case op_mul:
        rpn += '*';
        break;
    case op_div:
        rpn += '/';
        break;
    case op_mod:
        rpn += "MOD";
        break;
    case op_pow:
        rpn += "**";
        break;
    case op_lbracket:
        rpn += '(';
        break;
    case op_rbracket:
        rpn += ')';
        break;
    default:
        break;
    }
}

void Expression::addOpToPN(string &pn, opType op)
{
    // 加入空格分隔
    if (pn.length() > 0) {
        pn += ' ';
    }
    switch (op) {
    case op_add:
        pn += '+';
        break;
    case op_sub:
        pn += '-';
        break;
    case op_mul:
        pn += '*';
        break;
    case op_div:
        pn += '/';
        break;
    case op_mod:
        pn += "DOM";
        break;
    case op_pow:
        pn += "**";
        break;
    case op_lbracket:
        pn += '(';
        break;
    case op_rbracket:
        pn += ')';
        break;
    default:
        break;
    }
}

void Expression::processExp()
{
    switch (type) {
    case REM:
        processRem();
        break;
    case LET:
        processLet();
        break;
    case PRINT:
        processPrint();
        break;
    case INPUT:
        processInput();
        break;
    case GOTO:
        processGoto();
        break;
    case IF:
        processIf();
        break;
    case END:
        processEnd();
        break;
    default:
        break;
    }
}

void Expression::processRem()
{
    // REM
    root = new ExpNode(exp_ins, "REM", NULL);
    root->child = new ExpNode(exp_ins, exp.substr(3), root);
}

void Expression::processLet()
{
    // LET
    // 寻找expParts中等号的位置
    int pos = -1;
    for (int i = 0; i < expParts.size(); i++) {
        if (expPartTypes[i] == exp_equal_part) {
            pos = i;
            break;
        }
    }
    // 如果没有等号，则用throw抛出异常
    if (pos == -1) {
        this->type = ERROR;
        throw string("No '=' found in LET expression");
    }
    /*
    // 如果有等号，则将等号前后的表达式分别存入part[0]和part[1]
    part[0] = exp.substr(4, pos - 4);
    part[1] = exp.substr(pos + 1);
    // 将表达式转换为波兰表达式
    part[1] = PolishNotation(part[1]);
    // 生成表达式树
    root = new ExpNode(exp_op, "=", NULL);
    root->child = new ExpNode(exp_op, part[0], root);
    createExpTree(part[1], root);
    */

    // 检查变量名是否合法
    if (expParts[1] == "MOD" || expParts[1] == "IF" || expParts[1] == "THEN" || expParts[1] == "GOTO" ||
        expParts[1] == "END" || expParts[1] == "LET" || expParts[1] == "PRINT" || expParts[1] == "INPUT" ||
        expParts[1] == "REM" || expParts[1] == "RUN" || expParts[1] == "LIST" || expParts[1] == "CLEAR" ||
        expParts[1] == "HELP" || expParts[1] == "QUIT") {
        this->type = ERROR;
        throw string("Invalid variable name");
    }
    // 如果变量名是纯数字
    bool isNum = true;
    for (int i = 0; i < expParts[1].length(); i++) {
        if (expParts[1][i] < '0' || expParts[1][i] > '9') {
            isNum = false;
            break;
        }
    }
    if (isNum) {
        this->type = ERROR;
        throw string("Invalid variable name");
    }
    // 如果expPartTypes[1]不是exp_var_part
    if (expPartTypes[1] != exp_var_part) {
        this->type = ERROR;
        throw string("Invalid variable name");
    }
    // 如果有等号，分别处理等号前后语句
    PolishNotation(pos + 1, expParts.size());
    root = new ExpNode(exp_op, "=", NULL);
    root->child = new ExpNode(exp_var, expParts[1], root);
    createExpTree(pos + 1, expParts.size(), root);
    checkExpTree(root);
}

void Expression::processPrint()
{
    /*
    // PRINT
    // 将PRINT后的表达式存入part[0]
    part[0] = exp.substr(5);
    // 将表达式转换为波兰表达式
    part[0] = PolishNotation(part[0]);
    root = new ExpNode(exp_op, "PRINT", NULL);
    createExpTree(part[0], root);
    */

    PolishNotation(1, expParts.size());
    root = new ExpNode(exp_ins, "PRINT", NULL);
    createExpTree(1, expParts.size(), root);
    checkExpTree(root->child);
}

void Expression::processInput()
{
    // INPUT
    // 将INPUT后的表达式存入part[0]
    /*
    part[0] = exp.substr(5);
    root = new ExpNode(exp_op, "INPUT", NULL);
    root->child = new ExpNode(exp_var, part[0], root);
    */

    root = new ExpNode(exp_op, "INPUT", NULL);
    // 判断是否只有一个变量
    if (expParts.size() != 2) {
        this->type = ERROR;
        throw string("More than one variable found in INPUT expression");
    }
    // 检查是否是合法变量名
    if (expParts[1][0] >= 'A' && expParts[1][0] <= 'Z' || expParts[1][0] >= 'a' && expParts[1][0] <= 'z' ||
        expParts[1][0] == '_') {
        // 判断是否是保留关键字MOD
        if (expParts[1] == "MOD" || expParts[1] == "IF" || expParts[1] == "THEN" || expParts[1] == "GOTO" ||
            expParts[1] == "END" || expParts[1] == "LET" || expParts[1] == "PRINT" || expParts[1] == "INPUT" ||
            expParts[1] == "REM" || expParts[1] == "RUN" || expParts[1] == "LIST" || expParts[1] == "CLEAR" ||
            expParts[1] == "HELP" || expParts[1] == "QUIT") {
            this->type = ERROR;
            throw string("Invalid variable name");
        }
        // 如果变量名是纯数字
        bool isNum = true;
        for (int i = 0; i < expParts[1].length(); i++) {
            if (expParts[1][i] < '0' || expParts[1][i] > '9') {
                isNum = false;
                break;
            }
        }
        if (isNum) {
            this->type = ERROR;
            throw string("Invalid variable name");
        }
        // 如果变量名含有非法字符
        for (int i = 0; i < expParts[1].length(); i++) {
            if (!(expParts[1][i] >= 'A' && expParts[1][i] <= 'Z' || expParts[1][i] >= 'a' && expParts[1][i] <= 'z' ||
                  expParts[1][i] >= '0' && expParts[1][i] <= '9' || expParts[1][i] == '_')) {
                this->type = ERROR;
                throw string("Invalid variable name");
            }
        }
        root->child = new ExpNode(exp_var, expParts[1], root);
    } else {
        this->type = ERROR;
        throw string("Invalid variable name");
    }
}

void Expression::processGoto()
{
    // GOTO
    // 将GOTO后的表达式存入part[0]
    // part[0] = exp.substr(4);
    // 检查是否只有一个常数
    if (expParts.size() != 2) {
        this->type = ERROR;
        throw string("More than one number found in GOTO expression");
    }
    // 检查是否是合法常数
    for (int i = 0; i < expParts[1].length(); i++) {
        if (expParts[1][i] < '0' || expParts[1][i] > '9') {
            this->type = ERROR;
            throw string("Invalid number");
        }
    }
    root = new ExpNode(exp_ins, "GOTO", NULL);
    root->child = new ExpNode(exp_num, expParts[1], root);
}

void Expression::processIf()
{
    // IF
    // 寻找THEN
    int pos = -1;
    for (int i = 0; i < expParts.size(); i++) {
        if (expPartTypes[i] == exp_ins_part && expParts[i] == "THEN") {
            pos = i;
            break;
        }
    }
    // 如果没有THEN，则用throw抛出异常
    if (pos == -1) {
        this->type = ERROR;
        throw string("No 'THEN' found in IF expression");
    }
    /*
    // 如果有THEN，则将寻找> = < ,将IF后，> = <前的表达式存入part[0]，
    // 将> = <表达式存入part[1]，将> = <后的，
    // THEN前的表达式存入part[2]，将THEN后的表达式存入part[3]
    int pos1 = exp.find(">");
    int pos2 = exp.find("=");
    int pos3 = exp.find("<");
    if (pos1 != string::npos) {
        part[0] = exp.substr(3, pos1 - 3);
        part[1] = exp[pos1];
        part[2] = exp.substr(pos1 + 1, pos - pos1 - 1);
    } else if (pos2 != string::npos) {
        part[0] = exp.substr(3, pos2 - 3);
        part[1] = exp[pos2];
        part[2] = exp.substr(pos2 + 1, pos - pos2 - 1);
    } else if (pos3 != string::npos) {
        part[0] = exp.substr(3, pos3 - 3);
        part[1] = exp[pos3];
        part[2] = exp.substr(pos3 + 1, pos - pos3 - 1);
    } else {
        throw "No '>' or '=' or '<' found in IF expression";
    }

    part[3] = exp.substr(pos + 4);
    // 将表达式转换为波兰表达式
    part[0] = PolishNotation(part[0]);
    part[2] = PolishNotation(part[2]);

    // part[1]删除前面的空格
    while (part[1][0] == ' ') {
        part[1] = part[1].substr(1);
    }
    // part[1]删除后面的空格
    while (part[1][part[1].length() - 1] == ' ') {
        part[1] = part[1].substr(0, part[1].length() - 1);
    }
    // part[3]删除前面的空格
    while (part[3][0] == ' ') {
        part[3] = part[3].substr(1);
    }
    // part[3]删除后面的空格
    while (part[3][part[3].length() - 1] == ' ') {
        part[3] = part[3].substr(0, part[3].length() - 1);
    }
    root = new ExpNode(exp_op, "IF", NULL);
    createExpTree(part[0], root);
    createExpTree(part[2], root);
    root->child->next->next = new ExpNode(exp_op, part[1], root);
    root->child->next->next->next = new ExpNode(exp_num, part[3], root);
    // 调整child顺序
    ExpNode *node1 = root->child->next->next;
    ExpNode *node2 = root->child->next;
    node2->next = node1->next;
    node1->next = node2;
    root->child->next = node1;
    */
    int pos1 = -1;
    int pos2 = -1;
    int pos3 = -1;
    for (int i = 0; i < expParts.size(); i++) {
        if (expPartTypes[i] == exp_greater_part) {
            pos1 = i;
        } else if (expPartTypes[i] == exp_equal_part) {
            pos2 = i;
        } else if (expPartTypes[i] == exp_less_part) {
            pos3 = i;
        }
    }
    int sign = 0;
    // 判断是否只有一个符号
    if (pos1 != -1 && pos2 == -1 && pos3 == -1) {
        sign = pos1;
    } else if (pos1 == -1 && pos2 != -1 && pos3 == -1) {
        sign = pos2;
    } else if (pos1 == -1 && pos2 == -1 && pos3 != -1) {
        sign = pos3;
    } else {
        this->type = ERROR;
        throw string("No '>' or '=' or '<' found in IF expression Or more than one sign found");
    }
    // 判断最后一个THEN后是否是常数
    if (expPartTypes[expPartTypes.size() - 1] != exp_num_part) {
        this->type = ERROR;
        throw string("Invalid number after 'THEN' in IF expression");
    }
    // 将IF后，> = <前的表达式逆转为波兰表达式
    PolishNotation(1, sign);
    // 将> = <后的，THEN前的表达式逆转为波兰表达式
    PolishNotation(sign + 1, pos);
    root = new ExpNode(exp_op, "IF", NULL);
    createExpTree(1, sign, root);
    createExpTree(sign + 1, pos, root);
    root->child->next->next = new ExpNode(exp_ins, expParts[sign], root);
    root->child->next->next->next = new ExpNode(exp_num, expParts[expParts.size() - 1], root);
    // 调整child顺序
    ExpNode *node1 = root->child->next->next;
    ExpNode *node2 = root->child->next;
    node2->next = node1->next;
    node1->next = node2;
    root->child->next = node1;
    checkExpTree(root);
}

void Expression::processEnd()
{
    // END
    // 什么都不做
    // 判断END后是否还有内容？
    if (exp.length() > 3) {
        this->type = ERROR;
        throw string("Invalid END expression");
    }
    if (expParts.size() > 1) {
        this->type = ERROR;
        throw string("Invalid END expression");
    }
    root = new ExpNode(exp_ins, "END", NULL);
}

void Expression::readFirst(string exp, ExpNodeType &type, string &first, int &pos)
{
    // 删去前置空格
    while (exp[pos] == ' ') {
        pos++;
    }
    // 判断是否是操作符
    if (exp[pos] == '*' && exp[pos + 1] == '*') {
        type = exp_op;
        first = "**";
        pos += 2;
        return;
    } else if (exp[pos] == 'M' && exp[pos + 1] == 'O' && exp[pos + 2] == 'D') {
        type = exp_op;
        first = "MOD";
        pos += 3;
        return;
    } else if (exp[pos] == '+' || exp[pos] == '-' || exp[pos] == '*' || exp[pos] == '/') {
        type = exp_op;
        first = exp[pos];
        pos += 1;
        return;
    }
    // 如果是常数或变量
    else {
        // 读入常数或变量
        while (exp[pos] != ' ' && exp[pos] != '+' && exp[pos] != '-' && exp[pos] != '*' && exp[pos] != '/' &&
               exp[pos] != '(' && exp[pos] != ')' && !(exp[pos] == 'M' && exp[pos + 1] == 'O' && exp[pos + 2] == 'D') &&
               exp[pos] != '\0') {
            first += exp[pos];
            pos += 1;
        }
        // 判断是否是变量
        if (first[0] >= 'A' && first[0] <= 'Z' || first[0] >= 'a' && first[0] <= 'z') {
            type = exp_var;
        }
        // 如果不是变量，则是常数
        else {
            type = exp_num;
        }
        return;
    }
}

void Expression::createExpTree(string &exp, ExpNode *root)
{
    ExpNode *node = root;
    for (int i = 0; i < exp.length();) {
        string first;
        ExpNodeType type;
        readFirst(exp, type, first, i);
        ExpNode *nodeNext = new ExpNode(type, first, node);
        if (nodeNext->type == exp_op) {
            if (node->child == NULL)
                node->child = nodeNext;
            else
                node->child->next = nodeNext;

            node = nodeNext;
        } else {
            if (node->child == NULL)
                node->child = nodeNext;
            else
                node->child->next = nodeNext;

            while (node->child != NULL && node->child->next != NULL) {
                if (node->parent != NULL)
                    node = node->parent;
                else
                    break;
            }
        }
    }
}

void Expression::createExpTree(int begin, int end, ExpNode *root)
{
    ExpNode *node = root;
    // 如果partType是exp_add_part或exp_sub_part等等，type为exp_op
    // 如果partType是exp_var_part或exp_num_part，type为exp_var或exp_num
    // 如果partType是exp_neg_part，type为exp_neg
    for (int i = begin; i < end; ++i) {
        if (expPartTypes[i] == exp_empty_part)
            continue;
        if (node->parent == NULL && node->child != NULL && node->child->next != NULL)
            throw string("Invalid expression");
        if (node->exp == "PRINT" && node->child != NULL)
            throw string("Invalid expression");
        string first = expParts[i];
        ExpPartType partType = expPartTypes[i];
        ExpNodeType type;
        if (partType == exp_neg_part) {
            type = exp_neg;
        } else if (partType == exp_var_part) {
            type = exp_var;
        } else if (partType == exp_num_part) {
            type = exp_num;
        } else {
            type = exp_op;
        }
        ExpNode *nodeNext = new ExpNode(type, first, node);
        if (nodeNext->type == exp_op) {
            if (node->child == NULL)
                node->child = nodeNext;
            else
                node->child->next = nodeNext;

            node = nodeNext;
        } else if (type == exp_num || type == exp_var) {
            if (node->child == NULL)
                node->child = nodeNext;
            else
                node->child->next = nodeNext;

            while ((node->type == exp_op && node->child != NULL && node->child->next != NULL) ||
                   (node->type == exp_neg && node->child != NULL)) {
                if (node->parent != NULL)
                    node = node->parent;
                else
                    break;
            }
        } else {
            if (node->child == NULL)
                node->child = nodeNext;
            else
                node->child->next = nodeNext;

            node = nodeNext;
        }
    }
}

Expression::~Expression()
{
}

string Expression::PolishNotation(string exp)
{
    // 运算符包括 + - * / ** ( ) mod
    // 优先级从高到低为 ( ) ** * / mod + -
    // 从左到右扫描中缀表达式
    // 遇到操作数时，将其放入前缀表达式中
    // 遇到运算符时，比较其与栈顶运算符的优先级
    // 如果该运算符优先级大于栈顶运算符优先级，则将该运算符入栈
    // 否则，将栈顶运算符弹出并放入后缀表达式中，再次转到与新栈顶运算符相比较
    // 重复上述操作直到扫描完整个中缀表达式
    // 若遇到括号，则：
    // 如果是左括号，则直接压入栈
    // 如果是右括号，则依次弹出栈顶运算符并放入前缀表达式中，直到遇到左括号，此时将左括号弹出栈，但不放入后缀表达式中
    // 输入中，MOD 为取模运算符，** 为幂运算符
    stack<opType> opStack;
    string pn;

    for (int i = exp.length() - 1; i >= 0; i--) {
        if (exp[i] == ' ') {
            continue;
        }
        if (exp[i] == '+' || exp[i] == '-') {
            while (!opStack.empty() && opStack.top() != op_add && opStack.top() != op_sub &&
                   opStack.top() != op_rbracket) {
                addOpToPN(pn, opStack.top());
                opStack.pop();
            }
            opStack.push(exp[i] == '+' ? op_add : op_sub);
        } else if (exp[i] == '*' && exp[i - 1] == '*') {
            while (!opStack.empty() && opStack.top() != op_add && opStack.top() != op_sub && opStack.top() != op_mul &&
                   opStack.top() != op_div && opStack.top() != op_mod && opStack.top() != op_pow &&
                   opStack.top() != op_rbracket) {
                pn += ' ';
                addOpToPN(pn, opStack.top());
                opStack.pop();
            }
            opStack.push(op_pow);
            i -= 1;
        } else if (exp[i] == '*' || exp[i] == '/') {
            while (!opStack.empty() && opStack.top() != op_add && opStack.top() != op_sub && opStack.top() != op_mul &&
                   opStack.top() != op_div && opStack.top() != op_mod && opStack.top() != op_rbracket) {
                pn += ' ';
                addOpToPN(pn, opStack.top());
                opStack.pop();
            }
            opStack.push(exp[i] == '*' ? op_mul : op_div);
        } else if (exp[i] == 'D' && exp[i - 1] == 'O' && exp[i - 2] == 'M') {
            while (!opStack.empty() && opStack.top() != op_add && opStack.top() != op_sub && opStack.top() != op_mul &&
                   opStack.top() != op_div && opStack.top() != op_mod && opStack.top() != op_rbracket) {
                pn += ' ';
                addOpToPN(pn, opStack.top());
                opStack.pop();
            }
            opStack.push(op_mod);
            i -= 2;
        } else if (exp[i] == ')') {
            opStack.push(op_rbracket);
        } else if (exp[i] == '(') {
            while (opStack.top() != op_rbracket) {
                pn += ' ';
                addOpToPN(pn, opStack.top());
                opStack.pop();
            }
            opStack.pop();
        } else {
            // 用空格分隔变量和常数
            if (pn.length() > 0) {
                pn += ' ';
            }
            // 读入变量和常数
            while (exp[i] != ' ' && exp[i] != '+' && exp[i] != '-' && exp[i] != '*' && exp[i] != '/' && exp[i] != '(' &&
                   exp[i] != ')' && !(exp[i] == 'D' && exp[i - 1] == 'O' && exp[i - 2] == 'M') && exp[i] != '\0') {
                pn += exp[i];
                i -= 1;
            }
            i += 1;
        }
    }
    while (!opStack.empty()) {
        pn += ' ';
        // 使用+ - * / ** ( ) mod表示运算符
        addOpToPN(pn, opStack.top());
        opStack.pop();
    }
    // 删除多余空格
    while (pn[0] == ' ') {
        pn = pn.substr(1);
    }
    // 删除中间多余空格
    for (int i = 1; i < pn.length(); i++) {
        if (pn[i] == ' ' && pn[i - 1] == ' ') {
            pn = pn.substr(0, i) + pn.substr(i + 1);
            i -= 1;
        }
    }
    // 反转
    reverse(pn.begin(), pn.end());
    return pn;
}

void Expression::PolishNotation(int begin, int end)
{
    // 从vector中的begin到end-1的元素转换为波兰表达式
    // 运算符包括 + - * / ** ( ) MOD
    // 优先级从高到低为 ( ) ** * / MOD + -
    // 从左到右扫描中缀表达式
    // 遇到操作数时，将其放入前缀表达式中
    // 遇到运算符时，比较其与栈顶运算符的优先级
    // 如果该运算符优先级大于栈顶运算符优先级，则将该运算符入栈
    // 否则，将栈顶运算符弹出并放入前缀表达式中，再次转到与新栈顶运算符相比较
    // 重复上述操作直到扫描完整个中缀表达式
    // 若遇到括号，则：
    // 如果是左括号，则直接压入栈
    // 如果是右括号，则依次弹出栈顶运算符并放入前缀表达式中，直到遇到左括号，此时将左括号弹出栈，但不放入后缀表达式中
    // 输入中，MOD 为取模运算符，** 为幂运算符
    vector<ExpPartType> expPartTypes;
    vector<string> expParts;
    int numOfBracket = 0;
    for (int i = begin; i < end; i++) {
        expPartTypes.push_back(this->expPartTypes[i]);
    }
    for (int i = begin; i < end; i++) {
        expParts.push_back(this->expParts[i]);
    }
    reverse(expParts.begin(), expParts.end());
    reverse(expPartTypes.begin(), expPartTypes.end());
    stack<ExpPartType> opStack;
    vector<string> pn;
    vector<ExpPartType> pnTypes;
    for (int i = 0; i < expParts.size(); i++) {
        if (expPartTypes[i] == exp_empty_part) {
            continue;
        }
        // 如果是操作数
        if (expPartTypes[i] == exp_var_part || expPartTypes[i] == exp_num_part) {
            pn.push_back(expParts[i]);
            pnTypes.push_back(expPartTypes[i]);
        }
        // 如果是**
        else if (expPartTypes[i] == exp_pow_part) {
            while (!opStack.empty() && (opStack.top() == exp_pow_part || opStack.top() == exp_neg_part)) {
                pn.push_back(ExpPartTypeToString(opStack.top()));
                pnTypes.push_back(opStack.top());
                opStack.pop();
            }
            opStack.push(expPartTypes[i]);
        }
        // 如果是* / MOD
        else if (expPartTypes[i] == exp_mul_part || expPartTypes[i] == exp_div_part ||
                 expPartTypes[i] == exp_mod_part) {
            while (!opStack.empty() && opStack.top() != exp_add_part && opStack.top() != exp_sub_part &&
                   opStack.top() != exp_rbracket_part && opStack.top() != exp_div_part &&
                   opStack.top() != exp_mod_part && opStack.top() != exp_mul_part) {
                pn.push_back(ExpPartTypeToString(opStack.top()));
                pnTypes.push_back(opStack.top());
                opStack.pop();
            }
            opStack.push(expPartTypes[i]);
        }
        // 如果是 + -
        else if (expPartTypes[i] == exp_add_part || expPartTypes[i] == exp_sub_part) {
            while (!opStack.empty() && opStack.top() != exp_rbracket_part && opStack.top() != exp_add_part &&
                   opStack.top() != exp_sub_part) {
                pn.push_back(ExpPartTypeToString(opStack.top()));
                pnTypes.push_back(opStack.top());
                opStack.pop();
            }
            opStack.push(expPartTypes[i]);
        }
        // 如果是 ）
        else if (expPartTypes[i] == exp_rbracket_part) {
            opStack.push(expPartTypes[i]);
            numOfBracket += 1;
        }
        // 如果是 （
        else if (expPartTypes[i] == exp_lbracket_part) {
            while (opStack.top() != exp_rbracket_part) {
                pn.push_back(ExpPartTypeToString(opStack.top()));
                pnTypes.push_back(opStack.top());
                opStack.pop();
            }
            opStack.pop();
        }
        // 如果是 -
        else if (expPartTypes[i] == exp_neg_part) {
            while (!opStack.empty() && opStack.top() != exp_rbracket_part && opStack.top() != exp_add_part &&
                   opStack.top() != exp_sub_part) {
                pn.push_back(ExpPartTypeToString(opStack.top()));
                pnTypes.push_back(opStack.top());
                opStack.pop();
            }
            /* while (!opStack.empty() && opStack.top() != exp_rbracket_part) {
                pn.push_back(ExpPartTypeToString(opStack.top()));
                pnTypes.push_back(opStack.top());
                opStack.pop();
            } */
            opStack.push(expPartTypes[i]);
        }
    }
    while (!opStack.empty()) {
        pn.push_back(ExpPartTypeToString(opStack.top()));
        pnTypes.push_back(opStack.top());
        opStack.pop();
    }
    // 反转
    reverse(pn.begin(), pn.end());
    reverse(pnTypes.begin(), pnTypes.end());
    numOfBracket *= 2;
    try {
        // 存入expParts和expPartTypes
        for (int i = begin; i < end - numOfBracket; i++) {
            this->expParts[i] = pn[i - begin];
            this->expPartTypes[i] = pnTypes[i - begin];
        }
        for (int i = end - numOfBracket; i < end; i++) {
            this->expParts.erase(this->expParts.begin() + i);
        }
    } catch (...) {
        this->type = ERROR;
        throw string("Invalid expression");
    }
}

string Expression::ReversePolishNotation(string exp)
{
    // 运算符包括 + - * / ** ( ) mod
    // 优先级从高到低为 ( ) ** * / mod + -
    // 从左到右扫描中缀表达式
    // 遇到操作数时，将其放入后缀表达式中
    // 遇到运算符时，比较其与栈顶运算符的优先级
    // 如果该运算符优先级大于栈顶运算符优先级，则将该运算符入栈
    // 否则，将栈顶运算符弹出并放入后缀表达式中，再次转到与新栈顶运算符相比较
    // 重复上述操作直到扫描完整个中缀表达式
    // 若遇到括号，则：
    // 如果是左括号，则直接压入栈
    // 如果是右括号，则依次弹出栈顶运算符并放入后缀表达式中，直到遇到左括号，此时将左括号弹出栈，但不放入后缀表达式中
    // 输入中，MOD 为取模运算符，** 为幂运算符
    stack<opType> opStack;
    string rpn;
    for (int i = 0; i < exp.length(); i++) {
        if (exp[i] == ' ') {
            continue;
        }
        if (exp[i] == '+' || exp[i] == '-') {
            while (!opStack.empty() && opStack.top() != op_add && opStack.top() != op_sub &&
                   opStack.top() != op_lbracket) {
                addOpToRPN(rpn, opStack.top());
                opStack.pop();
            }
            opStack.push(exp[i] == '+' ? op_add : op_sub);
        } else if (exp[i] == '*' && exp[i + 1] == '*') {
            while (!opStack.empty() && opStack.top() != op_add && opStack.top() != op_sub && opStack.top() != op_mul &&
                   opStack.top() != op_div && opStack.top() != op_mod && opStack.top() != op_pow &&
                   opStack.top() != op_lbracket) {
                rpn += ' ';
                addOpToRPN(rpn, opStack.top());
                opStack.pop();
            }
            opStack.push(op_pow);
            i += 1;
        } else if (exp[i] == '*' || exp[i] == '/') {
            while (!opStack.empty() && opStack.top() != op_add && opStack.top() != op_sub && opStack.top() != op_mul &&
                   opStack.top() != op_div && opStack.top() != op_mod && opStack.top() != op_lbracket) {
                rpn += ' ';
                addOpToRPN(rpn, opStack.top());
                opStack.pop();
            }
            opStack.push(exp[i] == '*' ? op_mul : op_div);
        } else if (exp[i] == 'M' && exp[i + 1] == 'O' && exp[i + 2] == 'D') {
            while (!opStack.empty() && opStack.top() != op_add && opStack.top() != op_sub && opStack.top() != op_mul &&
                   opStack.top() != op_div && opStack.top() != op_mod && opStack.top() != op_lbracket) {
                rpn += ' ';
                addOpToRPN(rpn, opStack.top());
                opStack.pop();
            }
            opStack.push(op_mod);
            i += 2;
        } else if (exp[i] == '(') {
            opStack.push(op_lbracket);
        } else if (exp[i] == ')') {
            while (opStack.top() != op_lbracket) {
                rpn += ' ';
                addOpToRPN(rpn, opStack.top());
                opStack.pop();
            }
            opStack.pop();
        } else {
            // 用空格分隔变量和常数
            if (rpn.length() > 0) {
                rpn += ' ';
            }
            // 读入变量和常数
            while (exp[i] != ' ' && exp[i] != '+' && exp[i] != '-' && exp[i] != '*' && exp[i] != '/' && exp[i] != '(' &&
                   exp[i] != ')' && exp[i] != 'M' && exp[i] != 'O' && exp[i] != 'D' && exp[i] != '\0') {
                rpn += exp[i];
                i += 1;
            }
            i -= 1;
        }
    }
    while (!opStack.empty()) {
        rpn += ' ';
        // 使用+ - * / ** ( ) mod表示运算符
        addOpToRPN(rpn, opStack.top());
        opStack.pop();
    }
    // 删除多余空格
    while (rpn[0] == ' ') {
        rpn = rpn.substr(1);
    }
    // 删除中间多余空格
    for (int i = 1; i < rpn.length(); i++) {
        if (rpn[i] == ' ' && rpn[i - 1] == ' ') {
            rpn = rpn.substr(0, i) + rpn.substr(i + 1);
            i -= 1;
        }
    }
    return rpn;
}

void Expression::print()
{
    printExp();
}

void Expression::printExp()
{
    switch (type) {
    case REM:
        printRem();
        break;
    case LET:
        printLet();
        break;
    case PRINT:
        printPrint();
        break;
    case INPUT:
        printInput();
        break;
    case GOTO:
        printGoto();
        break;
    case IF:
        printIf();
        break;
    case END:
        printEnd();
        break;
    default:
        break;
    }
}

void Expression::printRem()
{
    cout << "REM" << '\t' << runTimes << endl;
    cout << "    " << exp.substr(3) << endl;
}

void Expression::printLet()
{
    cout << "LET =" << '\t' << runTimes << endl;
    printExpTree(root->child, 4);
}

void Expression::printPrint()
{
    cout << "PRINT" << '\t' << runTimes << endl;
    printExpTree(root->child, 5);
}

void Expression::printInput()
{
    cout << "INPUT" << '\t' << runTimes << endl;
    printExpTree(root->child, 5);
}

void Expression::printGoto()
{
    cout << "GOTO" << '\t' << runTimes << endl;
    printExpTree(root->child, 4);
}

void Expression::printIf()
{
    cout << "IF THEN" << '\t' << runTimes << "  " << IfTimes << endl;
    ExpNode *node0 = root->child;
    ExpNode *node1 = root->child->next;
    ExpNode *node2 = root->child->next->next;
    ExpNode *node3 = root->child->next->next->next;
    root->child->next->next->next = NULL;
    root->child->next->next = NULL;
    root->child->next = NULL;
    printExpTree(node0, 2);
    printExpTree(node1, 2);
    printExpTree(node2, 2);
    printExpTree(node3, 2);
    root->child->next = node1;
    root->child->next->next = node2;
    root->child->next->next->next = node3;
}

void Expression::printEnd()
{
    cout << "END" << '\t' << runTimes << endl;
}

void Expression::printExpTree(ExpNode *root, int width)
{
    if (root == NULL) {
        return;
    }

    for (int i = 0; i < width; i++) {
        cout << ' ';
    }
    cout << root->exp << endl;
    ExpNode *node = root->next;
    while (node != NULL) {
        printExpTree(node, width);
        node = node->next;
    }
    printExpTree(root->child, width + 4);
}

ExpNode *Expression::copyExpTree(ExpNode *root)
{
    if (root == NULL) {
        return NULL;
    }
    ExpNode *node = new ExpNode(root->type, root->exp, NULL);
    node->child = copyExpTree(root->child);
    node->next = copyExpTree(root->next);
    return node;
}

int Expression::executeExp()
{
    int ans = 0;
    switch (type) {
    case REM:
        ans = executeRem();
        break;
    case LET:
        ans = executeLet();
        break;
    case PRINT:
        ans = executePrint();
        break;
    case INPUT:
        ans = executeInput();
        break;
    case GOTO:
        ans = executeGoto();
        break;
    case IF:
        ans = executeIf();
        break;
    case END:
        ans = executeEnd();
        break;
    default:
        break;
    }
    return ans;
}

int Expression::executeRem()
{
    // REM
    // 什么都不做
    runTimes++;
    return 0;
}

int Expression::executeLet()
{
    // LET
    // 计算表达式
    int ans = calculateExpTree(root->child->next);
    // 将结果存入变量
    (*varMap)[root->child->exp].value = ans;
    runTimes++;
    return ans;
}

int Expression::executePrint()
{
    // PRINT
    // 计算表达式
    int ans = calculateExpTree(root->child);
    // 打印结果
    // cout << ans << endl;
    runTimes++;
    return ans;
}

int Expression::executeInput()
{
    // INPUT
    // 读入变量
    int ans = 0;
    // cin >> ans;
    // 将结果存入变量
    // (*varMap)[root->child->exp] = ans;
    runTimes++;
    return ans;
}

int Expression::executeGoto()
{
    // GOTO
    // 读入行号
    int lineNum = calculateExpTree(root->child);
    // 跳转到对应行
    runTimes++;
    return lineNum;
}

int Expression::executeIf()
{
    // IF
    // 计算表达式
    runTimes++;
    int left = calculateExpTree(root->child);
    int right = calculateExpTree(root->child->next->next);
    // 判断是否满足条件
    bool flag;
    if (root->child->next->exp == ">") {
        flag = left > right;
    } else if (root->child->next->exp == "=") {
        flag = left == right;
    } else if (root->child->next->exp == "<") {
        flag = left < right;
    }
    // 如果满足条件，则跳转到对应行
    if (flag) {
        int lineNum = calculateExpTree(root->child->next->next->next);
        IfTimes++;
        return lineNum;
    }
    return 0;
}

int Expression::executeEnd()
{
    // END
    // 什么都不做
    runTimes++;
    return -1;
}

void Expression::storeExpTree(ExpNode *root, int width)
{
    string exp = "";
    if (root == NULL) {
        return;
    }

    for (int i = 0; i < width; i++) {
        exp += ' ';
    }
    exp += root->exp;
    // 如果是变量显示变量调用次数
    // if (root->type == exp_var) {
    //    exp += string("   ") + std::to_string((*varMap)[root->exp].getCallTimes());
    //}
    expressionTree.push_back(exp);

    ExpNode *node = root->next;
    storeExpTree(root->child, width + 4);
    while (node != NULL) {

        storeExpTree(node, width);
        node = node->next;
    }
}

void Expression::storeExpressionTree(ExpNode *root)
{
    expressionTree.clear();
    storeExp();
}

void Expression::storeExpressionTree()
{
    expressionTree.clear();
    if (type == ERROR) {
        expressionTree.push_back("!ERROR!");
        expressionTree.push_back(std::to_string(line) + "    ERROR");
        return;
    }
    storeExp();
}

void Expression::storeExp()
{
    switch (type) {
    case REM:
        storeRem();
        break;
    case LET:
        storeLet();
        break;
    case PRINT:
        storePrint();
        break;
    case INPUT:
        storeInput();
        break;
    case GOTO:
        storeGoto();
        break;
    case IF:
        storeIf();
        break;
    case END:
        storeEnd();
        break;
    default:
        break;
    }
}

void Expression::storeRem()
{
    // REM
    // 什么都不做
    expressionTree.push_back(std::to_string(line) + "  REM" + string("   ") + std::to_string(runTimes));
    expressionTree.push_back("    " + exp.substr(3));
}

void Expression::storeLet()
{
    // LET
    expressionTree.push_back(std::to_string(line) + "  LET =" + string("   ") + std::to_string(runTimes));
    // 计算width长度等于数字line的长度加上6
    int width = 6;
    int temp = line;
    string blank = "      ";
    while (temp > 0) {
        temp /= 10;
        width++;
        blank += ' ';
    }
    // 对于第一个变量，显示变量调用次数
    if (root->child->type == exp_var) {
        expressionTree.push_back(blank + root->child->exp + string("   ") +
                                 std::to_string((*varMap)[root->child->exp].getCallTimes()));
    } else {
        expressionTree.push_back(std::to_string(line) + "    " + root->child->exp);
    }

    storeExpTree(root->child->next, width);
}

void Expression::storePrint()
{
    // PRINT
    expressionTree.push_back(std::to_string(line) + "  PRINT" + string("   ") + std::to_string(runTimes));
    // 计算width长度等于数字line的长度加上7
    int width = 6;
    int temp = line;
    while (temp > 0) {
        temp /= 10;
        width++;
    }
    storeExpTree(root->child, width);
}

void Expression::storeInput()
{
    // INPUT
    expressionTree.push_back(std::to_string(line) + "  INPUT" + string("   ") + std::to_string(runTimes));
    // 计算width长度等于数字line的长度加上4
    int width = 8;
    string blank = "        ";

    int temp = line;
    while (temp > 0) {
        temp /= 10;
        width++;
        blank += ' ';
    }
    // 对于第一个变量，显示变量调用次数
    if (root->child->type == exp_var) {
        expressionTree.push_back(blank + root->child->exp + string("   ") +
                                 std::to_string((*varMap)[root->child->exp].getCallTimes()));
    } else {
        expressionTree.push_back(std::to_string(line) + "    " + root->child->exp);
    }
    // storeExpTree(root->child->next, width);
}

void Expression::storeGoto()
{
    // GOTO
    expressionTree.push_back(std::to_string(line) + "  GOTO" + string("   ") + std::to_string(runTimes));
    // 计算width长度等于数字line的长度加上3
    int width = 7;
    int temp = line;
    while (temp > 0) {
        temp /= 10;
        width++;
    }
    storeExpTree(root->child, width);
}

void Expression::storeIf()
{
    // IF
    expressionTree.push_back(std::to_string(line) + "  IF THEN" + string("   ") + std::to_string(IfTimes) +
                             string("  ") + std::to_string(runTimes - IfTimes));
    ExpNode *node0 = root->child;
    ExpNode *node1 = root->child->next;
    ExpNode *node2 = root->child->next->next;
    ExpNode *node3 = root->child->next->next->next;
    root->child->next->next->next = NULL;
    root->child->next->next = NULL;
    root->child->next = NULL;
    // 计算width长度等于数字line的长度加上3
    int width = 6;
    int temp = line;
    while (temp > 0) {
        temp /= 10;
        width++;
    }
    storeExpTree(node0, width);
    storeExpTree(node1, width);
    storeExpTree(node2, width);
    storeExpTree(node3, width);
    root->child->next = node1;
    root->child->next->next = node2;
    root->child->next->next->next = node3;
}

void Expression::storeEnd()
{
    // END
    expressionTree.push_back(std::to_string(line) + "  END" + string("   ") + std::to_string(runTimes));
}

void Expression::splitExp()
{
    // 找到关键词的位置
    std::vector<std::string> keywords = {"REM", "LET", "PRINT", "INPUT", "GOTO", "IF", "END"};
    int pos = std::string::npos;

    int i = 0;
    for (const std::string &keyword : keywords) {
        pos = exp.find(keyword);
        if (pos != std::string::npos) {
            break;
        }
        i++;
    }

    // 如果没有关键词，则把exp.type设为ERROR
    if (pos == std::string::npos) {
        type = ERROR;
        throw string("Syntax error in your expression.");
        return;
    }

    setType();
    // 检查关键词后是否是空格
    if (exp[pos + keywords[i].length()] != ' ' && type != END && type != CLEAR && type != LIST && type != HELP &&
        type != QUIT && type != RUN && type != LOAD && type != REM) {
        type = ERROR;
        throw string("Syntax error in your expression.");
        return;
    }

    // 如果是REM
    if (type == REM) {
        processRem();
        return;
    }

    // 在运算符前后加入空格
    for (int i = 0; i < exp.length(); i++) {
        if (exp[i] == '*' && exp[i + 1] == '*') {
            if (i > 0 && exp[i - 1] != ' ') {
                exp = exp.substr(0, i) + ' ' + exp.substr(i);
                i += 1;
            }
            if (i < exp.length() - 2 && exp[i + 2] != ' ') {
                exp = exp.substr(0, i + 2) + ' ' + exp.substr(i + 2);
            }
            ++i;
        } else if (exp[i] == '+' || exp[i] == '-' || exp[i] == '/' || exp[i] == '(' || exp[i] == ')' || exp[i] == '>' ||
                   exp[i] == '=' || exp[i] == '<' || exp[i] == '*') {
            if (i > 0 && exp[i - 1] != ' ') {
                exp = exp.substr(0, i) + ' ' + exp.substr(i);
                i += 1;
            }
            if (i < exp.length() - 1 && exp[i + 1] != ' ') {
                exp = exp.substr(0, i + 1) + ' ' + exp.substr(i + 1);
            }
        }
    }

    // 删除多余空格
    while (exp[0] == ' ') {
        exp = exp.substr(1);
    }
    for (int i = 1; i < exp.length(); i++) {
        if (exp[i] == ' ' && exp[i - 1] == ' ') {
            exp = exp.substr(0, i) + exp.substr(i + 1);
            i -= 1;
        }
    }

    // 运算符类型
    std::vector<std::string> operators = {"+", "*", "/", "**", "MOD", "(", ")", "=", ">", "<"};
    std::vector<ExpPartType> operatorsType = {exp_add_part,     exp_mul_part,      exp_div_part,      exp_pow_part,
                                              exp_mod_part,     exp_lbracket_part, exp_rbracket_part, exp_equal_part,
                                              exp_greater_part, exp_less_part};
    // 将各个元素存入expParts
    for (int i = 0; i < exp.length(); i++) {
        string part = "";
        while (exp[i] != ' ' && exp[i] != '\0') {
            part += exp[i];
            i++;
        }
        expParts.push_back(part);
        // 如果是运算符
        if (part == "+" || part == "*" || part == "/" || part == "**" || part == "MOD" || part == "(" || part == ")" ||
            part == "=" || part == ">" || part == "<") {
            expPartTypes.push_back(
                operatorsType[std::find(operators.begin(), operators.end(), part) - operators.begin()]);
        }
        // 如果是指令
        else if (part == "REM" || part == "LET" || part == "PRINT" || part == "INPUT" || part == "GOTO" ||
                 part == "IF" || part == "THEN" || part == "END") {
            expPartTypes.push_back(exp_ins_part);
        }
        // 如果是变量
        else if (part[0] >= 'A' && part[0] <= 'Z' || part[0] >= 'a' && part[0] <= 'z' || part[0] == '_') {
            // 检查变量名是否合法
            bool ok = true;
            for (int i = 1; i < part.length(); i++) {
                if (!(part[i] >= 'A' && part[i] <= 'Z' || part[i] >= 'a' && part[i] <= 'z' ||
                      part[i] >= '0' && part[i] <= '9' || part[i] == '_')) {
                    ok = false;
                    break;
                }
            }
            if (part == "REM" || part == "LET" || part == "PRINT" || part == "INPUT" || part == "GOTO" ||
                part == "IF" || part == "THEN" || part == "END" || part == "MOD") {
                ok = false;
            }
            if (ok) {
                expPartTypes.push_back(exp_var_part);
            } else {
                expPartTypes.push_back(exp_error_part);
                this->type = ERROR;
                throw string("Syntax error in your expression.");
            }
        }
        // 如果是数字
        else if (part[0] >= '0' && part[0] <= '9') {
            bool ok = true;
            for (int i = 1; i < part.length(); i++) {
                if (part[i] < '0' || part[i] > '9') {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                expPartTypes.push_back(exp_num_part);
            } else {
                expPartTypes.push_back(exp_error_part);
                this->type = ERROR;
                throw string("Syntax error in your expression.");
            }
        }
        // 区分减号和负号
        else if (part == "-") {
            ExpPartType type = expPartTypes[expPartTypes.size() - 1];
            if (type == exp_add_part || type == exp_sub_part || type == exp_mul_part || type == exp_div_part ||
                type == exp_pow_part || type == exp_mod_part || type == exp_lbracket_part || type == exp_ins_part ||
                type == exp_equal_part || type == exp_greater_part || type == exp_less_part) {
                expPartTypes.push_back(exp_neg_part);
            } else {
                expPartTypes.push_back(exp_sub_part);
            }
        }
        // 其他情况
        else {
            expPartTypes.push_back(exp_error_part);
            this->type = ERROR;
            throw string("Syntax error in your expression.");
        }
    }
    // 括号匹配
    int numOfLbracket = 0;
    int numOfRbracket = 0;
    for (int i = 0; i < expParts.size(); i++) {
        if (expParts[i] == "(") {
            numOfLbracket++;
        } else if (expParts[i] == ")") {
            numOfRbracket++;
        }
    }
    if (numOfLbracket != numOfRbracket) {
        expPartTypes.push_back(exp_error_part);
        this->type = ERROR;
        throw string("Syntax error in your expression.");
    }

    // 如果加减号连续出现，则为错误
    for (int i = 0; i < expPartTypes.size() - 1; i++) {
        if (expPartTypes[i] == exp_add_part || expPartTypes[i] == exp_sub_part || expPartTypes[i] == exp_neg_part ||
            expPartTypes[i] == exp_mul_part || expPartTypes[i] == exp_div_part || expPartTypes[i] == exp_pow_part ||
            expPartTypes[i] == exp_mod_part) {
            if (expPartTypes[i + 1] == exp_add_part || expPartTypes[i + 1] == exp_sub_part ||
                expPartTypes[i + 1] == exp_neg_part) {
                expPartTypes.push_back(exp_error_part);
                this->type = ERROR;
                throw string("Syntax error in your expression.");
            }
        }
    }
}

string Expression::ExpPartTypeToString(ExpPartType type)
{
    switch (type) {
    case exp_add_part:
        return "+";
    case exp_sub_part:
        return "-";
    case exp_mul_part:
        return "*";
    case exp_div_part:
        return "/";
    case exp_pow_part:
        return "**";
    case exp_mod_part:
        return "MOD";
    case exp_lbracket_part:
        return "(";
    case exp_rbracket_part:
        return ")";
    case exp_equal_part:
        return "=";
    case exp_greater_part:
        return ">";
    case exp_less_part:
        return "<";
    case exp_ins_part:
        return "INS";
    case exp_var_part:
        return "VAR";
    case exp_num_part:
        return "NUM";
    case exp_neg_part:
        return "-";
    case exp_error_part:
        return "ERROR";
    case exp_empty_part:
        return "EMPTY";
    default:
        return "ERROR";
    }
}

bool Expression::checkExpTree(ExpNode *root)
{
    if (root == NULL) {
        return true;
    }
    if (root->type == exp_op) {
        if (root->child == NULL || root->child->next == NULL) {
            throw string("Invalid expression in checkExpTree()");
            return false;
        }
    }
    return checkExpTree(root->child) && checkExpTree(root->next);
}

bool Expression::checkIfExpTree(ExpNode *root)
{
    if (root == NULL) {
        return true;
    }
    if (root->type == exp_op) {
        if (root->child == NULL || root->child->next == NULL) {
            throw string("Invalid expression in checkIfExpTree()");
            return false;
        }
    }
    return checkIfExpTree(root->child) && checkIfExpTree(root->next);
}