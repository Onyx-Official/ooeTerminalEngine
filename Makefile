CC = gcc
CC_FILES = *.c
CC_FLAGS = -Wall -Wextra -pedantic -std=c17 -lpthread -fsanitize=address #-laio -lrt

EXEC = ooe
BUILD_CMD = $(CC) $(CC_FILES) -o $(EXEC) $(CC_FLAGS)

build:
	$(BUILD_CMD)

###terminal_engine: main.o
#	gcc -lpthread -o $@ $^
#	rm *.o 
#main.o: main.c
#	gcc -c -o $@ $^ -Wall -W -Wextra -O3 -pedantic -std=c11	-lpthread###