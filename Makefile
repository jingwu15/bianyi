all:
	@gcc -m32 c4.c -o ./bin/c4
	@./bin/c4 hello.c

test:
	@gcc -m32 b.c -o ./bin/b
	./bin/b

v1:
	@gcc -m32 b_v1.c -o ./bin/b_v1
	@./bin/b_v1

wc:
	@gcc wchar.c -o ./bin/wchar
	@./wchar

机编:
	@gcc 机编.c include/func_jibian.c -o ./bin/机编
	@./bin/机编

beep1:
	arm-none-linux-gnueabi-gcc -c -g beep.s -o beep.o
	arm-none-linux-gnueabi-ld  -Ttext 0x40008000 beep.o -o beep.elf
	arm-none-linux-gnueabi-objcopy -O binary beep.elf  beep.bin
	arm-none-linux-gnueabi-objdump -D beep.elf > beep.dis

代码0:
	arm-none-linux-gnueabi-gcc -c -g ./代码0-mov/start.s -o ./代码0-mov/start.o
	arm-none-linux-gnueabi-ld  -Ttext 0x40008000 ./代码0-mov/start.o -o ./代码0-mov/start.elf
	arm-none-linux-gnueabi-objcopy -O binary ./代码0-mov/start.elf  ./代码0-mov/start.bin
	arm-none-linux-gnueabi-objdump -D ./代码0-mov/start.elf > ./代码0-mov/start.dis

代码1:
	arm-none-linux-gnueabi-gcc -c -g ./代码1-beep/start.s -o ./代码1-beep/start.o
	arm-none-linux-gnueabi-ld  -Ttext 0x40008000 ./代码1-beep/start.o -o ./代码1-beep/start.elf
	arm-none-linux-gnueabi-objcopy -O binary ./代码1-beep/start.elf  ./代码1-beep/start.bin
	arm-none-linux-gnueabi-objdump -D ./代码1-beep/start.elf > ./代码1-beep/start.dis

clean:
	@rm -rf *.o */*.o *.elf */*.elf *.dis */*.dis *.bin */*.bin *.out

