LIBPATH = ../Lib
include $(LIBPATH)/Makefile.common 

all: test_lab7

clean:
	rm *.o test_lab7

######################################################################
OBJS = rsa.o sign.o dsa.o attack_dsa.o

rsa.o: rsa.c rsa.h
	$(CC) $(CFLAGS) -c rsa.c -o ./rsa.o

sign.o: sign.c sign.h
	$(CC) $(CFLAGS) -c sign.c -o ./sign.o

dsa.o: dsa.c dsa.h
	$(CC) $(CFLAGS) -c dsa.c -o ./dsa.o

attack_dsa.o: attack_dsa.c attack_dsa.h
	$(CC) $(CFLAGS) -c attack_dsa.c -o ./attack_dsa.o

test_lab7: test_lab7.c $(OBJS)
	$(CC) $(LDFLAGS) $(CFLAGS) test_lab7.c $(OBJS) $(LIBS) -o ./test_lab7

test_lab7s:
	for i in 1 2 3 4 5 6 7 8 9 10 11; do ./test_lab7 $$i; done
