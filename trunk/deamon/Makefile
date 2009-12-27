OBJS = luirc_deamon.o preferences.o
CC = g++ -g -O3 -Wall
FILE = luirc_deamon 
EXECUT = luirc
LIBRARYS = boost_program_options

luirc: $(OBJS)
	$(CC) -l$(LIBRARYS) $(OBJS) -o $(EXECUT)
main: 
	$(CC) $(FILE).cpp -c $(FILE).o
preferences:
	$(CC) preferences.cpp -c preferences.o 
clean:
	rm  $(FILE).o $(EXECUT)