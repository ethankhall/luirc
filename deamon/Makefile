OBJS = luirc_deamon.o
CC = g++ -O3 -Wall
FILE = luirc_deamon
EXECUT = luirc
LIBRARYS = boost_program_options

luirc: $(OBJS)
	$(CC) -l$(LIBRARYS) $(OBJS) -o $(EXECUT)
main: 
	$(CC) $(FILE).cpp -c $(FILE).o
clean:
	rm  $(FILE).o $(EXECUT)