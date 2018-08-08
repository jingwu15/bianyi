/*
自制编译器，目前仅支持 hello world

示例代码：
#include <stdio.h>

int main(int argc, char *argv) {
    printf("hello world!\n");
    return 0;
}

编译器设计如下：
关键字：char int return exit void main
数据类型：char int point
汇编代码：IMM PUSH ADJ LEV LC LI PRTF EXIT

语法：类C

内存段设计：
data：  常量段
code：  代码段
src：   源码段
fubiao: 符号表 
 */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>

//关键字
enum {Num = 128, Sys, Glo, Loc, Id,
    Char, Int, Return, Exit, Void, Main};
//数据类型
enum {CHAR, INT, POINT};
//汇编代码
enum {IMM, PUSH, ADJ, LEV, LC, LI, PRTF, EXIT};
//符号
enum {Fu, Haxi, Ming, IType, DType, Value, GIType, GDType, GValue};

char *src, *data, *ldata, *xianzai;

int *fu, fusize = 8, msize = 256 * 1024, dType, fuhao, haxi, *fubiao, *code;

void next() {
    printf("next");
    while(fuhao = *src) {
        src++;
        if(fuhao == '#') { while(*src != '\n') { src++; } src++; }
        else if((fuhao >= 'a' && fuhao <= 'z') || (fuhao >= 'A' && fuhao <= 'Z') || fuhao == '_') {
            xianzai = src - 1;

            haxi = fuhao;
            while((fuhao >= 'a' && fuhao <= 'z') || (fuhao >= 'A' && fuhao <= 'Z') || (fuhao >= '0' && fuhao <= '9') || fuhao == '_') {
                haxi = haxi * 147 + *src++;
            }
            haxi = haxi << 6 + src - xianzai;

            fu = fubiao;
            while(fu[Fu]) {
                if(fu[Haxi] == haxi && memcmp((char *)fu[Ming], xianzai, src-xianzai) == 0) {dType = fu[DType]; return; }
                fu = fu + 8;
            }
            fu[Haxi] = haxi;
            fu[Ming] = (int)xianzai;
            return;
        }
    }
    return ;
}

int main(int argc, char *argv) {
    int i = 0, fd;
    char *fname = "./hello.c";

    data = malloc(msize);
    code = malloc(msize);
    fubiao = malloc(msize);

    src = "char int return printf exit void main";
    for(i = Char; i <= Return; i++) {printf("test");}
    //for(i = Char; i <= Return; i++) {next(); fu[Value] = i;}
    //for(i = PRTF; i <= EXIT; i++)   {next(); fu[IType] = Sys; fu[Value] = i;}

    //src = malloc(msize);
    //fd = open(fname, 0);
    //read(fd, src, msize);
    //printf("%s-\n", *src);

    printf("end!\n");
}

