all: assignment-1.01
	make assignment-1.01

assignment-1.01: assignment-1.01.c
	gcc -Wall -Werror -ggdb3 assignment-1.01.c -o assignment-1.01

clean: 
	rm assignment-1.01 
	rm assignment-1.01.stackdump