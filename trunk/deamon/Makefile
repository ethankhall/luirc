OBJS = luirc_deamon.o preferences.o
CC = gcc -O3 -Wall
FILE = luirc_deamon.c
EXECUT = luirc

luirc: $(OBJS)
	$(CC) $(OBJS) -o $(EXECUT)
main: preferences.o
	$(CC) -c luirc_deamon.o luirc_deamon.c
preferences:
	$(CC) -c preferences.o preferences.c

test:
	g++ -lboost_program_options test.cpp -o test
clean:
	rm *~ *.o 