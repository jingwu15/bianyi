### 自写编辑器

```
用的是 exynos4412/itop4412 板子，基于 ARM Cortex -A9 处理器，用的是 ARMv7-A1 指令集
ARM 文档：http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.home/index.html
ARM 体系结构 -> Reference Manuals -> ARMv7-AR Reference Manual -> ARM® Architecture Reference Manual ARMv7-A and ARMv7-R edition
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
    arm-none-linux-gnueabi-gcc -c -g ./代码0-mov/start.s -o ./代码0-mov/start.o
    arm-none-linux-gnueabi-ld  -Ttext 0x40008000 ./代码0-mov/start.o -o ./代码0-mov/start.elf
    arm-none-linux-gnueabi-objcopy -O binary ./代码0-mov/start.elf  ./代码0-mov/start.bin
    arm-none-linux-gnueabi-objdump -D ./代码0-mov/start.elf > ./代码0-mov/start.dis
```

查看 ./代码0-mov/start.dis
```
./代码0-mov/start.elf:     file format elf32-littlearm

Disassembly of section .text:

40008000 <_start>:
40008000:   e3a01064    mov r1, #100    ; 0x64

Disassembly of section .ARM.attributes:
......
```

查看 ./代码0-mov/start.bin
od -w4 -tx2 -v 代码0-mov/start.bin
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

