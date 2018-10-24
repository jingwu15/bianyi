#include <stdio.h>
#include <stdlib.h>

//size_t fread ( void * ptr, size_t size, size_t count, FILE * stream );
//其中，ptr：指向保存结果的指针；size：每个数据类型的大小；count：数据的个数；stream：文件指针
//函数返回读取数据的个数。

//size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream );
//其中，ptr：指向保存数据的指针；size：每个数据类型的大小；count：数据的个数；stream：文件指针
//函数返回写入数据的个数。

int main() {
    char *filename = "arm-beep.bin";
    //本代码仅测试ARM祼机下执行的二进制文件，用普通的C语言来生成
    //ARM的机器指令是定长的32位指令，与32位的整型长度正好一致，故可用整型数组来存储ARM指令
    //本测试用的是ARM的蜂鸣器代码，蜂鸣器祼机二进制代码(od -w4 -tx2 -v beep.bin)如下：
    //0000000 1014 e59f
    //0000004 0000 e591
    //0000010 000f e3c0
    //0000014 0002 e380
    //0000020 0000 e581
    //0000024 fffe eaff
    //0000030 fffe eaff
    //0000034 00a0 1140
    //0000040
    FILE *pFile;
    //转换为整数数组（每条指令的前后16位是颠倒的）如下：
    int rows[] = {
        0xe59f1014,
        0xe5910000,
        0xe3c0000f,
        0xe3800002,
        0xe5810000,
        0xeafffffe,
        0xeafffffe,
        0x114000a0
    };
    int rowSize = sizeof(rows[0]);
    int rowsLen = sizeof(rows)/rowSize;

    //printf("arr-len-%d-%d\n", rowSize, rowsLen);
    if((pFile = fopen(filename, "wb")) == NULL) {
        printf("cant open the file");
        exit(0);
    }
    //可以写多个连续的数据
    fwrite(rows , rowSize, rowsLen, pFile);
    fclose(pFile);

    //文件生成后，可用md5sum对比是否一致

    return 0;
}

