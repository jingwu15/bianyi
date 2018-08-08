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
6. 解析全局变量 -> 函数 -> 代码块 -> 表达式
7. 执行程序

示例程序：
#include <stdio.h>

int main(int argc, char *argv) {
    printf("hello!");
    return 0;
}
 */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>

//data段/代码指针
char *data, *src, *current;
//text段，符号表，符号
int *text, *ltext, *symbol, mark, hash, *id, *idmain, *currentId, ty, *bp, *sp, *lsp, ival, line = 0;
//关键字
enum {
    Num = 128, Sys, Fun, Glo, Loc, Id,
    Char, Int, Return, 
    Assign, Cond
};
//数据类型
enum {CHAR, INT, PTR};
//汇编操作码
enum { LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,
       OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
       PRTF,EXIT };
//ID，标识符
int idSize = 8;
enum {Mark, Hash, Name, IdType, DataType, Value, GIdType, GDataType, GValue };

void next() {
    while(mark = *src) {
        src++;
        if(mark == '\n') {
        } else if(mark == '#') {
            while(*src != '\n') src++;
            src++;
            line++;
        } else if((mark >= 'a' && mark <= 'z') || (mark >= 'A' && mark <= 'Z') || mark == '_') {
            current = src - 1;
            hash = mark;
            while((*src >= 'a' && *src <= 'z') 
                || (*src >= 'A' && *src <= 'Z') 
                || (*src >= '0' && *src <= '9') || *src == '_') 
                hash = hash * 147 + *src++;
            hash = hash << 6 + src - current;
            //printf("--%d----\n", hash);
            //printf("--%c----\n", *(current+1));
            //printf("--%d--%s--\n", hash, *current);
            id = symbol;
            while(id[Mark]) {
                if(id[Hash] == hash && !memcmp((char *)id[Name], current, src-current)) { mark = id[Mark]; return; }
                id = id + idSize;
            }
            id[Name] = (int)current;
            id[Hash] = hash;
            mark = id[Mark] = Id;
            return;
        } else if(mark >= '0' && mark <= '9') {

        } else if(mark == '"') {      //解析字符串
            current = data;
            while(*src != '"') {
                ival = *src++;
                if(ival == '\\') {
                    if(*src++ == 'n') ival = '\n';
                }
                *data++ = ival;
            }
            ++src;
            ival = (int)current;
            return;
        } else if(mark == '\'') {      //解析单个字符
        } else if(mark == '(' || mark == ';' || mark == ')' || mark == '{' || mark == '}'){ return; 
        } else {}
    }
    return;
}

//表达式
void expr(int level) {
    int t;
    if(mark == '"') {     //表达式参数
        *++text = IMM; *++text = ival; next();
        while(mark == '"') next(); 
        data = (char *)((int)data + sizeof(int) & -sizeof(int)); ty = PTR;     //最后一位加0，作为字符串的结束
    } else if(mark == Id) {
        currentId = id;
        next();
        if(mark == '(') {
            t = 0;
            next();
            while(mark != ')') { expr(Assign); *++text = PSH; ++t; if(mark == ',') next(); }
            next();
            if(id[IdType] == Sys) { *++text = currentId[Value]; }
            else if(id[IdType] == Fun) { }
            else {printf("错误的函数调用！");}
            if(t) { *++text = ADJ; *++text = t;}
            ty = currentId[DataType];
        } else {
        }
    }
    while(mark >= level) {
    }
}

//语句 if/else/while/return
void stmt() {
    if(mark == Return) {
        next();
        if (mark != ';') expr(Assign);
        *++text = LEV;
        if (mark == ';') next(); else { printf("%d: 未获取到期望的;号\n", line); exit(-1); }
    } else if(mark == ';') {
        next();
    } else if(mark == '{') {
        next();
        while(mark == '}') stmt();
        next();
    } else if(mark == ';') {
        next();
    } else {
        //printf("stmt---%c---\n", (char *)mark);
        expr(Assign);
    }
}

//程序
void params() {
    int ty, i, loc;

    //先解析数据类型，后解析变量名或函数名
    next();
    while(mark) {
        ty = INT;
        //解析全局变量
        if(mark == Int) { next(); }
        else if(mark == Char) { ty = CHAR; next(); }
        else {}
        while(mark != ';' && mark != '{') {
            if(mark != Id) { printf("全局标识符[%s]重复定义！\n", (char *)id[Name]); return; }
            if(id[IdType]) { printf("全局变量[%s]不可重复定义！\n", (char *)id[Name]); return;}
            id[DataType] = ty;
            next();
            if(mark == '(') {
                id[IdType] = Fun;
                id[Value] = (int)(text + 1);
                //printf("xxxxxxxxxxxxxxxxxx-%d-\n", id[Value]);
                ty = INT;
                //参数
               i = 0;
                next();
                while(mark != ')') {
                    if(mark == Int) { next(); }
                    if(mark == Char) { ty=CHAR; next(); }
                    if(mark != Id) { printf("参数定义错误！"); }
                    id[IdType] = Loc;
                    //id[Value] = i;
                    next();
                    if(mark == ',') next();
                }
                next();
                loc = ++i;
                *++text = ENT; *++text = i - loc;        // 设置进入函数指令，i-loc为函数范围，可以理解为多小个CPU指令
                if(mark != '{') { printf("函数体定义错误！"); }
                //printf("mark--stmt-%c--\n", (char)mark);
                next();
                //printf("stmt-before-%d-\n", mark);
                while(mark != '}') { stmt(); } //printf("stmt-after-%c-\n", (char)mark);
            } else {
                //printf("if-else----xxx\n");
                //全局变量定义
                //id[IdType] = Glo;
                //id[Value] = i;
            }
            next();
        }
        next();
    }
}

