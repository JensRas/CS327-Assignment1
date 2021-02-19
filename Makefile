all: rlg327
	make rlg327

rlg327: rlg327.c
	gcc -Wall -Werror -ggdb3 rlg327.c -o rlg327

clean: 
	rm -f rlg327.exe
	rm -f rlg327.exe.stackdump
	#rm -f samples/.rlg327/dungeon