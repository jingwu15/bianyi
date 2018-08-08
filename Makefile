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
	@gcc wc.c -o ./bin/wchar
	@./wchar

clean:
	@rm -rf ./*.bin ./bin/* *.out

