/**
 * 实现思路：
 * 1. 读取源文件
 * 2. 解析代码
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>


int *e,*le,
    tk,
    hash,
    *id,
    *tk_table;

//关键字
//Num[数字]    Fun[函数]  Sys[系统函数] Glo[全局]    Loc[局部]   Id[标识符]
//Char[字符]   Else[㫘则] Enum[枚举]    If[如果]     Int[整数]   Return[返回] While[循环]
//Assign[分配] Cond[条件] Lor[或]       Lan[与]      Or[或/汇编] Xor[异或]    And[与/汇编]  
//Eq[相等]     Ne[不等]   Lt[小于等于]  Gt[大于等于] Le[小于]    Ge[大于]     Shl[左移]    
//Shr[右移]    Add[加]    Sub[减少]     Mul[乘]      Div[除]     Mod[余]      Inc[自增]  
//Dec[自减]    Brak[数组]
enum {
    TK_Num = 128, TK_Fun, TK_Sys, TK_Glo, TK_Loc, TK_Id,
    TK_Char, TK_Return, 
    TK_Assign, TK_Cond
};

//类型
enum { TY_CHAR };

int Idsz = 6;
enum { ID_Tk, ID_Hash, ID_Name, ID_Class, ID_Type, ID_Val };

char *p, *pp, *data;

char * tyToStr(int type) {
    if(type == TY_CHAR) return "char";
    return "";
}

char * tkToStr(int tk) {
    if(tk ==    TK_Num) return "num";
    if(tk ==    TK_Fun) return "fun";
    if(tk ==    TK_Sys) return "sys";
    if(tk ==    TK_Glo) return "glo";
    if(tk ==    TK_Loc) return "loc";
    if(tk ==    TK_Id ) return "id";
    if(tk ==   TK_Char) return "char";
    if(tk == TK_Return) return "return";
    if(tk == TK_Assign) return "assign";
    if(tk ==   TK_Cond) return "cond";
    return "";
}

void p_tktable() {
    int i, *start;
    start = tk_table;
    for(i = 0; i < 15; i++) {
        //printf("0[%s]-1[%d]-2[%s]-3[%s]-4[%d]-5[%s]\n", tkToStr(*(start+0)), *(start+1), (char *)((int)*(start+2)), tkToStr(*(start+3)), tyToStr(*(start+4)), start+5);
        //printf("0[%s]-1[%d]-2[%s]-3[%s]-4[%d]-5[%s]\n", tkToStr(start[0]), start[1], (char *)start[2], tkToStr(start[3]), tyToStr(start[4]), start[5]);
        printf("0[%s]-1[%d]-2[%s]-\n", tkToStr(start[0]), start[1], (char *)start[2]);
        start = start + Idsz;
    }
    return;
}

// 解析标识符
// 识别出变量名/函数名, 作为id，供后面使用，并 计算 hash 值 
// 识别出立即数，存入ival，以备记录的 id 的 val 中
// 识别出操作符(+-*/%^|=,==)，供后面使用
// 代码是有一定顺序，就是按照这个顺序解析
void next() {
    while(tk = *p) {
        ++p;
        if(tk == '#') { while(*p != '\n') { p++; } p++;   return ; }
        else if((tk >= 'a' && tk <= 'z') || (tk >= 'A' && tk <= 'Z') || tk == '_') {
            pp = p - 1;
            //函数或变量名
            hash = tk;
            id = tk_table;
            while((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_') hash = hash * 147 + *p++;
            hash = hash << 6 + p - pp;
            //判断域名或变量名是否存在，不存在则添加
            while(id[ID_Tk]) {
                if(hash == id[ID_Hash] && !memcmp((char *)id[ID_Name], pp, p - pp)) { tk = id[ID_Tk]; return; }
                id = id + Idsz;
            }
            //设置新的标识符
            id[ID_Name] = (int)pp;
            id[ID_Hash] = hash;
            tk = id[ID_Tk] = TK_Id;        // 重新标识tk为TK_Id
            printf("\n");
            return ;
        }
        else if(tk == '"') {
            char ival;
            int i,len=0;
            while(*p != '"') {
                ival = *p;
                if(*p == '\\' && *(p+1) == 'n') {
                    ival = '\n';
                    p++;
                }
                *data++ = ival;
                len++;
                p++;
            }
            for(i = 0; i < len; i++) {
                data--;
                printf("%c", data);
            }
            printf("\ndata--%d--%s--\n", len, data);
            p++;
            return ;
        }
    }
}

void expr() {

}

void stmt() {

}

int main(int argc, char **argv) {
    if(argc != 2) { printf("参数错误"); return -1; }
    ++argv;

    int i, fd, psz, c, base_type;
    psz = 256 * 1024;   //p分配的内存
    tk_table = malloc(psz);
    data = malloc(psz);
    // 分配符号表空间

    //将关键字加入符号表
    p = "char return printf exit void main";
    for(i = TK_Num; i <= TK_Cond; i++) { next(); id[ID_Val] = i;}

    p = malloc(psz);
    fd = open(*argv, O_RDONLY);
    c = read(fd, p, psz - 1);

    next();
    while(tk) {
        //基本的数据类型
        base_type = TY_CHAR;
        if(tk == TY_CHAR) next();

        //代码未结束
        if(tk != ';', tk != '}') {
            //解析函数参数
            if(tk == '(') {
                i = 0;
                while(tk != ')') {
                    //解析数据类型
                    if(tk == TK_Char) next();
                    //如果数据类型后面不是变量名，则错误
                    if(tk != TK_Id) { printf("错误的参数声明"); return -1; }

                    //根据类型，判断是否定义过
                    if(id[ID_Class] == TK_Loc) { printf("局部变量已经定义"); return -1; }
                    id[ID_Class] = TK_Loc;
                    id[ID_Type] = TY_CHAR;
                    id[ID_Val] = i++;         //参数的序号
                    if(tk == ',') next();
                }
                //---------- 函数参数解析完成----------------

                next();
                //解析函数体
                if(tk == '{') {
                    if(tk != '}') next();   continue;
                    stmt();
                }
            }
        }
    }
    p_tktable();
}

