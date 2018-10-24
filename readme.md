### 自写编辑器

```
用的是 exynos4412/itop4412 板子，基于 ARM Cortex -A9 处理器，用的是 ARMv7-A1 指令集
ARM 文档：http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.home/index.html
ARM 体系结构 -> Reference Manuals -> ARMv7-AR Reference Manual -> ARM® Architecture Reference Manual ARMv7-A and ARMv7-R edition


https://blog.csdn.net/baidu_37973494/article/details/82796800
https://www.cnblogs.com/getyoulove/p/3675748.html
https://blog.csdn.net/sunnylgz/article/details/8049116
https://blog.csdn.net/u010245383/article/details/25919757
```


gcc -m32 c4.c -o c4.bin

./c4.bin hello.c


```
00000000: 00010000 00010000 10011111 11100101
00000004: 00000000 00000000 10010001 11100101
00000008: 00001111 00000000 11000000 11100011
0000000c: 00000010 00000000 10000000 11100011
00000010: 00000000 00000000 10000001 11100101
00000014: 11111110 11111111 11111111 11101010
00000018: 10100000 00000000 01000000 00010001


00000000: 1010 9fe5
00000004: 0000 91e5
00000008: 0f00 c0e3
0000000c: 0200 80e3
00000010: 0000 81e5
00000014: feff ffea
00000018: a000 4011

    ldr r1,=0x114000A0        @准备地址
    ldr r0,[r1]               @读取片上外设寄存器数据 到cpu通用寄存 器
    bic r0,r0,#0xf            @清零要修改位
    orr r0,r0,#0x2            @或修改值 0x2设置输出模式
    str r0,[r1]               @将数据 写回到片上外设
```



查看二进制文件
```
od -w4 -tx2 -v 代码-0/cmd_ldr.bin
od --endian=little -w4 -tx2 -v 代码-0/cmd_ldr.bin
```


源码
```
.text

.globl _start
_start:
    mov r1, #0x64
```

Makefile文件
```
代码0:
    arm-none-linux-gnueabi-gcc -c -g ./代码0-mov/mov.s -o ./代码0-mov/mov.o
    arm-none-linux-gnueabi-ld  -Ttext 0x40008000 ./代码0-mov/mov.o -o ./代码0-mov/mov.elf
    arm-none-linux-gnueabi-objcopy -O binary ./代码0-mov/mov.elf  ./代码0-mov/mov.bin
    arm-none-linux-gnueabi-objdump -D ./代码0-mov/mov.elf > ./代码0-mov/mov.dis
```

查看 ./代码0-mov/mov.dis
```
./代码0-mov/mov.elf:     file format elf32-littlearm

Disassembly of section .text:

40008000 <_start>:
40008000:   e3a01064    mov r1, #100    ; 0x64

Disassembly of section .ARM.attributes:
......
```

查看 ./代码0-mov/mov.bin
od -w4 -tx2 -v 代码0-mov/mov.bin
```
0000000 1064 e3a0
00000
```

汇编指令与二进制指令映射(小端编码)
```
汇编指令: mov r1, #0x64                        二进制指令: E3A01064

指令码(16位)                   操作数(16位)
cond 00 常数 opcode S SBZ      Rd   shifer_operand
1110 00 1    1101   0 0000     0001 0000 0110 0100
E    3        A       0        1    0    6    4

注：二进制文件中的指令码 与 dis文件中的指令码 前后16位是颠倒的, 文档及教料书的指令码与dis 文件相同
dis 文件中指令码: 40008000:   e3a0 1064    mov r1, #100    ; 0x64
二进制文件指令码: 0000000     1064 e3a0
```


```
代码-0    演示对比汇编指令与二进制码指令
代码-1    演示最基本的蜂鸣器
```


### 机器码每个位的意义
```
P
W
U
Rt
Rd
op1
op
Rn
```







### 汇编器
```
1. 将所有的伪指令处理为真正的指令
2. 转换指令为机器码
```

