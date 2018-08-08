#include <stdio.h>
#include <wchar.h>
#include <locale.h>
int main() {
    setlocale(LC_ALL, "zh_CN.UTF-8");
    FILE *fp;
    wchar_t buff[100], *wc;
    fp = fopen("./测试.txt", "r");
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
