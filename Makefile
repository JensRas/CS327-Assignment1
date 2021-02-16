all: rlg327
	make rlg327

rlg327: rlg327.c
	gcc -Wall -Werror -ggdb3 rlg327.c -o rlg327

clean: 
	rm rlg327 
	rm rlg327.stackdump