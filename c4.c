// c4.c - 4个函数实现C编辑器 
// char/int/指针
// if/while/return/表达式
// 仅实现了满足自编译的功能

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>

char *p, *lp, // 源码的当前位置
     *data;   // data/bss 段指针

int *e, *le,  // 汇编中的文本段代码      .text      主要是函数
    *id,      // 当前解析的id    id是变量或函数属性
    *sym,     // 符号表(简单的id列表)
    tk,       // 当前标识符
    ival,     // 当前标识符的值
    ty,       // 当前表达式类型
    loc,      // 本地变量偏移
    line,     // 当前行号
    src,      // 输出源码和汇编标识
    debug;    // 输出执行指令
int flag = 0;

// 标识符和类(操作符优先级)
//Num[数字]    Fun[函数]  Sys[系统函数] Glo[全局]    Loc[局部]   Id[标识符]
//Char[字符]   Else[㫘则] Enum[枚举]    If[如果]     Int[整数]   Return[返回] While[循环]
//Assign[分配] Cond[条件] Lor[或]       Lan[与]      Or[或/汇编] Xor[异或]    And[与/汇编]
//Eq[相等]     Ne[不等]   Lt[小于等于]  Gt[大于等于] Le[小于]    Ge[大于]     Shl[左移]
//Shr[右移]    Add[加]    Sub[减少]     Mul[乘]      Div[除]     Mod[余]      Inc[自增]
//Dec[自减]    Brak[数组]
enum {
    Num = 128, Fun, Sys, Glo, Loc, Id,
    Char, Else, Enum, If, Int, Return, Sizeof, While,
    Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak           //Assign 对应的是运算操作，可转换成汇编
};

// 操作码（汇编）
// LEA[去局部变量地址,以pc+1地址,以bp为基址,**地址**载入累加器a]                       IMM[pc+1作为为立即数载入累加器a] 
// JMP[无条件跳转到pc+1] JSR[进入子程序,将pc+2入栈作为返回地址,跳转到pc+1]             BZ[累加器为零时分支,当累加器a为0时跳转到[PC+1],不为零时跳转到PC+2继续执行]  
// BNZ[累加器不为零时分支,当累加器a不为0时跳转到[PC+1],为零时跳转到pc+2]               ENT[进入子程序,将bp压栈,基址bp指向栈顶,然后将栈顶生长pc+1字,作为参数传递空间] 
// ADJ[用于子函数的堆栈，ADJ后跟的参数是指令的长度，也是堆栈的长度]                    LEV[离开子程序,堆栈指针sp = bp,从堆栈中弹出基址bp,pc] 
// LI[以a为地址取int数]                                                                LC[以a为地址取char] 
// SI[以栈顶为地址存int数并弹栈sp++=a]                                                 SC[以栈顶为地址存char并弹栈sp++=a] 
// PSH[将a压栈]
// OR[或] XOR[异或] AND[与] EQ[相等] NE[不等] LT[小于等于] GT[大于等于] LE[小于] GE[大于] SHL[左移] SHR[右移] ADD[加] SUB[减] MUL[乖] DIV[除] MOD[余]
// OPEN[open] READ[read] CLOS[close] PRTF[printf] MALC[malloc] FREE[free] MSET[memset] MCMP[memcmp] EXIT[exit]
enum { LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,
       OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
       OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT };

// 类型
enum { CHAR, INT, PTR };

// id的结构，enum 类型，模拟数组
// Tk[标识符] Hash[哈唏值] Name[名称/前四个字符] Class[标识符类型] Type[数据类型] Val[标识符值] HClass[同Class] HType[用Type] HVal[用Val] Idsz[Id的长度]
enum { Tk, Hash, Name, Class, Type, Val, HClass, HType, HVal, Idsz };

// 调试用的函数，用于将类型转换为字符串输出
char * tyToStr(int type) {
    if(type == CHAR) return "char";
    if(type ==  INT) return "int";
    if(type ==  PTR) return "ptr";
    return "";
}

