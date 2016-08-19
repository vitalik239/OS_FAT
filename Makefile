CC = g++
CFLAGS =-g -Wall

all:
	$(CC) $(CFLAGS) general.cpp format.cpp -o format
	$(CC) $(CFLAGS) general.cpp mkdir.cpp -o mkdir
	$(CC) $(CFLAGS) general.cpp import.cpp -o import
	$(CC) $(CFLAGS) general.cpp ls.cpp -o ls
	$(CC) $(CFLAGS) general.cpp export.cpp -o export
	$(CC) $(CFLAGS) general.cpp rm.cpp -o rm
	$(CC) $(CFLAGS) general.cpp rmdir.cpp -o rmdir
	
run:
	rm -rf test
	mkdir test	
	./format test 10
	./mkdir test /dir1
	./mkdir test /dir2
	./mkdir test /dir1/dir3
	./ls test /
	./import test ./general.cpp /dir1/gen.cpp
	./ls test /dir1
	./export test /dir1/gen.cpp gen.cpp
	./rm test /dir1/gen.cpp
	./import test ./general.cpp /dir1/gen.cpp
	./export test /dir1/gen.cpp gen.cpp
	cmp gen.cpp general.cpp
	./rmdir test /dir1
	./ls test /

clean:
	rm -rf test gen.cpp	
	rm -rf *.o *.dSYM
	rm -rf format mkdir import rmdir ls export ls rm
