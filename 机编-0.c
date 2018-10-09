//gcc -m32 机编-0.c -o 机编
//./机编

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <wchar.h>
#include <locale.h>

int codeSize = 1024 * 256, val;
char *daima, *daimaShi, *opcode, *opcodeShi;


//ID
//type ID struct {
//    int hash         //hash
//    char *name
//    int scope,
//    int type,
//    int value,
//}
//ID *ids;

enum TK {
    NUM = 128,
    EQ, 
    R0, R1, 
    LDR, BIC, ORR, STR
};

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

int matchShu(char raw) {
    if(raw >= '0' && raw <= '9') {
        return 1;
    } else {
        return 0;
    }
}

int cNumMap(char p) {
    if(p == '0') { return 0; }
    if(p == '1') { return 1; }
    if(p == '2') { return 2; }
    if(p == '3') { return 3; }
    if(p == '4') { return 4; }
    if(p == '5') { return 5; }
    if(p == '6') { return 6; }
    if(p == '7') { return 7; }
    if(p == '8') { return 8; }
    if(p == '9') { return 9; }
    if(p == 'a' || p == 'A') { return 10; }
    if(p == 'b' || p == 'B') { return 11; }
    if(p == 'c' || p == 'C') { return 12; }
    if(p == 'd' || p == 'D') { return 13; }
    if(p == 'e' || p == 'E') { return 14; }
    if(p == 'f' || p == 'F') { return 15; }
    return -1;
}

int str2Int(char *p, int from, int to) {
    int value = 0;
    if(from == 2) {
        while(*p == '0' || *p == '1') { value = value * 2 + cNumMap(*p); printf("--%d--\n", value); p++; }
    }
    if(from == 8) {
        while(*p >= '0' && *p <= '7') { value = value * 8 + cNumMap(*p); p++; }
    }
    if(from == 10) {
        while(*p >= '0' && *p <= '9') { value = value * 10 + cNumMap(*p); p++; }
    }
    if(from == 16) {
        while((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F')) { value = value * 16 + cNumMap(*p); p++; }
    }
    ///if(to == 2) {
    ///    if(from == 2) { return value; }
    ///}
    ///if(to == 8) {
    ///    if(from == 8) { return value; }
    ///}
    ///if(to == 10) {
    ///    if(from == 10) { return value; }
    ///}
    ///if(to == 16) {
    ///    if(from == 16) { return value; }
    ///}
    return value;
}

//字符串对对
int strEq(char *raw0, char *raw1) {
    while(1) {
        if(*raw0 && *raw1) {
            if(*raw0 != *raw1) return 0;
            raw0++;
            raw1++;
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

int getToken() {
    int start, length, token;
    while(token = *daima) {
        daima++;
        if(*daima == '\r' || *daima == '\n') {
            daima++;
            continue;
        } else if (*daima == ' ') {
            daima++;
            continue;
        } else if (*daima == ',') {
            token = *daima;
            return token;
        } else if (*daima == '=') {
            return EQ;
        } else if (*daima == '[') {
            token = *daima;
            return token;
        } else if (matchZimu(*daima)) {
            start = (int)daima - 1;
            while(matchCmd(*daima)) {
                daima++;
            }
            length = (int)daima - start;
            if(strEq((char *)start, "ldr") == 0) {
                //printf("cmd: ldr\n");
                return LDR;
            } else if(strEq((char *)start, "bic") == 0) {
                //printf("cmd: bic\n");
                return BIC;
            } else if(strEq((char *)start, "orr") == 0) {
                //printf("cmd: orr\n");
                return ORR;
            } else if(strEq((char *)start, "str") == 0) {
                //printf("cmd: str\n");
                return STR;
            } else if(strEq((char *)start, "r0") == 0) {
                return R0;
            } else if(strEq((char *)start, "r1") == 0) {
                return R1;
            } else {
                printf("cmd: other\n");
                exit(-1);
            }
            continue;
        } else if (matchShu(*daima)) {
            if(*(daima+1) == 'x' || *(daima+1) == 'x' ) {
                daima++;
                //16进制
                val = 111;
                printf("16进制\n");
                //str2Int(, 16, 10);
            } else {
                //8进制
                val = 111;
                printf("8进制\n");
                //str2Int(, 8, 10);
            }
            return NUM;
        } else if (*daima == '@') {
            while(*daima != '\r' || *daima != '\n') {
                daima++;
                continue;
            }
            continue;
        } else {
            printf("取 token 错误! %c", *daima);
            exit(-1);
        }
    }
    return 0;
}

void cmdLdr() {
    //ldr r1,=0x114000A0        @准备地址
    //ldr r0,[r1]               @读取片上外设寄存器数据 到cpu通用寄存器
    int reg = getToken();
    int separator = getToken();
    printf("cmdLdr--start--%d-%d\n", reg, separator);
    if(separator == ',') {
        int valType = getToken();
        if(valType == EQ) {
            //伪指令: ldr r1,=0x114000A0        @准备地址
            //*opcode = '';
            printf("EQ======\n");
            valType = getToken();
            if(valType != NUM) {
                printf("LDR 指令错误, 需要数字\n");
            }
            printf("LDR: %d\n", val);
            printf("伪指令: ldr r1,=0x114000A0 ----ldr r1, %d\n", valType);
        //} else if(valType == EQ_VAR) {
            //*opcode = '';
        } else {
            //异常, 格式错误
        }
    } else {
            //异常, 格式错误
    }
    printf("cmdLdr--end--%d-%d\n", reg, separator);
}

void cmdBic() {
    printf("cmdBic--start\n");
}

void cmdOrr() {
    printf("cmdOrr--start\n");
}

void cmdStr() {
    printf("cmdStr--start\n");
}

void jiexiCode() {
    int token;
    while(1) {
        token = getToken();
        if(token == LDR) {
            cmdLdr();
        } else if(token == BIC) {
            cmdBic();
        } else if(token == ORR) {
            cmdOrr();
        } else if(token == STR) {
            cmdStr();
        } else {
        }
    }
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
        result = fgets(line, 1000, fp);
        if(result == NULL) {
            break;
            printf("读取文件失败!");
            //return -1;
        }
        start = line;
        while(*start) {
            *daima++ = *start++;
        }
    }
    daima = daimaShi;

    jiexiCode();
}

