//gcc -m32 机编-0.c -o 机编
//./机编

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <wchar.h>
#include <locale.h>

int codeSize = 1024 * 256, val, opcode[10000], *opcodeShi;
char *daima, *daimaShi;


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

int matchWord(char raw) {
    if((raw >= 'a' && raw <= 'z') || (raw >= 'A' && raw <= 'Z') || (raw >= '0' && raw <= '9')) {
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
    //printf("strEq--%p-%c----%p-%c----\n", raw0, *raw0, raw1, *raw1);
    int flag = 1;
    while(1) {
        //printf("strEq--c--%c-%d----%c-%d\n", *raw0, *raw0, *raw1, *raw1);
        if(*raw1 == '\0') break;
        if(*raw0 && *raw1) {
            if(*raw0 != *raw1) { flag = 0; break; }
            raw0++; raw1++; continue;
        }
        if(*raw0 && !*raw1) { flag = 0; break; }
        if(!*raw0 && *raw1) { flag = 0; break; }
    }
    //printf("flag----%d\n", flag);
    //printf("%s----\n", raw0);
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

int getToken() {
    char *start;
    int length, token;
    while(1) {
        if(*daima == '\0') return 0;
        token = *daima;
        //printf("while---%c\n", *daima);
        if(*daima == '\r' || *daima == '\n') {
            daima++;
        } else if(*daima == ' ') {
            daima++;
        } else if(*daima == ',') {
            token = *daima;
            daima++;
            return token;
        } else if(*daima == '=') {
            token = EQ;
            daima++;
            return token;
        } else if(*daima == '[') {
            token = *daima;
            daima++;
            return token;
        } else if(*daima == ']') {
            token = *daima;
            daima++;
            return token;
        } else if(matchZimu(*daima)) {
            start = daima;
            while(matchWord(*daima)) daima++;
            //length = daima - start;
            if(strEq(start, "ldr") == 1) {
                //printf("cmd: ldr\n");
                //printf("start: \n", *(char *)start);
                return LDR;
            } else if(strEq(start, "bic") == 1) {
                //printf("cmd: bic------start  %s\n", start);
                //exit(-1);
                return BIC;
            } else if(strEq(start, "orr") == 1) {
                return ORR;
            } else if(strEq(start, "str") == 1) {
                return STR;
            } else if(strEq(start, "r0") == 1) {
                //printf("reg: r0\n");
                return R0;
            } else if(strEq(start, "r1") == 1) {
                //printf("reg: r1\n");
                return R1;
            } else {
                printf("cmd: other\n");
                exit(-1);
            }
        } else if(*daima == '#') {
            daima++;
            if(*daima == '0') {
                daima++;
                if(*daima == 'x' || *daima == 'X') {    //16进制
                    daima++;
                    while((*daima >= '0' && *daima <= '9') || (*daima >= 'a' && *daima <= 'f') || (*daima >= 'A' && *daima <= 'F')) { val = val * 16 + cNumMap(*daima); daima++; }
                } else {                    //8进制
                    while(*daima >= '0' && *daima <= '7') { val = val * 8 + cNumMap(*daima); daima++; }
                }
            } else {                        //10进制
                while(*daima >= '0' && *daima <= '9') { val = val * 10 + cNumMap(*daima); daima++; }
            }
            daima++;
            return NUM;
        } else if(matchShu(*daima)) {
            if(*daima == '0') {
                daima++;
                if(*daima == 'x' || *daima == 'X') {    //16进制
                    daima++;
                    while((*daima >= '0' && *daima <= '9') || (*daima >= 'a' && *daima <= 'f') || (*daima >= 'A' && *daima <= 'F')) { val = val * 16 + cNumMap(*daima); daima++; }
                } else {                    //8进制
                    while(*daima >= '0' && *daima <= '7') { val = val * 8 + cNumMap(*daima); daima++; }
                }
            } else {                        //10进制
                while(*daima >= '0' && *daima <= '9') { val = val * 10 + cNumMap(*daima); daima++; }
            }
            daima++;
            return NUM;
        } else if(*daima == '@') {
            //printf("%s\n", daima);
            //while(*daima != '\r' && *daima != '\n') { daima++; printf("while--@--%c--\n", *daima); }
            while(*daima != '\r' && *daima != '\n') daima++; 
            //printf("@@@@@ \n");
            //printf("%d\n", daima);
        } else {
            printf("取 token 错误! %d\n", *daima);
            exit(-1);
        }
    }
    return 0;
}

void cmdLdr() {
    int regDest, regOp, separator, end, valType;
    regDest   = getToken();
    separator = getToken();
    //printf("cmdLdr--start--%d-%d\n", regDest, separator);
    if(separator == ',') {
        valType = getToken();
        if(valType == EQ) {
            //伪指令: ldr r1,=0x114000A0        @准备地址
            valType = getToken();
            if(valType != NUM) {
                printf("LDR 指令错误, ldr r1,=imm 需要数字\n");
            }
            *opcode = ;
            printf("LDR: r1,=imm---------%d\n", val);
        } else if(valType == '[') {
            regOp = getToken();
            end  = getToken();
            printf("LDR: ldr r0,[r1]------%d----%d\n", regOp, val);
        } else {
            //异常, 格式错误
        }
    } else {
            //异常, 格式错误
    }
    //printf("cmdLdr--end--%d-%d\n", regDest, separator);
}

void cmdBic() {
    int regDest, regOp, separator, end, valType;
    regDest   = getToken();
    separator = getToken();
    //printf("cmdBic--start--%d-%d\n", regDest, separator);
    if(separator == ',') {
        //bic r0,r0,#0xf
        regOp = getToken();
        separator = getToken();
        if(separator == ',') {
            end = getToken();
            printf("BIC: bic r0,r0,#imm----%d----%d\n", regOp, end);
        } else {
            printf("BIC: bic r0,r0,#imm----#immerror\n");
        }
    } else {
        //异常, 格式错误
        printf("BIC: bic r0,r0,#imm----,format,error\n");
    }
    //printf("cmdBic--end--%d-%d\n", regDest, separator);
}

void cmdOrr() {
    int regDest, regOp, separator, end, valType;
    regDest   = getToken();
    separator = getToken();
    //printf("cmdORR--start--%d-%d\n", regDest, separator);
    if(separator == ',') {
        //orr r0,r0,#0x2
        regOp = getToken();
        separator = getToken();
        if(separator == ',') {
            end = getToken();
            printf("ORR: orr r0,r0,#imm----%d----%d\n", regOp, end);
        } else {
            printf("ORR: orr r0,r0,#imm----#immerror\n");
        }
    } else {
        //异常, 格式错误
        printf("ORR: orr r0,r0,#imm----,format,error\n");
    }
    //printf("cmdORR--end--%d-%d\n", regDest, separator);
}

void cmdStr() {
    int regDest, regOp, separator, end, valType;
    regDest   = getToken();
    separator = getToken();
    //printf("cmdStr--start--%d-%d\n", regDest, separator);
    if(separator == ',') {
        valType = getToken();
        if(valType == '[') {
            //str r0,[r1]
            regOp = getToken();
            end  = getToken();
            printf("STR: str r0,[r1]------%d----%c\n", regOp, end);
        } else {
            //异常, 格式错误
        }
    } else {
        //异常, 格式错误
    }
    //printf("cmdStr--end--%d-%d\n", regDest, separator);
}

void jiexiCode() {
    int token, i=0;
    while(1) {
        //printf("jiexiCode: %d\n", i++);
        token = getToken();
        //printf("cmd-token---%d\n", token);
        if(token == LDR) {
            cmdLdr();
        } else if(token == BIC) {
            cmdBic();
        } else if(token == ORR) {
            cmdOrr();
        } else if(token == STR) {
            cmdStr();
        } else if(token == 0) {
            break;
        } else {
            printf("其他命令: %d\n", token);
        }
    }
    printf("解析完成!\n");
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