// 调试用的函数，用于将标识符转换为字符串输出
char * tkToStr(int tk) {
    if(tk ==    Num) return "num";
    if(tk ==    Fun) return "fun";
    if(tk ==    Sys) return "sys";
    if(tk ==    Glo) return "glo";
    if(tk ==    Loc) return "loc";
    if(tk ==     Id) return "id";
    if(tk ==   Char) return "char";
    if(tk ==   Else) return "else";
    if(tk ==   Enum) return "enum";
    if(tk ==     If) return "if";
    if(tk ==    Int) return "int";
    if(tk == Return) return "return";
    if(tk == Sizeof) return "sizeof";
    if(tk ==  While) return "while";
    if(tk == Assign) return "assign";
    if(tk ==   Cond) return "cond";
    if(tk ==    Lor) return "lor";
    if(tk ==     Or) return "or";
    if(tk ==    Xor) return "xor";
    if(tk ==    And) return "and";
    if(tk ==     Eq) return "eq";
    if(tk ==     Ne) return "ne";
    if(tk ==     Lt) return "lt";
    if(tk ==     Le) return "le";
    if(tk ==     Ge) return "ge";
    if(tk ==    Shl) return "shl";
    if(tk ==    Shr) return "shr";
    if(tk ==    Add) return "add";
    if(tk ==    Sub) return "sub";
    if(tk ==    Mul) return "mul";
    if(tk ==    Div) return "div";
    if(tk ==    Mod) return "mod";
    if(tk ==    Inc) return "inc";
    if(tk ==    Dec) return "dec";
    if(tk ==   Brak) return "brak";
    return "";
}

// 解析标识符
// 识别出变量名/函数名, 作为id，供后面使用，并 计算 hash 值 
// 识别出立即数，存入ival，以备记录的 id 的 val 中
// 识别出操作符(+-*/%^|=,==)，供后面使用
// 代码是有一定顺序，就是按照这个顺序解析
void next() {
    char *pp;

    while(tk = *p) {
        ++p;
        if(tk == '\n') {
            if(src) {
                printf("%d: %.*s", line, p - lp, lp);
                lp = p;
                while (le < e) {
                    printf("%8.4s", &"LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,"
                                     "OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,"
                                     "OPEN,READ,CLOS,RTF,MALC,FREE,MSET,MCMP,EXIT,"[*++le * 5]);
                    if (*le <= ADJ) printf(" %d\n", *++le); else printf("\n");
                }
            }
            ++line;
        }
        else if (tk == '#') {
            while (*p != 0 && *p != '\n') ++p;
        }
        else if ((tk >= 'a' && tk <= 'z') || (tk >= 'A' && tk <= 'Z') || tk == '_') {
            pp = p - 1;
            //tk的hash算法
            while ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') || *p == '_')
                tk = tk * 147 + *p++;
            tk = (tk << 6) + (p - pp);
            id = sym;
            while (id[Tk]) {
                if (tk == id[Hash] && !memcmp((char *)id[Name], pp, p - pp)) { tk = id[Tk]; return; }
                id = id + Idsz;
            }
            id[Name] = (int)pp;
            id[Hash] = tk;
            tk = id[Tk] = Id;
            return;
        }
        else if (tk >= '0' && tk <= '9') {
            if (ival = tk - '0') { while (*p >= '0' && *p <= '9') ival = ival * 10 + *p++ - '0'; }
            else if (*p == 'x' || *p == 'X') {
                while ((tk = *++p) && ((tk >= '0' && tk <= '9') || (tk >= 'a' && tk <= 'f') || (tk >= 'A' && tk <= 'F')))
                    ival = ival * 16 + (tk & 15) + (tk >= 'A' ? 9 : 0);
            }
            else { while (*p >= '0' && *p <= '7') ival = ival * 8 + *p++ - '0'; }
            tk = Num; 
            return;
        }
        else if (tk == '/') {
            if (*p == '/') {
                ++p;
                while (*p != 0 && *p != '\n') ++p;
            }
            else {
                tk = Div;
                return;
            }
        }
        else if (tk == '\'' || tk == '"') {
            pp = data;
            while (*p != 0 && *p != tk) {
                if ((ival = *p++) == '\\') {
                    if ((ival = *p++) == 'n') ival = '\n';
                }
                if (tk == '"') *data++ = ival;
            }
            ++p;
            if (tk == '"') ival = (int)pp; else tk = Num;
            return;
        }
        else if (tk == '=') { if (*p == '=') { ++p; tk = Eq; } else tk = Assign; return; }
        else if (tk == '+') { if (*p == '+') { ++p; tk = Inc; } else tk = Add; return; }
        else if (tk == '-') { if (*p == '-') { ++p; tk = Dec; } else tk = Sub; return; }
        else if (tk == '!') { if (*p == '=') { ++p; tk = Ne; } return; }
        else if (tk == '<') { if (*p == '=') { ++p; tk = Le; } else if (*p == '<') { ++p; tk = Shl; } else tk = Lt; return; }
        else if (tk == '>') { if (*p == '=') { ++p; tk = Ge; } else if (*p == '>') { ++p; tk = Shr; } else tk = Gt; return; }
        else if (tk == '|') { if (*p == '|') { ++p; tk = Lor; } else tk = Or; return; }
        else if (tk == '&') { if (*p == '&') { ++p; tk = Lan; } else tk = And; return; }
        else if (tk == '^') { tk = Xor;  return; }
        else if (tk == '%') { tk = Mod;  return; }
        else if (tk == '*') { tk = Mul;  return; }
        else if (tk == '[') { tk = Brak; return; }
        else if (tk == '?') { tk = Cond; return; }
        else if (tk == '~' || tk == ';' || tk == '{' || tk == '}' || tk == '(' || tk == ')' || tk == ']' || tk == ',' || tk == ':') { return; }
    }
}

