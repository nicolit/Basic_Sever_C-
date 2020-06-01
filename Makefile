CC=g++  # compiler name
CFLAGS=-std=c++11 -Wall
LDFLAGS = -g # -g flag adds debugging information to the executable file
LFLAGS = -Wall # flag use in files linkage - most compiler warnings 
SERVERSRC=Server.h Server.cpp
CLIENTSRC=Client.h Client.cpp
EVENTSRC=Event.h Event.cpp
LOGGERSRC=Logger.h Logger.cpp
COMMPARSER=CommandParser.h CommandParser.cpp
INCLUDS = -I. Server.h Event.h Logger.h CommandParser.h
SRCFILES=emServer.cpp Server.cpp Event.cpp Logger.cpp CommandParser.cpp
LIBOBJ=$(LIBSRC:.cpp=.o)
OFFSET=-D_FILE_OFFSET_BITS=64
TARFLAGS = -cvf
TARNAME = ex5.tar

SERVEREXC= emServer
CLIENTEXC= emClient
TARGET = $(SERVEREXC) $(CLIENTEXC) 

TAR_SRC= Makefile $(SERVERSRC) $(EVENTSRC) $(LOGGERSRC) $(COMMPARSER) \
		$(CLIENTSRC) emServer.cpp emClient.cpp README
		

all: $(TARGET)
.DEFAULT_GOAL := all

Event.o: $(EVENTSRC)
	$(CC) $(CFLAGS) -c Event.cpp

Logger.o: $(LOGGERSRC)
	$(CC) $(CFLAGS) -c Logger.cpp
	
CommandParser.o: $(COMMPARSER)
	$(CC) $(CFLAGS) -c CommandParser.cpp

Server.o: $(SERVERSRC) $(EVENTSRC) $(LOGGERSRC) $(COMMPARSER)
	$(CC) $(CFLAGS) -c Server.cpp

emServer.o: emServer.cpp Server.o Event.o Logger.o CommandParser.o
			$(CC) $(CFLAGS) -pthread -c emServer.cpp
			
emServer: emServer.o
		  $(CC) $(CFLAGS) -pthread Server.o Event.o Logger.o CommandParser.o emServer.o -o emServer
		  
Client.o: $(CLIENTSRC) $(LOGGERSRC) $(COMMPARSER)
		  $(CC) $(CFLAGS) -c Client.cpp

emClient.o: emClient.cpp Logger.o CommandParser.o Client.o
			$(CC) $(CFLAGS) -c emClient.cpp

emClient: emClient.o
		  $(CC) $(CFLAGS) Logger.o CommandParser.o Client.o emClient.o -o emClient

clean:
	rm -rf $(TARGET) $(LIBOBJ) $(OBJ) *.o *~ *core *.gch

depend:
	makedepend -- $(CFLAGS) -- $(SRC) $(LIBSRC)

tar:
	tar $(TARFLAGS) $(TARNAME) $(TAR_SRC)
