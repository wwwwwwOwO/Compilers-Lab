# 词法分析扫描器的设计实现

19335074 黄玟瑜

19级计算机科学与技术（超算）

## 实验要求

设计XX(以C为例)语言的词法分析器

+ 词法规则
  + 了解所选择编程语言单词符号及其种别值

+ 功能
  + 输入一个C语言源程序文件demo.c
  + 输出一个文件tokens.txt，该文件包括每一个单词及其种类枚举值，每行一个单词

+ 提交5个文件
  + 实验报告（所支持的单词范围，自动机设计，设计思路）
  + C语言词法分析源程序：source.c（源程序包）
  + C语言词法分析程序的可执行文件：clang.out/clang.exe
  + C语言源程序文件：demo.c（实验输入）
  + 词法分析及结果文件： tokens.txt（实验输出）

+ 同时上传源码至Github

## 引言

### 概述

在本次实验中，笔者先根据C语言的语言规则为每个单词规定了种别码，确定了合法字符集，随后根据合法字符集

和每个单词的识别规则构建了确定的有限自动机，随后根据该自动机编写了词法分析程序。

### 实验环境

Windows 11

gcc version 8.1.0 (x86_64-posix-seh-rev0, Built by MinGW-W64 project)

### 编译

```shell
mingw32-make
```

### 运行

```shell
ming32-make test
```

### 说明

1. 实验报告即本文档

2. 源程序包括：main.cpp、Lexer.cpp、Lexer.h、DataType.cpp、DataType.h五个文件

3. 可执行文件为Lexer.exe，运行需指定输入文件和输出文件，如

   ```
   ./Lexer.exe demo.c tokens.txt
   ```

4. 实验输入为demo.c，包含了大部分可识别的单词

5. 词法分析及结果文件为 tokens.txt

6. Github仓库 https://github.com/wwwwwwOwO/Compilers-Lab

## 建立自动机

词法分析器的功能输入源程序，按照构词规则分解成一系列单词符号。单词是语言中具有独立意义的最小单位，包括关键字、标识符、运算符、界符和常量等

1. **关键字** 是由程序语言定义的具有固定意义的标识符。例如，Pascal 中的begin，end，if，while都是保留字。这些字通常不用作一般标识符。
2. **标识符** 用来表示各种名字，如变量名，数组名，过程名等等。
3. **常数**  常数的类型一般有整型、实型、布尔型、文字型等。
4. **运算符** 如+、-、*、/等等。
5. **界符**  如逗号、分号、括号、等等。

### 识别规则（识别范围）

各个Token类的识别规则如下（本实验所实现的自动机的识别范围）：

| 类别   | 识别规则                                                     | 映射关系         | 种别码 |
| ------ | ------------------------------------------------------------ | ---------------- | ------ |
| 关键字 | auto、break、case、char、const、continue、default、do、double、else、enum、extern、float、for、goto、if、int、long、register、return、short、signed、sizeof、static、struct、switch、typedef、unsigned、union、void、volatile、while | 一字一种         | 0~31   |
| 标识符 | c(c\|d)*  (c为字母，d为数字)                                 | 统一作为一种单词 | 32     |
| 常数   | d*、"c\*"、'c'，(c为字母，d为数字)                           | 一型一种         | 33~35  |
| 运算符 | (、)、[、]、->、!、~、++、--、-、*、&、/、%、+、<<、>>、<、<=、>、>=、==、!=、\^、\|、\&\&、\|\|、\=、+\=、-\=、\*\=、/\=、%\=、&\=、^\=、\|\= | 一字一种         | 36~71  |
| 界符   | ;、{、}、,、:                                                | 一字一种         | 72~76  |

### 字符集

| 类别     | 字符                                                  |
| -------- | ----------------------------------------------------- |
| 字母     | a\~z、A\~Z                                            |
| 数字     | 0\~9                                                  |
| 空白符   |                                                       |
| 换行符   |                                                       |
| 运算符   | (、)、[、]、-、+、\*、%、<、>、=、\|、&、\^、！、~、^ |
| 单引号   | '                                                     |
| 双引号   | "                                                     |
| 斜杠     | /                                                     |
| 界符     | ;、{、}、,、:                                         |
| 非法字符 | 其他字符                                              |

