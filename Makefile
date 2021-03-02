all: rlg327
	make rlg327
#	make heap

rlg327: rlg327.c
	gcc -Wall -Werror -ggdb3 rlg327.c -o rlg327

#heap: heap.c
#	gcc -Wall -Werror -ggdb3 heap.c -o heap

clean: 
	rm -f rlg327.exe
	rm -f rlg327.exe.stackdump
	#rm -f samples/.rlg327/dungeon