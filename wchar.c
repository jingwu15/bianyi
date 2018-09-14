#include <stdio.h>
#include <wchar.h>
#include <locale.h>
int main() {
    setlocale(LC_ALL, "zh_CN.UTF-8");
    FILE *fp;
    wchar_t buff[1000], *wc;
    //fp = fopen("./测试.txt", "r");
    //fp = fopen("./机编-示例-beep.s", "r");
    fp = fopen("./机编-示例-蜂鸣器.机", "r");
    if(fp == NULL) {
        printf("error");
    } else {
        fgetws(buff, 1000, fp); //这里换成fgetws()
        wprintf(L"%ls\n",buff);
        wc = buff;

        while(*wc) {
            if(*wc == L'取') {
                wprintf(L"正确匹配到字符: %lc\n", *wc);
            } else {
                wprintf(L"%lc\n", *wc);
            }
            wc++;
        }
    }
    return 0;
}
