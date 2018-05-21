/*
编译器写作步骤：
1. 设计关键字：int char if else return main void exit printf sizeof bool
2. 设计各种数据段，如：data段, text段, bss段, 堆, 栈
3. 设计符号属性，标识符类型，哈希值，名称，作用域，数据类型，变量值，全局变量作用域，全局变量数据类型，全局变量值
4. 设计语言语法

解析步骤：
1. 读取参数
2. 初始化各段
3. 初始化关键字
4. 初始化系统函数
5. 读取文件
6. 解析全局变量
7. 解析函数，代码块，表达式
8. 执行程序
 */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>

//data段/代码指针
char *data, *codep, *current;
//text段，符号表，符号
int *text, *ltext, *symbol, mark, hash, *id, line = 0;
//关键字
enum {
    Num = 128, Sys, Fun, Glo, Loc, Id,
    Char, Int,
    Assign, Cond
};
//数据类型
enum {CHAR };
//汇编操作码
enum {PRTF, EXIT, VOID, MAIN };
//ID，标识符
int idSize = 8;
enum {Mark, Hash, Name, Class, Type, Value, GClass, GType, GValue };

void next() {
    mark = *codep;
    while(mark) {
        if(mark == '\n') {
            line++;
        } else if((mark >= 'a' && mark <= 'z') || (mark >= 'A' && mark <= 'Z') || mark == '_') {
            current = codep;
            hash = mark;
            while((*codep >= 'a' && *codep <= 'z') 
                || (*codep >= 'A' && *codep <= 'Z') 
                || (*codep >= '0' && *codep <= '9') || *codep == '_') 
                hash = hash * 147 + *codep++;
            hash = hash << 6 + codep - current;
            id = symbol;
            while(id[Mark]) {
                if(id[Hash] == hash && !memcmp((char *)id[Name], current, codep-current)) { mark = id[Mark]; return;}
                id = id + idSize;
            }
            id[Name] = (int)current;
            id[Hash] = hash;
            mark = id[Mark] = Id;
            return;
        } else if(mark >= '0' && mark <= '9') {

        } else if(mark == '"') {
        } else {
        }
    }
    return;
}

//表达式
void expr() {
    if(mark == '"') {
        ival = data;
        codep++;
        while(*codep != '"') { *ival++ = *codep++;}
    } else if(mark == Id) {
        next();
        if(mark == '(') {
            expr(Assign);
        } else {
        }
    }

}

//代码块 if/else/while/return
void stmt() {
    if(mark == If) {
    } else if(mark == Else) {
    } else if(mark == Return) {
    } else if(mark == Return) {
    } else {
        next();
        expr(Assign);
    }
}

//程序
void params() {
    next();
    while(mark) {
        ty = INT;
        //解析全局变量
        if(mark == Int) { next(); } 
        else if(mark == Char) {ty = CHAR; next(); } 
        else {}
        if(mark == '(') {
            id[Type] = ty;
            while(mark != ';' && mark != '{') {
                ty = INT;
                if(mark == Int) { next(); } 
                else if(mark == Char) { ty = CHAR; next(); } 
                else {}
                if(mark != Id) { printf("函数定义错误！"); return; }
                if(mark != '}') stmt();
            }
        } else {
            id[Class] = Glo;
            id[Type] = ty;
            //id[Value] = Int;
        }
    }
}

//执行程序，实质是一个虚拟机
void eval() {
}

int main(int argc, char *argv) {
    char *fname ="./hello.c";
    int fp, result, *idmain;
    int i, msize = 256 * 1024;

    //分配内存
    codep = malloc(msize);
    data = malloc(msize);
    text = malloc(msize);

    fp = open(fname, O_RDONLY);
    if(fp < 0) {
        printf("不能打开文件！");
        exit(1);
    }
    result = read(fp, codep, msize);
    if(result) {
        printf("文件: %d", result);
    }

    //将关键字加入符号表
    codep = "char printf exit void main";
    for(i = Char; i <= Int; i++) {next(); id[Mark] = i; }
    for(i = PRTF; i <= EXIT; i++) {next(); id[Class] = Sys; id[Type] = Int; id[Value] = i; }
    next(); id[Value] = CHAR;
    next(); idmain = id;

    //执行程序解析
    params();

    //执行程序
    eval();

    return 0;
}

