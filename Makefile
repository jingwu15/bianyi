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

clean:
	@rm -rf ./*.bin ./bin/* *.out

