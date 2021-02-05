CC = g++
C = -c
O = -o
VERSION = --std=c++11
FLAGS = -Wall -g
SOURCES = x25519.cpp montgomery.cpp
OBJECTS = $(SOURCES:.cpp=.o)
OUTPUT = x25519
REMOVE = x25519 *.o

$(OUTPUT): $(OBJECTS)
	$(CC) $(FLAGS) $(VERSION) $(OBJECTS) -o $(OUTPUT) -lgmp -lgmpxx
x25519.o: x25519.cpp montgomery.h global.h
	$(CC) $(C) $(FLAGS) $(VERSION) x25519.cpp -lgmp -lgmpxx
montgomery.o: montgomery.cpp montgomery.h global.h
	$(CC) $(C) $(FLAGS) $(VERSION) montgomery.cpp -lgmp -lgmpxx
.PHONY:
clean:
	@rm -rf $(REMOVE)

