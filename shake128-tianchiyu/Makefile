CC = g++
C = -c
O = -o
VERSION = --std=c++11
FLAGS = -Wall -g
SOURCES = main.cpp keccak.cpp sponge.cpp keccak_p.cpp
OBJECTS = $(SOURCES:.cpp=.o)
OUTPUT = shake128
REMOVE = shake128 *.o

$(OUTPUT): $(OBJECTS)
	$(CC) $(FLAGS) $(VERSION) $(OBJECTS) -o $(OUTPUT)
main.o: main.cpp keccak.h
	$(CC) $(C) $(FLAGS) $(VERSION) main.cpp
keccak.o: keccak.cpp keccak.h
	$(CC) $(C) $(FLAGS) $(VERSION) keccak.cpp
sponge.o: sponge.cpp sponge.h
	$(CC) $(C) $(FLAGS) $(VERSION) sponge.cpp
keccak_p.o: keccak_p.cpp keccak_p.h
	$(CC) $(C) $(FLAGS) $(VERSION) keccak_p.cpp
.PHONY:
clean:
	@rm -rf $(REMOVE)