### 注释

注释的识别规则为：//+换行符，识别到注释时可以跳过。

### 自动机状态转换表

根据上述识别规则，可以得到10个状态，分别为Blank、Number、Letter、SingleQuote、DoubleQuote、Operator、Delimiter、Bar、Error、Comment。

| 状态        | 说明                                                         |
| ----------- | ------------------------------------------------------------ |
| Blank       | 起始状态，遇到非空白符时终止，同时转移到下一状态             |
| Number      | 数字状态，遇到非数字符号时终止                               |
| Letter      | 字母状态，遇到非数字、字母时终止                             |
| SingleQuote | 单引号状态，遇到换行或单引号时终止                           |
| DoubleQuote | 双引号状态，遇到换行或双引号时终止                           |
| Operator    | 运算符状态，遇到任何符号都会终止（运算符长度<=2)             |
| Delimiter   | 界符，遇到任何符号都会终止（界符长度=1)                      |
| Bar         | 遇到'/'进入注释状态，遇到其他'='当作'/='处理，遇到其他符号当作除号'/'处理 |
| Error       | 错误状态，遇到换行或空格终止                                 |

因此状态转换表如下：

|      |cBlank| cNumber| cLetter| cSingleQuote| cDoubleQuote| cOperator| cNewline| cBar| cDelimiter| cIllegal|
| ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
|Blank      | Blank | Number | Letter | SingleQuote | DoubleQuote | Operator | Blank | Bar | Delimiter | Error |
|Number      | Blank | Number | Error | SingleQuote | DoubleQuote | Operator | Blank | Bar | Delimiter | Error |
|Letter      | Blank | Letter | Letter | SingleQuote | DoubleQuote | Operator | Blank | Bar | Delimiter | Error |
|SingleQuote      | Blank | SingleQuote | SingleQuote | Blank | SingleQuote | SingleQuote | Blank | SingleQuote | SingleQuote | SingleQuote |
|DoubleQuote      | Blank | DoubleQuote | DoubleQuote | DoubleQuote | Blank | DoubleQuote | Blank | DoubleQuote | DoubleQuote | DoubleQuote |
|Operator      | Blank | Number | Letter | SingleQuote | DoubleQuote | Operator or Blank or Error | Blank | Error | Delimiter | Error |
|Delimiter      | Blank | Number | Letter | SingleQuote | DoubleQuote | Operator | Blank | Bar | Delimiter | Error |
|Bar      | Blank | Number | Letter | SingleQuote | DoubleQuote | Blank(=) or Error(other) | Blank | Comment | Delimiter | Error |
|Error      | Blank | Error | Error | Error | Error | Error | Blank | Error | Error | Error |
|Comment      | Comment | Comment | Comment | Comment | Comment | Comment | Blank | Comment | Comment | Comment |

## 程序主体架构

程序包含5个文件：

+ main.cpp：主程序，输入需要分析的文件后，让Lexer进行词法分析
+ Lexer.cpp：词法分析器Lexer的实现，提供接口给main函数调用
+ Lexer.h：Lexer类的定义
+ DataType.cpp：Token类、Tokens类（Token序列）、ErrorMsg类（错误信息类）的实现，提供接口给Lexer调用
+ DataType.h：Token类、Tokens类（Token序列）、ErrorMsg类（错误信息类）的定义，种别码定义

在输入一个文件后，程序将改文件交给Lexer处理，Lexer进行词法分析的函数为`AnalyseTokens`

```cpp
void Lexer::AnalyseTokens(ifstream& sourceCode, ErrorMsgs& errorMsgs, Tokens& tokens);
```

程序主体架构如下：

```cpp
// void Lexer::AnalyseTokens(ifstream& sourceCode, ErrorMsgs& errorMsgs, Tokens& tokens);
switch(status)
{
    case Blank:
        switch(symbol)
        {
            case Blank:
                ...
                state = Blank;
                break;
            case Letter:
                ...
                break;
            case Number:
                ...
                break;
            ...
        }
    case Number:
        ...
    ...
}
```

