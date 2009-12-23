OBJS = luirc_deamon.o
CC = g++ -O3 -Wall
FILE = luirc_deamon
EXECUT = luirc
LIBRARYS = boost_program_options

luirc: $(OBJS)
	$(CC) -g -l$(LIBRARYS) $(OBJS) -o $(EXECUT)
main: 
	$(CC) -g $(FILE).cpp -c $(FILE).o
clean:
	rm  $(FILE).o $(EXECUT)