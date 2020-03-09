SRC = src/isatomic.cpp
CC = gcc

isatomic: $(SRC)
	gcc -pthread -O0 -o isatomic $(SRC)

clean: 
	rm -f test.txt isatomic
