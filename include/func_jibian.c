#include <stdio.h>
#include <wchar.h>
#include "jibian.h"

/**
 * 读取源代码
 * setlocale(LC_ALL, "zh_CN.UTF-8");
 * wchar_t code[10000];    int ret, codeLen;
 * ret = readCode("./机编-示例-蜂鸣器.机", code, &codeLen);
 * wprintf(L"正确匹配到字符: %ls--\ncodeLen: %d\n", code, codeLen);
 */
int readCode(const char *codefile, wchar_t *code, int *codeLen) {
    FILE *fp = fopen("./机编-示例-蜂鸣器.机", "r");
    if(fp == NULL) { printf("error"); return 1; }

    wchar_t tmp[10240], *ret, *p;
    *codeLen = 0;
    while(1) {
        ret = fgetws(tmp, 1000, fp); //这里换成fgetws()
        if(ret == NULL) break;
        p = tmp;
        while(*p) {
            code[*codeLen] = *p;
            p++;
            (*codeLen)++;
        }
    }
    return 0;
}

