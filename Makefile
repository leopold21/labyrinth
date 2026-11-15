CC = gcc
CFLAGS = -std=c17 -Wall -Wextra -Wpedantic
CPPFLAGS = -Iinclude

all: build/labyrinth 

build/labyrinth: build/main.o build/labyrinth.o build/interface.o build/memory.o build/monster.o 
	$(CC) build/main.o build/labyrinth.o build/interface.o build/memory.o build/monster.o -o build/labyrinth

build/main.o: src/main.c include/labyrinth.h
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c src/main.c -o build/main.o

build/labyrinth.o: src/labyrinth.c include/labyrinth.h
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c src/labyrinth.c -o build/labyrinth.o

build/interface.o: src/interface.c include/labyrinth.h 
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c src/interface.c -o build/interface.o

build/memory.o: src/memory.c include/labyrinth.h 
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c src/memory.c -o build/memory.o

build/monster.o: src/monster.c include/labyrinth.h
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c src/monster.c -o build/monster.o

build/tests/tests_labyrinth: tests/tests_labyrinth.c tests/minunit.h build/labyrinth.o build/interface.o build/memory.o build/monster.o
	mkdir -p build/tests
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/tests_labyrinth.c build/labyrinth.o build/interface.o build/memory.o build/monster.o -o build/tests/tests_labyrinth

.PHONY: all run test clean

run: build/labyrinth
	./build/labyrinth

test: build/tests/tests_labyrinth
	./build/tests/tests_labyrinth > build/tests/result_tests_labyrinth.txt
	
	
clean: 
	rm -rf build