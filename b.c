#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>

char *p;

//解析代码
void next() {
    printf("%c", *p);
    *p++;
}

int main(int argc, char **argv) {
    if(argc != 2) { printf("参数错误"); return -1; }
    ++argv;

    int fd, psz, c;
    psz = 256 * 1024;   //p分配的内存
    p = malloc(psz);
    fd = open(*argv, O_RDONLY);
    c = read(fd, p, psz - 1);
    while(*p) {
        next();
    }
}