`AnalyseTokens`对输入字符流`sourceCode`进行词法分析，将合法的Token序列存储在`tokens`中，同时将错误信息存储在`errorMsgs`中。

## 实验结果

对demo.c进行词法分析：

demo.c

```c
int main (void)
{
    int a=1, d=2,c;
    if(a <= d)
    {
        c=a;
        a=d;
        d=c;
    }
    a = ~a;
    d *= 2;
    switch(a)
    {
        case 1:
            c--;
            break;
        case 2:
            c++;
            break;
    }
    char ch[10]="ok"; // comment
    char x,y='a';
    c=a+d;  
    return 0;
}
```

分析结果如下，输出的Token格式为`<种别码> 值`

```
<16> int
<32> main
<36> (
<29> void
<37> )
<75> {
<16> int
<32> a
<71> =
<34> 1
<72> ,
<32> d
<71> =
<34> 2
<72> ,
<32> c
<73> ;
<15> if
<36> (
<32> a
<53> <=
<32> d
<37> )
<75> {
<32> c
<71> =
<32> a
<73> ;
<32> a
<71> =
<32> d
<73> ;
<32> d
<71> =
<32> c
<73> ;
<76> }
<32> a
<71> =
<59> ~
<32> a
<73> ;
<32> d
<65> *=
<34> 2
<73> ;
<25> switch
<36> (
<32> a
<37> )
<75> {
<02> case
<34> 1
<74> :
<32> c
<42> --
<73> ;
<01> break
<73> ;
<02> case
<34> 2
<74> :
<32> c
<41> ++
<73> ;
<01> break
<73> ;
<76> }
<03> char
<32> ch
<38> [
<34> 10
<39> ]
<71> =
<33> ok
<73> ;
<03> char
<32> x
<72> ,
<32> y
<71> =
<35> a
<73> ;
<32> c
<71> =
<32> a
<43> +
<32> d
<73> ;
<19> return
<34> 0
<73> ;
<76> }
```

对照种别码表，实验结果符合预期。

## 附录：Token类别码对照表

| 类别码 | 单词       | 类别码 | 单词 |
| ------ | ---------- | ------ | ---- |
| 0      | auto       | 39     | ]    |
| 1      | break      | 40     | ->   |
| 2      | case       | 41     | ++   |
| 3      | char       | 42     | --   |
| 4      | const      | 43     | +    |
| 5      | continue   | 44     | -    |
| 6      | default    | 45     | *    |
| 7      | do         | 46     | /    |
| 8      | double     | 47     | %    |
| 9      | else       | 48     | <<   |
| 10     | enum       | 49     | >>   |
| 11     | extern     | 50     | >    |
| 12     | float      | 51     | <    |
| 13     | for        | 52     | >=   |
| 14     | goto       | 53     | <=   |
| 15     | if         | 54     | ==   |
| 16     | int        | 55     | !=   |
| 17     | long       | 56     | !    |
| 18     | register   | 57     | \|\| |
| 19     | return     | 58     | &&   |
| 20     | short      | 59     | ~    |
| 21     | signed     | 60     | \|   |
| 22     | sizeof     | 61     | &    |
| 23     | static     | 62     | ^    |
| 24     | struct     | 63     | +=   |
| 25     | switch     | 64     | -=   |
| 26     | typedef    | 65     | *=   |
| 27     | unsigned   | 66     | /=   |
| 28     | union      | 67     | %=   |
| 29     | void       | 68     | &=   |
| 30     | volatile   | 69     | \|=  |
| 31     | while      | 70     | ^=   |
| 32     | identifier | 71     | =    |
| 33     | string     | 72     | ,    |
| 34     | integer    | 73     | ;    |
| 35     | character  | 74     | :    |
| 36     | (          | 75     | {    |
| 37     | )          | 76     | }    |
| 38     | [          |        |      |

