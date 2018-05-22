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
enum {INT, CHAR };
//汇编操作码
enum { LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,
       OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
       OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT };
//ID，标识符
int idSize = 8;
enum {Mark, Hash, Name, IdType, DataType, Value, GIdType, GDataType, GValue };

void next() {
    while(mark = *src) {
        src++;
        if(mark == '\n') {
            line++;
        } else if((mark >= 'a' && mark <= 'z') || (mark >= 'A' && mark <= 'Z') || mark == '_') {
            printf("next in id \n");
            current = src-1;
            hash = mark;
            while((*src >= 'a' && *src <= 'z') 
                || (*src >= 'A' && *src <= 'Z') 
                || (*src >= '0' && *src <= '9') || *src == '_') 
                hash = hash * 147 + *src++;
            hash = hash << 6 + src - current;
            id = symbol;
            while(id[Mark]) {
                if(id[Hash] == hash && !memcmp((char *)id[Name], current, src-current)) { mark = id[Mark]; return;}
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
                if(mark == '"') *data++ = ival;
            }
            ival = (int)current;
            return;
        } else if(mark == '\'') {      //解析单个字符
        } else {
        }
    }
    return;
}

//表达式
void expr(int level) {
    int t;
    if(mark == '"') {     //表达式参数
        *++text = IMM; *++text = ival; next();
        while(mark == '"') next(); 
        ival = (int)data;
        while(*src != '"') { ival = *src++; }
    } else if(mark == Id) {
        currentId = id;
        next();
        if(mark == '(') {
            t = 0;
            while(mark != ')') { expr(Assign); *++text = PSH; ++t; if(mark == ',') next(); }
            next();
            if(id[IdType] == Sys) { }
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
    } else if(mark == ';') {
        next();
    } else if(mark == '{') {
        next();
        while(mark == '}') stmt();
        next();
    } else {
        expr(Assign);
    }
}

//程序
void params() {
    int ty;

    //先解析数据类型，后解析变量名或函数名
    next();
    while(mark) {
        ty = INT;
        //解析全局变量
        if(mark == Int) { next(); }
        else if(mark == Char) { ty = CHAR; next(); }
        else {}
        while(mark != ';' && mark != '{') {
            if(id[IdType] != Id) printf("标识符重复定义！\n");
            if(id[IdType]) printf("全局变量不可重复定义！\n");
            if(mark == '(') {
                ty = INT;
                //参数
                next();
                while(mark != ')') {
                    if(mark == Int) { next(); }
                    if(mark == Char) { ty=CHAR; next(); }
                    if(mark != Id) { printf("参数定义错误！"); }
                    id[IdType] = Loc;
                    //id[Value] = i;
                    next();
                }
                next();
                if(mark != '{') { printf("函数体定义错误！"); }
                while(mark != '}') stmt();
            } else {
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

    // 执行汇编代码
    while(1) {
        i = *pc++;
        ++cycle;         //CPU的指令周期，每循环一次，代表一条指令
        //if(debug) {
        if(1) {
            printf("%d> %.4s", cycle,
              &"LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,"
               "OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,"
               "OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT,"[i * 5]);
            if(i <= ADJ) printf(" %d\n", *pc); else printf("\n");
        }
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

        else if (i == OPEN) a = open((char *)sp[1], *sp);
        else if (i == READ) a = read(sp[2], (char *)sp[1], *sp);
        else if (i == CLOS) a = close(*sp);
        else if (i == PRTF) { t = sp + pc[1]; a = printf((char *)t[-1], t[-2], t[-3], t[-4], t[-5], t[-6]); }
        else if (i == MALC) a = (int)malloc(*sp);
        else if (i == FREE) free((void *)*sp);
        else if (i == MSET) a = (int)memset((char *)sp[2], sp[1], *sp);
        else if (i == MCMP) a = memcmp((char *)sp[2], (char *)sp[1], *sp);
        else if (i == EXIT) { printf("exit(%d) cycle = %d\n", *sp, cycle); return; }
        else { printf("unknown instruction = %d! cycle = %d\n", i, cycle); return; }
    }
}

int main(int argc, char *argv) {
    char *fname ="./hello.c";
    int fp, result;
    int i, msize = 256 * 1024;

    //分配内存
    src = malloc(msize);
    data = malloc(msize);
    text = malloc(msize);
    symbol = malloc(msize);
    lsp = sp   = malloc(msize);

    fp = open(fname, O_RDONLY);
    if(fp < 0) {
        printf("不能打开文件！");
        exit(1);
    }
    result = read(fp, src, msize);
    if(result) {
        printf("文件: %d", result);
    }

    //将关键字加入符号表
    src = "char printf exit void main";
    for(i = Char; i <= Return; i++) { next(); id[Mark] = i; }
    for(i = PRTF; i <= EXIT; i++)   { next(); id[IdType] = Sys; id[DataType] = Int; id[Value] = i; }
    next(); id[Value] = CHAR;
    next(); idmain = id;

    //执行程序解析
    params();

    //执行程序
    eval();
    printf("%d-%d-%d-%d-%d-%p-%d-%d", idmain[0], idmain[1], idmain[2], idmain[3], idmain[4], idmain[5], idmain[6], idmain[7]);

    printf("end! \n");
    return 0;
}