//执行程序，实质是一个虚拟机
void eval() {
    // 设置栈
    int cycle = 0, i = 0, *pc, *t, a, msize = 256 * 1024;
    bp = sp = (int *)((int)sp + msize);
    *--sp = EXIT; // 如果main返回，调用exit
    *--sp = PSH; t = sp;
    //*--sp = argc;
    //*--sp = (int)argv;
    *--sp = (int)t;
    pc = (int *)idmain[Value];
    //printf("%d------------\n", idmain[Value]);

    // 执行汇编代码
    while(1) {
        i = *pc++;
        ++cycle;         //CPU的指令周期，每循环一次，代表一条指令
        //if(debug) {
        //if(1) {
        //    printf("%d> %.4s", cycle,
        //      &"LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,"
        //       "OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,"
        //       "PRTF,EXIT,"[i * 5]);
        //    if(i <= ADJ) printf(" %d\n", *pc); else printf("\n");
        //}
        if      (i == LEA) a = (int)(bp + *pc++);                             // 加载局部变量
        else if (i == IMM) a = *pc++;                                         // 加载全局变量或立即数
        else if (i == JMP) pc = (int *)*pc;                                   // 跳转
        else if (i == JSR) { *--sp = (int)(pc + 1); pc = (int *)*pc; }        // 跳转到子程序
        else if (i ==  BZ) pc = a ? pc + 1 : (int *)*pc;                      // 为零分支
        else if (i == BNZ) pc = a ? (int *)*pc : pc + 1;                      // 非零分支
        else if (i == ENT) { *--sp = (int)bp; bp = sp; sp = sp - *pc++; }     // 进入子程序
        else if (i == ADJ) sp = sp + *pc++;                                   // 马上进入子函数，设置堆栈，将栈顶指针加上子函数的长度
        else if (i == LEV) { sp = bp; bp = (int *)*sp++; pc = (int *)*sp++; } // 离开子程序
        else if (i ==  LI) a = *(int *)a;                                     // 加载整数
        else if (i ==  LC) a = *(char *)a;                                    // 加载字符
        else if (i ==  SI) *(int *)*sp++ = a;                                 // 存储整数
        else if (i ==  SC) a = *(char *)*sp++ = a;                            // 存储字符
        else if (i == PSH) *--sp = a;                                         // 压栈

        else if (i ==  OR) a = *sp++ |  a;
        else if (i == XOR) a = *sp++ ^  a;
        else if (i == AND) a = *sp++ &  a;
        else if (i ==  EQ) a = *sp++ == a;
        else if (i ==  NE) a = *sp++ != a;
        else if (i ==  LT) a = *sp++ <  a;
        else if (i ==  GT) a = *sp++ >  a;
        else if (i ==  LE) a = *sp++ <= a;
        else if (i ==  GE) a = *sp++ >= a;
        else if (i == SHL) a = *sp++ << a;
        else if (i == SHR) a = *sp++ >> a;
        else if (i == ADD) a = *sp++ +  a;
        else if (i == SUB) a = *sp++ -  a;
        else if (i == MUL) a = *sp++ *  a;
        else if (i == DIV) a = *sp++ /  a;
        else if (i == MOD) a = *sp++ %  a;

        //else if (i == PRTF) { printf("--------printf----\n"); t = sp + pc[1]; a = printf((char *)t[-1], t[-2], t[-3], t[-4], t[-5], t[-6]); }
        else if (i == PRTF) { t = sp + pc[1]; a = printf((char *)t[-1], t[-2], t[-3], t[-4], t[-5], t[-6]); }
        else if (i == EXIT) { printf("exit(%d) cycle = %d\n", *sp, cycle); return; }
        else { printf("unknown instruction = %d! cycle = %d\n", i, cycle); return; }
    }
}

int main(int argc, char *argv) {
    char *fname ="./hello.c";
    int fp, result;
    int i, msize = 256 * 1024;

    //分配内存
    data     = malloc(msize);
    text     = malloc(msize);
    symbol   = malloc(msize);
    lsp = sp = malloc(msize);

    //将关键字加入符号表
    src = "char int return printf exit void main";
    for(i = Char; i <= Return; i++) { next(); id[Mark] = i; }
    for(i = PRTF; i <= EXIT; i++)   { next(); id[IdType] = Sys; id[DataType] = Int; id[Value] = i; }
    next(); id[Value] = CHAR;
    next(); idmain = id;
    //printf("main--%d--%s--\n", idmain[IdType], (char *)idmain[Name]);

    src = malloc(msize);
    fp = open(fname, 0); if(fp < 0) { printf("不能打开文件！"); return -1; }
    result = read(fp, src, msize); if(result <= 0) { printf("文件: %d", result); return -1; }

    //执行程序解析
    params();

    //执行程序
    eval();
    //printf("%d-%d-%d-%d-%d-%p-%d-%d", idmain[0], idmain[1], idmain[2], idmain[3], idmain[4], idmain[5], idmain[6], idmain[7]);

    printf("end! \n");
    return 0;
}