void expr(int lev) {
    int t, *d;

    //普通字符 -> 汇编码
    if (!tk) { printf("%d: unexpected eof in expression\n", line); exit(-1); }
    else if (tk == Num) { *++e = IMM; *++e = ival; next(); ty = INT; }
    else if (tk == '"') {
        *++e = IMM; *++e = ival; next();
        while (tk == '"') next();
        data = (char *)((int)data + sizeof(int) & -sizeof(int)); ty = PTR;
    }
    else if (tk == Sizeof) {
        next(); if (tk == '(') next(); else { printf("%d: open paren expected in sizeof\n", line); exit(-1); }
        ty = INT; if (tk == Int) next(); else if (tk == Char) { next(); ty = CHAR; }
        while (tk == Mul) { next(); ty = ty + PTR; }
        if (tk == ')') next(); else { printf("%d: close paren expected in sizeof\n", line); exit(-1); }
        *++e = IMM; *++e = (ty == CHAR) ? sizeof(char) : sizeof(int);
        ty = INT;
    }
    else if (tk == Id) {
        //sizeof(int);
        //printf("%d\n", 4);
        //int big = 10;
        d = id; next();
        if (tk == '(') {                                                                 //解析函数调用
            next();
            t = 0;
            while(tk != ')') { expr(Assign); *++e = PSH; ++t; if(tk == ',') next(); }    //将函数参数压栈
            next();
            if (d[Class] == Sys) *++e = d[Val];                                          //系统函数，d[Val] 的值就是函数对应的汇编码 即系统函数在enum结构体中整数值
            else if (d[Class] == Fun) { *++e = JSR; *++e = d[Val]; }                     //自定义函数，d[Val] 的值就是函数对应的汇编码 即
            else { printf("%d: bad function call\n", line); exit(-1); }                  //非函数调用
            if (t) { *++e = ADJ; *++e = t; }                                             //在 text段 中设置ADJ指令，用于子函数的堆栈，ADJ后跟的参数是指令的长度，也是堆栈的长度
            ty = d[Type];                                                                //将id的数据类型赋值给ty
        }
        else if (d[Class] == Num) { *++e = IMM; *++e = d[Val]; ty = INT; }
        else {
            if (d[Class] == Loc) { *++e = LEA; *++e = loc - d[Val]; }                    //局部变量
            else if (d[Class] == Glo) { *++e = IMM; *++e = d[Val]; }                     //全局变量
            else { printf("%d: undefined variable\n", line); exit(-1); }
            *++e = ((ty = d[Type]) == CHAR) ? LC : LI;                                   //设置变量对应的加载方式
        }
    }
    else if (tk == '(') {
        next();
        if (tk == Int || tk == Char) {
            t = (tk == Int) ? INT : CHAR; next();
            while (tk == Mul) { next(); t = t + PTR; }
            if (tk == ')') next(); else { printf("%d: bad cast\n", line); exit(-1); }
            expr(Inc);
            ty = t;
        }
        else {
            expr(Assign);
            if (tk == ')') next(); else { printf("%d: close paren expected\n", line); exit(-1); }
        }
    }
    else if (tk == Mul) {
        next(); expr(Inc);
        if (ty > INT) ty = ty - PTR; else { printf("%d: bad dereference\n", line); exit(-1); }
        *++e = (ty == CHAR) ? LC : LI;
    }
    else if (tk == And) {
        next(); expr(Inc);
        if (*e == LC || *e == LI) --e; else { printf("%d: bad address-of\n", line); exit(-1); }
        ty = ty + PTR;
    }
    else if (tk == '!') { next(); expr(Inc); *++e = PSH; *++e = IMM; *++e = 0; *++e = EQ; ty = INT; }
    else if (tk == '~') { next(); expr(Inc); *++e = PSH; *++e = IMM; *++e = -1; *++e = XOR; ty = INT; }
    else if (tk == Add) { next(); expr(Inc); ty = INT; }
    else if (tk == Sub) {
        next(); *++e = IMM;
        if (tk == Num) { *++e = -ival; next(); } else { *++e = -1; *++e = PSH; expr(Inc); *++e = MUL; }
        ty = INT;
    }
    else if (tk == Inc || tk == Dec) {
        t = tk; next(); expr(Inc);
        if (*e == LC) { *e = PSH; *++e = LC; }
        else if (*e == LI) { *e = PSH; *++e = LI; }
        else { printf("%d: bad lvalue in pre-increment\n", line); exit(-1); }
        *++e = PSH;
        *++e = IMM; *++e = (ty > PTR) ? sizeof(int) : sizeof(char);
        *++e = (t == Inc) ? ADD : SUB;
        *++e = (ty == CHAR) ? SC : SI;
    }
    else { printf("%d: bad expression\n", line); exit(-1); }

    //汇编码进栈
    while (tk >= lev) { //优先爬升/自上而下运算符优先
        t = ty;
        if (tk == Assign) {
            next();
            if (*e == LC || *e == LI) *e = PSH; else { printf("%d: bad lvalue in assignment\n", line); exit(-1); }
            expr(Assign); *++e = ((ty = t) == CHAR) ? SC : SI;
        }
        else if (tk == Cond) {
            next();
            *++e = BZ; d = ++e;
            expr(Assign);
            if (tk == ':') next(); else { printf("%d: conditional missing colon\n", line); exit(-1); }
            *d = (int)(e + 3); *++e = JMP; d = ++e;
            expr(Cond);
            *d = (int)(e + 1);
        }
        else if (tk == Lor) { next(); *++e = BNZ; d = ++e; expr(Lan); *d = (int)(e + 1); ty = INT; }
        else if (tk == Lan) { next(); *++e = BZ;  d = ++e; expr(Or);  *d = (int)(e + 1); ty = INT; }
        else if (tk == Or)  { next(); *++e = PSH; expr(Xor); *++e = OR;  ty = INT; }
        else if (tk == Xor) { next(); *++e = PSH; expr(And); *++e = XOR; ty = INT; }
        else if (tk == And) { next(); *++e = PSH; expr(Eq);  *++e = AND; ty = INT; }
        else if (tk == Eq)  { next(); *++e = PSH; expr(Lt);  *++e = EQ;  ty = INT; }
        else if (tk == Ne)  { next(); *++e = PSH; expr(Lt);  *++e = NE;  ty = INT; }
        else if (tk == Lt)  { next(); *++e = PSH; expr(Shl); *++e = LT;  ty = INT; }
        else if (tk == Gt)  { next(); *++e = PSH; expr(Shl); *++e = GT;  ty = INT; }
        else if (tk == Le)  { next(); *++e = PSH; expr(Shl); *++e = LE;  ty = INT; }
        else if (tk == Ge)  { next(); *++e = PSH; expr(Shl); *++e = GE;  ty = INT; }
        else if (tk == Shl) { next(); *++e = PSH; expr(Add); *++e = SHL; ty = INT; }
        else if (tk == Shr) { next(); *++e = PSH; expr(Add); *++e = SHR; ty = INT; }
        else if (tk == Add) {
            next(); *++e = PSH; expr(Mul);
            if ((ty = t) > PTR) { *++e = PSH; *++e = IMM; *++e = sizeof(int); *++e = MUL;  }
            *++e = ADD;
        }
        else if (tk == Sub) {
            next(); *++e = PSH; expr(Mul);
            if (t > PTR && t == ty) { *++e = SUB; *++e = PSH; *++e = IMM; *++e = sizeof(int); *++e = DIV; ty = INT; }
            else if ((ty = t) > PTR) { *++e = PSH; *++e = IMM; *++e = sizeof(int); *++e = MUL; *++e = SUB; }
            else *++e = SUB;
        }
        else if (tk == Mul) { next(); *++e = PSH; expr(Inc); *++e = MUL; ty = INT; }
        else if (tk == Div) { next(); *++e = PSH; expr(Inc); *++e = DIV; ty = INT; }
        else if (tk == Mod) { next(); *++e = PSH; expr(Inc); *++e = MOD; ty = INT; }
        else if (tk == Inc || tk == Dec) {
            if (*e == LC) { *e = PSH; *++e = LC; }
            else if (*e == LI) { *e = PSH; *++e = LI; }
            else { printf("%d: bad lvalue in post-increment\n", line); exit(-1); }
            *++e = PSH; *++e = IMM; *++e = (ty > PTR) ? sizeof(int) : sizeof(char);
            *++e = (tk == Inc) ? ADD : SUB;
            *++e = (ty == CHAR) ? SC : SI;
            *++e = PSH; *++e = IMM; *++e = (ty > PTR) ? sizeof(int) : sizeof(char);
            *++e = (tk == Inc) ? SUB : ADD;
            next();
        }
        else if (tk == Brak) {
            next(); *++e = PSH; expr(Assign);
            if (tk == ']') next(); else { printf("%d: close bracket expected\n", line); exit(-1); }
            if (t > PTR) { *++e = PSH; *++e = IMM; *++e = sizeof(int); *++e = MUL;  }
            else if (t < PTR) { printf("%d: pointer type expected\n", line); exit(-1); }
            *++e = ADD;
            *++e = ((ty = t - PTR) == CHAR) ? LC : LI;
        }
        else { printf("%d: compiler error tk=%d\n", line, tk); exit(-1); }
    }
}

