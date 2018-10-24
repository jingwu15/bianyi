//gcc -m32 机编-0.c -o 机编
//./机编

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <wchar.h>
#include <locale.h>

//字符串对对
int strEq(char *raw0, char *raw1) {
    printf("%p-%p\n", raw0, raw1);
    //int length = 0, flag = 1;
    int flag = 1;
    while(1) {
        //length++;
        if(*raw0 && *raw1) {
            if(*raw0 != *raw1) { flag = 0; break; }
            raw0++;
            raw1++;
        } else if(*raw0 && !*raw1) {
            flag = 0; break;
        } else if(!*raw0 && *raw1) {
            flag = 0; break;
        } else {
            break;
        }
    }
    //while(length--) raw0--;
    ////raw0 = raw0 - length - 1;
    ////raw1 = raw1 - length - 1;
    //printf("raw0-end:%s\n", raw0);
    return flag;
}

int strCopy(char *raw, char *dest, int length) {
    int i = 0;
    while(i < length) {
        *raw++ = *dest++;
        i++;
    }
    return 1;
}

int main(int argc, char * argv) {
    char *start = "ldr r1 ,=0x1111100000";
    if(strEq(start, "ldr") == 0) {
        printf("1--cmd-ldr: \n");
    }
    if(strEq(start, "ldr r1") == 0) {
        printf("1--cmd-ldr r1: \n");
    }
}

