#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <wchar.h>
#include <locale.h>

int codeSize = 1024 * 256;
char *daima, *daimaShi, *opcode, *opcodeShi;

//token
//enum{}

//ID
//type ID struct {
//    int hash         //hash
//    char *name
//    int scope,
//    int type,
//    int value,
//}
//ID *ids;

int getHash(char *raw, int length) {
    int i = 0, token = 0;
    while(i< length) {
        token = token * 147 + *raw++;
        i++;
    }
    token = (token << 6) + length;
    return token;
}

int matchId(char raw) {
    if((raw >= 'a' && raw <= 'z') || (raw >= 'A' && raw <= 'Z') || (raw >= '0' && raw <= '9') || raw == '_') {
        return 1;
    } else {
        return 0;
    }
}

int matchCmd(char raw) {
    if((raw >= 'a' && raw <= 'z') || (raw >= 'A' && raw <= 'Z')) {
        return 1;
    } else {
        return 0;
    }
}

int matchZimu(char raw) {
    if((raw >= 'a' && raw <= 'z') || (raw >= 'A' && raw <= 'Z')) {
        return 1;
    } else {
        return 0;
    }
}


//字符串对对
int strEq(char *raw0, char *raw1) {
    while(1) {
        if(*raw0 && *raw1) {
            if(*raw0 == *raw1) {
                raw0++;
                raw1++;
                continue;
            } else {
                return 0;
            }
        } else if(*raw0 && !*raw1) {
                return 0;
        } else if(!*raw0 && *raw1) {
                return 0;
        } else {
            break;
        }
    }
    return 1;
}

int strCopy(char *raw, char *dest, int length) {
    int i = 0;
    while(i < length) {
        *raw++ = *dest++;
        i++;
    }
    return 1;
}

void getToken() {
    int start, length, token;
    while(token = *daima) {
        daima++;
        if(*daima == '\r' || *daima == '\n') {
            daima++;
            continue;
        } else if (*daima == ' ') {
            daima++;
            continue;
        } else if (matchZimu(*daima)) {
            start = (int)daima - 1;
            while(matchCmd(*daima)) {
                daima++;
            }
            length = (int)daima - start;
            if(strEq((char *)start, "ldr")) {
                printf("ldr yi zhao dao!");
            }
            continue;
        } else if (*daima == '@') {
            while(*daima != '\r' || *daima != '\n') {
                daima++;
                continue;
            }
            continue;
        } else {
            printf("取 token 错误!");
            exit(-1);
        }
    }
}

void jiexiBiaoDaShi() {
}

void jiexiCode() {
    getToken();
}

int main(int argc, char * argv) {
    setlocale(LC_ALL, "zh_CN.UTF-8");
    FILE *fp = fopen("./代码-0.机", "r");
    if(fp == NULL) {
        printf("打开文件失败!");
        return -1;
    }
    int eizhi, i;
    char line[1024], *result, *start;
    daimaShi = daima = malloc(codeSize);
    opcodeShi = opcode = malloc(codeSize);
    while(1) {
        result = fgets(line, 1023, fp);
        if(result == NULL) {
            printf("读取文件失败!");
            //return -1;
        }
        start = line;
        while(*start) {
            *daima++ = *start++;
        }
    }
    //daima = daimaShi;

    //jiexiCode();
}

