# QBasic 设计思路

## 1. 类的继承关系

- ``MainWindow``: 主窗口类，继承自 ``QMainWindow``，负责整个程序的主界面。
``MainWindow`` 包含``centralwidget``，``statusbar``，``menubar`` 。 其中``centralwidget``使用``vertiacalLayout``布局，分成四个部分。 第一个部分使用``horizontalLayout``分为两个部分，``CodeDisplay``和``textBrowser``。 第二部分是``treeDisplay``，用于展示语法树。 第三部分是三个按钮，分别是``载入代码``，``运行代码``，和``清除代码``。 ``btnLoad``，``btnRun``，``btnClear``。 第四部分是一个``cmdLineEdit``，用于输入代码。  
这个类的主要作用是负责整个程序的主界面，以及各个部件的布局。

- ``Expression``: 表达式类，用于存放每条语句。通过``map``关联一个键值``lineNum``，表示这条语句的序号。
``Expression``包含``int runTimes``, ``type expType``, ``QString expStr``,``exp_node``分别表示这条语句的运行次数，语句类型，和语句内容，以及语法树的根节点。

- ``exp_node``: 语法树节点类，用于存放语法树的节点。包含``type nodeType``, ``QString nodeStr``, ``exp_node *child``, ``exp_node *brother``，分别表示节点类型，节点内容，子节点，兄弟节点。

- 枚举类型``type``: 用于表示语句类型。

```c++
enum type{
    // 语句类型
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
    QUIT
};
```

## 2. Bugs_list

- 使用/n换行时，会调用两次``on_cmdLineEdit_editingFinished``，导致代码运行两次。

- 负号的识别和处理。

计划更改：使用``QRegExp``进行匹配，匹配到负号时，将其替换为``-1*``。

改为单条语句执行。

还差负号处理和错误语句处理。

48 204
