CC = gcc

BASEFLAGS = -Wall -pthread
NODEBUG_FLAGS = -dNDEBUG 
DEBUG_FLAGS = -g

LDLIBS = -lcurses -lm -pthread

OBJS = main.o caterpillars.o console.o gameglobals.o player.o refresher.o enemy.o llist.o bullet.o

EXE = caterpillars

debug: CFLAGS = $(BASEFLAGS) $(DEBUG_FLAGS)
debug: $(EXE)

release: CFLAGS = $(BASEFLAGS) $(NODEBUG_FLAGS) 
release: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(EXE) $(LDLIBS)

main.o: main.c caterpillars.h
	$(CC) $(CFLAGS) -c main.c

caterpillars.o: caterpillars.c caterpillars.h console.h gameglobals.h
	$(CC) $(CFLAGS) -c caterpillars.c 
	
console.o: console.c console.h
	$(CC) $(CFLAGS) -c console.c
	
gameglobals.o: gameglobals.c gameglobals.h
	$(CC) $(CFLAGS) -c gameglobals.c
	
player.o: player.c player.h
	$(CC) $(CFLAGS) -c player.c

refresher.o: refresher.c refresher.h
	$(CC) $(CFLAGS) -c refresher.c

enemy.o: enemy.c enemy.h
	$(CC) $(CFLAGS) -c enemy.c
	
llist.o: llist.c llist.h
	$(CC) $(CFLAGS) -c llist.c
	
bullet.o: bullet.c bullet.h
	$(CC) $(CFLAGS) -c bullet.c
	

clean:
	rm -f $(OBJS)
	rm -f *~
	rm -f $(EXE)

run:
	./$(EXE)

