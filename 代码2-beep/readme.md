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
代码1:
    arm-none-linux-gnueabi-gcc -c -g ./代码1-beep/start.s -o ./代码1-beep/start.o
    arm-none-linux-gnueabi-ld  -Ttext 0x40008000 ./代码1-beep/start.o -o ./代码1-beep/start.elf
    arm-none-linux-gnueabi-objcopy -O binary ./代码1-beep/start.elf  ./代码1-beep/start.bin
    arm-none-linux-gnueabi-objdump -D ./代码1-beep/start.elf > ./代码1-beep/start.dis
```

查看 ./代码1-beep/start.dis
```
./代码1-beep/start.elf:     file format elf32-littlearm

Disassembly of section .text:

40008000 <_start>:
40008000:   e3a01064    mov r1, #100    ; 0x64

Disassembly of section .ARM.attributes:
......
```

查看 ./代码1-beep/start.bin
od -w4 -tx2 -v 代码1-beep/cmd_ldr.bin
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

40008000:	e59f1014 	ldr	r1, [pc, #20]	; 4000801c <_loop+0x4>
e    5    9    f    1    0    1    4 	ldr	r1, [pc, #20]	; 4000801c <_loop+0x4>
1110 0101 1001 1111 0001 0000 0001 0100
cond 010P U0W1 Rn   Rt   imm12
if P == W then UNPREDICTABLE; (un predictable)
t = UInt(Rt); imm32 = ZeroExtend(imm12, 32); add = (U == ‘1’);

40008004:	e5910000 	ldr	r0, [r1]
e    5    9    1    0    0    0    0 	ldr	r0, [r1]
1110 0101 1001 0001 0000 0000 0000 0000
cond 010P U0W1 Rn   Rt   imm12
if P == ‘0’ && W == ‘1’ then SEE LDRT;

40008008:	e3c0000f 	bic	r0, r0, #15
e    3    c    0    0    0    0    f 	bic	r0, r0, #15
1110 0011 1100 0000 0000 0000 0000 1111
cond 0011 110S Rn   Rd   imm12 
d = UInt(Rd); n = UInt(Rn); setflags = (S == ‘1’);
(imm32, carry) = ARMExpandImm_C(imm12, APSR.C);

4000800c:	e3800002 	orr	r0, r0, #2
e    3    8    0    0    0    0    2 	orr	r0, r0, #2
1110 0011 1000 0000 0000 0000 0000 0010
cond 0011 100S Rn   Rd   imm12


40008010:	e5810000 	str	r0, [r1]
e    5    8    1    0    0    0    0 	str	r0, [r1]
1110 0101 1000 0001 0000 0000 0000 0000
cond 010P U0W0 Rn   Rt   imm12


40008018 <_loop>:
40008018:	eafffffe 	b	40008018 <_loop>
4000801c:	114000a0 	smlaltbne	r0, r0, r0, r0	; <UNPREDICTABLE>