void stmt() {
    int *a, *b;

    if (tk == If) {
        next();
        if (tk == '(') next(); else { printf("%d: open paren expected\n", line); exit(-1); }
        expr(Assign);
        if (tk == ')') next(); else { printf("%d: close paren expected\n", line); exit(-1); }
        *++e = BZ; b = ++e;
        stmt();
        if (tk == Else) {
            *b = (int)(e + 3); *++e = JMP; b = ++e;
            next();
            stmt();
        }
        *b = (int)(e + 1);
    }
    else if (tk == While) {
        next();
        a = e + 1;
        if (tk == '(') next(); else { printf("%d: open paren expected\n", line); exit(-1); }
        expr(Assign);
        if (tk == ')') next(); else { printf("%d: close paren expected\n", line); exit(-1); }
        *++e = BZ; b = ++e;
        stmt();
        *++e = JMP; *++e = (int)a;
        *b = (int)(e + 1);
    }
    else if (tk == Return) {
        next();
        if (tk != ';') expr(Assign);
        *++e = LEV;
        if (tk == ';') next(); else { printf("%d: semicolon expected\n", line); exit(-1); }
    }
    else if (tk == '{') {
        next();
        while (tk != '}') stmt();
        next();
    }
    else if (tk == ';') {
        next();
    }
    else {
        expr(Assign);
        if (tk == ';') next(); else { printf("%d: semicolon expected\n", line); exit(-1); }
    }
}

