# Experience from QBasic

## Outline of architecture of QBasic

1. My mainwindow is a QMainwindow, which consists of a CodeDisplay, a cmdInput, a syntaxTree, and a resultDisplay, besides three buttons for running, loading and clearing the code.

2. 当按Load按钮时，会弹出一个QFileDialog，让用户选择一个文件，然后将文件中的内容按行解析成多个Expression，然后将这些Expression放到CodeDisplay中，这里做parse检查，如果有错误，将Expression的type设置为Error，不显示。

3. 当按Run按钮时，会将CodeDisplay中的所有Expression按顺序执行，每执行一个Expression，都会将其结果显示在resultDisplay中，如果有错误，将Expression的type设置为Error，显示在语法树中。

4. 当按Clear按钮时，会将CodeDisplay中的所有Expression清空，将resultDisplay中的内容清空，将语法树中的内容清空。

## 创建Expression类

1. 传递String参数到Expression类的构造函数中，第一步先区分符号，将所有特殊符号+-*/ ** 的前后加上空格，进行第一次检查是否有不存在该类型，如果有，将type设置为Error；如果没有进行下一步解析，进行polish notation的转换，如果有错误，将type设置为Error，如果没有，将type设置为Expression，下一步构造成树，接着检查各个根节点的type，如果有错误，将type设置为Error，如果没有，将type设置为Expression。

2. 反思： 这里的检查过程可以用正则表达式来实现，这样会更加简洁，但是我不会正则表达式，所以就用了这种笨方法。（AI骂得好）
正则表达式检查方法：

 ```C++
QRegExp regExp("[0-9]+");
if (regExp.exactMatch(str)) {
    // do something
}

 ```

3. 运行Expression类的run函数，首先判断type是否为Expression，如果不是，直接返回；如果是，判断type是否为Error，如果是，将result设置为Error，返回；如果不是，将result设置为运算结果，返回。

反思： 运算Expression的run函数可以放在MainWindow中，这样在判断类型运行，以及对整体的处理的时候会更加方便，但是这样会破坏Expression类的封装性，所以我还是选择了在Expression类中运行。
