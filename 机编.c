#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <wchar.h>
#include <locale.h>
#include "include/jibian.h"

//https://blog.csdn.net/yanghongchang_/article/details/25025799
//https://blog.csdn.net/gywtzh0889/article/details/71083459/
//http://www.sohu.com/a/130879450_464086                                       arm


//翻译9歩曲: 
//1. 定义好符号表,代码段,堆栈段,全局变量段, 未初始化段
//2. 规划好符号表: 类别,作用域,起始地址, 唯一标识符,数据类型, 唯一值, 哈唏算法, tk = tk * 147 + *p++; tk = (tk << 6) + (p - pp);
//3. 解析表达式
//4. 定好起始函数
//5. 解析Token
//6. 解析控制逻辑
//7. 转换


//ID,作用域,名称,地址,
int duan[10];
int codeLen, poolsz = 256 * 1024;
wchar_t *code, *lcode, *daima, c;

//标识符, 作用域, 标识符类别,哈唏值,数据类型,入口地址
enum biaoShiFu {}


//解析Token
wchar_t parseToken() {
    wchar_t *dangQian;
    while(*code) {
        dangQian = code;
        if(*code == L'@') {
            while(*code != L'\n') code++;
            code++;
        } else if(*code == L'.') {           //段地址
            code++;
            while(
                (*code >= L'a' && *code <= L'z')          //小写字母
                || (*code >= L'A' && *code <= L'Z')       //大写字母
                || (*code >= L'一' && *code <= L'龥')     //汉字 20902
                || *code == L'_'                          //下横线
            ) {
                code++; 
            }
            if((code - dangQian) > 32) { printf("%s", "标识符最长32字符"); exit(1); }
            if(*code != L" ") { printf("%s", "段标识符格式错误"); exit(1); }
            duan[1] = (int)malloc(poolsz);
            code++;
        } else if(
            (*(code+1) >= L'a' && *(code+1) <= L'z')         //小写字母
            || (*(code+1) >= L'A' && *(code+1) <= L'Z')      //大写字母
            || (*(code+1) >= L'一' && *(code+1) <= L'龥')    //汉字 20902
            || *(code+1) == L'_'                             //下横线
            || *(code+1) == L'-'                             //中横线
        ) {        //标号
            while(
                (*code >= L'a' && *code <= L'z')             //小写字母
                || (*code >= L'A' && *code <= L'Z')          //大写字母
                || (*code >= L'一' && *code <= L'龥')        //汉字 20902
                || *code == L'_'                             //下横线
                || *(code+1) == L'-'                         //中横线
            ) {
                code++; 
            }
            if((code - dangQian) > 32) { printf("%s", "标识符最长32字符"); exit(1); }
            if(*code != L":") { printf("%s", "标号符式错误"); exit(1); }
            type = "标号";


        } else if(*code == L' ') {
            code++;
        } else {
        }
    }
}

//解析控制程序
void parseCtl() {
}


void convert() {
}

int main(int argc, char * argv) {
    setlocale(LC_ALL, "zh_CN.UTF-8");
    int ret;
    wchar_t a = L'a', b = L'一', c = L'龥';
    printf("--%x--%x--%x\n", a, b, c);
    exit(0);

    if(!(lcode = code = malloc(poolsz))) { printf("could not malloc(%d) source area\n", poolsz); return -1; }
    ret = readCode("./代码-x.机", code, &codeLen);
    //wprintf(L"%ls--\ncodeLen: %d\n", code, codeLen);
    while(*code) {
        next();
        wprintf(L"%lc", *code);
        code++;
    }

    return 0;
}

