.text

.globl _start
_start:
	@BEEP_CON  0x114000A0                GPD0.CON | (0x2 << 0)
	@BEEP_DAT  0x114000A4  
	@BEEP_PUD  0x114000A8
	@BEEP_DRV  0x114000AC
    ldr r1,=0x114000A0        @׼����ַ
    ldr r0,[r1]               @��ȡƬ������Ĵ������� ��cpuͨ�üĴ� ��
    bic r0,r0,#0xf            @����Ҫ�޸�λ
    orr r0,r0,#0x2            @���޸�ֵ 0x2�������ģʽ
    str r0,[r1]               @������ д�ص�Ƭ������

_stop:
	b _stop

_loop:
    b _loop


