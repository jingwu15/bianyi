
all:
	gcc -m32 c4.c -o c4.bin
	./c4.bin hello.c

test:
	gcc -m32 b.c -o b.bin
	./b.bin hello.c

clean:
	@rm -rf ./c4.bin ./b.bin