int main(int argc, char **argv) {
    int fd, bt, ty, poolsz, *idmain;
    int *pc, *sp, *bp, a, cycle; // 虚拟机寄存器
    int i, *t; // 临时变量

    --argc; ++argv;
    if (argc > 0 && **argv == '-' && (*argv)[1] == 's') { src = 1; --argc; ++argv; }
    if (argc > 0 && **argv == '-' && (*argv)[1] == 'd') { debug = 1; --argc; ++argv; }
    if (argc < 1) { printf("usage: c4 [-s] [-d] file ...\n"); return -1; }

    if ((fd = open(*argv, 0)) < 0) { printf("could not open(%s)\n", *argv); return -1; }

    poolsz = 256*1024; //编译器内存 
    if (!(sym    = malloc(poolsz))) { printf("could not malloc(%d) symbol area\n", poolsz); return -1; }
    if (!(le = e = malloc(poolsz))) { printf("could not malloc(%d) text   area\n", poolsz); return -1; }
    if (!(data   = malloc(poolsz))) { printf("could not malloc(%d) data   area\n", poolsz); return -1; }
    if (!(sp     = malloc(poolsz))) { printf("could not malloc(%d) stack  area\n", poolsz); return -1; }

    memset(sym,  0, poolsz);
    memset(e,    0, poolsz);
    memset(data, 0, poolsz);
    int *lsym = sym;

    p = "char else enum if int return sizeof while "
        "open read close printf malloc free memset memcmp exit void main";
    i = Char; while (i <= While) { next(); id[Tk] = i++; } // 添加关键字到符号表
    i = OPEN; while (i <=  EXIT) { next(); id[Class] = Sys; id[Type] = INT; id[Val] = i++; } // 添加系统函数到符号表
    next(); id[Tk] = Char; // void 类型指针
    next(); idmain = id; // 保存程序入口

    if (!(lp = p = malloc(poolsz))) { printf("could not malloc(%d) source area\n", poolsz); return -1; }
    if ((i = read(fd, p, poolsz-1)) <= 0) { printf("read() returned %d\n", i); return -1; }
    p[i] = 0;
    close(fd);

    // 解析声明
    flag = 1;
    line = 1;
    next();
    //printf("\n--%d--%s--\n", tk, tk);
    //解析全局变量
    while(tk) {
        printf("--%c--\n", tk);
        bt = INT; // 基础类型
        if(tk == Int) next();
        else if(tk == Char) { next(); bt = CHAR; }
        else if(tk == Enum) {
            next();
            if(tk != '{') next();
            if(tk == '{') {
                next();
                i = 0;
                while (tk != '}') {
                    if (tk != Id) { printf("%d: bad enum identifier %d\n", line, tk); return -1; }
                    next();
                    if (tk == Assign) {
                        next();
                        if (tk != Num) { printf("%d: bad enum initializer\n", line); return -1; }
                        i = ival;
                        next();
                    }
                    id[Class] = Num; id[Type] = INT; id[Val] = i++;
                    if (tk == ',') next();
                }
                next();
            }
        }
        while(tk != ';' && tk != '}') {
            ty = bt;
            while (tk == Mul) { next(); ty = ty + PTR; }
            if(tk != Id) { printf("%d: bad global declaration\n", line); return -1; }
            if(id[Class]) { printf("%d: duplicate global definition\n", line); return -1; }
            next();
            id[Type] = ty;
            //在此之前定义的变量都是全局变量
            if(tk == '(') { // id后紧跟(是函数定义
                id[Class] = Fun;
                id[Val] = (int)(e + 1);        //定义函数的入口地址，这里也是e段的起始地址，main函数的入口地址
                next(); i = 0;
                //函数参数
                while (tk != ')') {
                    ty = INT;
                    if (tk == Int) next();
                    else if (tk == Char) { next(); ty = CHAR; }
                    while (tk == Mul) { next(); ty = ty + PTR; }
                    if (tk != Id) { printf("%d: bad parameter declaration\n", line); return -1; }
                    if (id[Class] == Loc) { printf("%d: duplicate parameter definition\n", line); return -1; }
                    //如果变量存在，且不是局部变量，则是全局变量，将全局变量的值放入Hxxx字段里
                    id[HClass] = id[Class]; id[Class] = Loc;
                    id[HType]  = id[Type];  id[Type] = ty;
                    id[HVal]   = id[Val];   id[Val] = i++;
                    next();
                    if(tk == ',') next();
                }
                next();
                //函数体
                if (tk != '{') { printf("%d: bad function definition\n", line); return -1; }
                loc = ++i;
                next();
                //函数体内首先定义的中变量，先解析这一部分变量
                while (tk == Int || tk == Char) {
                    bt = (tk == Int) ? INT : CHAR;
                    next();
                    while (tk != ';') {
                        ty = bt;
                        while (tk == Mul) { next(); ty = ty + PTR; }
                        if (tk != Id) { printf("%d: bad local declaration\n", line); return -1; }
                        //重复局部变量定义
                        if (id[Class] == Loc) { printf("%d: duplicate local definition\n", line); return -1; }
                        id[HClass] = id[Class]; id[Class] = Loc;
                        id[HType]  = id[Type];  id[Type] = ty;
                        id[HVal]   = id[Val];   id[Val] = ++i;
                        next();
                        if(tk == ',') next();
                    }
                    next();
                }
                *++e = ENT; *++e = i - loc;        // 设置进入函数指令，i-loc为函数范围，可以理解为多小个CPU指令
                while(tk != '}') stmt();           // 真正的函数体: if/else/while/return/{/}/;/表达式
                *++e = LEV;                        // 离开函数
                id = sym;
                // 取消局部变量声明, 函数处理完毕, 将全局变量恢复
                while (id[Tk]) {
                    if (id[Class] == Loc) {
                        id[Class] = id[HClass];
                        id[Type] = id[HType];
                        id[Val] = id[HVal];
                    }
                    id = id + Idsz;
                }
            }
            else {
                //全局变量
                id[Class] = Glo;
                id[Val] = (int)data;
                data = data + sizeof(int);
            }
            if(tk == ',') next();
        }
        next();
    }
    //int *start;
    //start = lsym;
    //for(i = 0; i < 100; i++) {
    //    printf("0[%s]-1[%d]-2[%s]-3[%s]-4[%d]-5[%s]-6[%s]-7[%s]-8[%p]-9[%d]\n", tkToStr(*(start+0)), *(start+1), (char *)(*(start+2)), tkToStr(*(start+3)), tyToStr(*(start+4)), start+5, tkToStr(*(start+6)), tyToStr(*(start+7)), start+8, *(start+9));
    //    start = start + 9;
    //}
    //return 0;

    //将 pc 指向 text段的 main函数的入口地址, 即 *e 指向的内存段
    if (!(pc = (int *)idmain[Val])) { printf("main() not defined\n"); return -1; }
    if (src) return 0;

    // 设置栈
    bp = sp = (int *)((int)sp + poolsz);
    *--sp = EXIT; // 如果main返回，调用exit
    *--sp = PSH; t = sp;
    *--sp = argc;
    *--sp = (int)argv;
    *--sp = (int)t;

    // 执行汇编代码
    cycle = 0;
    while(1) {
        i = *pc++;
        ++cycle;         //CPU的指令周期，每循环一次，代表一条指令
        if(debug) {
            printf("%d> %.4s", cycle,
              &"LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,"
               "OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,"
               "OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT,"[i * 5]);
            if(i <= ADJ) printf(" %d\n", *pc); else printf("\n");
        }
        if      (i == LEA) a = (int)(bp + *pc++);                             // 加载局部变量
        else if (i == IMM) a = *pc++;                                         // 加载全局变量或立即数
        else if (i == JMP) pc = (int *)*pc;                                   // 跳转
        else if (i == JSR) { *--sp = (int)(pc + 1); pc = (int *)*pc; }        // 跳转到子程序
        else if (i ==  BZ) pc = a ? pc + 1 : (int *)*pc;                      // 为零分支
        else if (i == BNZ) pc = a ? (int *)*pc : pc + 1;                      // 非零分支
        else if (i == ENT) { *--sp = (int)bp; bp = sp; sp = sp - *pc++; }     // 进入子程序
        else if (i == ADJ) sp = sp + *pc++;                                   // 马上进入子函数，设置堆栈，将栈顶指针加上子函数的长度
        else if (i == LEV) { sp = bp; bp = (int *)*sp++; pc = (int *)*sp++; } // 离开子程序
        else if (i ==  LI) a = *(int *)a;                                     // 加载整数
        else if (i ==  LC) a = *(char *)a;                                    // 加载字符
        else if (i ==  SI) *(int *)*sp++ = a;                                 // 存储整数
        else if (i ==  SC) a = *(char *)*sp++ = a;                            // 存储字符
        else if (i == PSH) *--sp = a;                                         // 压栈

        else if (i ==  OR) a = *sp++ |  a;
        else if (i == XOR) a = *sp++ ^  a;
        else if (i == AND) a = *sp++ &  a;
        else if (i ==  EQ) a = *sp++ == a;
        else if (i ==  NE) a = *sp++ != a;
        else if (i ==  LT) a = *sp++ <  a;
        else if (i ==  GT) a = *sp++ >  a;
        else if (i ==  LE) a = *sp++ <= a;
        else if (i ==  GE) a = *sp++ >= a;
        else if (i == SHL) a = *sp++ << a;
        else if (i == SHR) a = *sp++ >> a;
        else if (i == ADD) a = *sp++ +  a;
        else if (i == SUB) a = *sp++ -  a;
        else if (i == MUL) a = *sp++ *  a;
        else if (i == DIV) a = *sp++ /  a;
        else if (i == MOD) a = *sp++ %  a;

        else if (i == OPEN) a = open((char *)sp[1], *sp);
        else if (i == READ) a = read(sp[2], (char *)sp[1], *sp);
        else if (i == CLOS) a = close(*sp);
        else if (i == PRTF) { t = sp + pc[1]; a = printf((char *)t[-1], t[-2], t[-3], t[-4], t[-5], t[-6]); }
        else if (i == MALC) a = (int)malloc(*sp);
        else if (i == FREE) free((void *)*sp);
        else if (i == MSET) a = (int)memset((char *)sp[2], sp[1], *sp);
        else if (i == MCMP) a = memcmp((char *)sp[2], (char *)sp[1], *sp);
        else if (i == EXIT) { printf("exit(%d) cycle = %d\n", *sp, cycle); return *sp; }
        else { printf("unknown instruction = %d! cycle = %d\n", i, cycle); return -1; }
    }
}
