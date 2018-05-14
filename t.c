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

int main(int argc, char **argv) {
    char *p, *pt;
    char *t;
    p = malloc(1024);
    t = p;
    *p++ = 'n';
    *p++ = 'i';
    *p++ = 'h';
    *p++ = 'a';
    *p++ = 'o';
    //p = p - 5;
    printf("%s\n", p);
    printf("%s\n", t);
    pt = "char else enum if int return sizeof while "
    "open read close printf malloc free memset memcmp exit void main";
    printf("%s\n", pt);

    int poolsz = 256*1024;
    pt = malloc(poolsz);
    int fd = open("/data/code/jingwu/bianyi/hello.c", 0);
    int it = read(fd, pt, poolsz-1);
    printf("%d\n", it);
    if(it) {
        printf("%s\n", pt);
    }
    if(pt[it]) {
        printf("if\n");
    } else {
        printf("else\n");
    }
}

