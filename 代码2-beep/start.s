.text

.globl _start
_start:
	@BEEP_CON  0x114000A0                GPD0.CON | (0x2 << 0)
	@BEEP_DAT  0x114000A4  
	@BEEP_PUD  0x114000A8
	@BEEP_DRV  0x114000AC
    ldr r1,=0x114000A0        @准备地址
    ldr r0,[r1]               @读取片上外设寄存器数据 到cpu通用寄存 器
    bic r0,r0,#0xf            @清零要修改位
    orr r0,r0,#0x2            @或修改值 0x2设置输出模式
    str r0,[r1]               @将数据 写回到片上外设

_stop:
	b _stop

_loop:
    b _loop


