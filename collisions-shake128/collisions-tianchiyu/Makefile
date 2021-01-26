CC = mpic++
C = -c
O = -o
VERSION = --std=c++11
FLAGS = -Wall -g
PTHREAD = -lpthread
SOURCES = collision.cpp keccak.cpp utilities.cpp
OUTPUT1 = shake128
OUTPUT2 = collision
REMOVE = collision *.o


ifeq ($(TARGET),Collision)
	FLAGS += -D COLLISION_
endif

collision: collision.o keccak.o utilities.o
	$(CC) $(FLAGS) $(VERSION) collision.o keccak.o utilities.o -o $(OUTPUT2)
collision.o: collision.cpp
	$(CC) $(C) $(FLAGS) $(VERSION)  collision.cpp
keccak.o: keccak.cpp keccak.h
	$(CC) $(C) $(FLAGS) $(VERSION) keccak.cpp
utilities.o: utilities.cpp
	$(CC) $(C) $(FLAGS) $(VERSION) utilities.cpp

.PHONY:
clean:
	@rm -rf $(REMOVE)
