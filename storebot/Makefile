CFLAGS=-Wall -ggdb
LDFLAGS=-lm -lSDL -lSDL_net
OBJS=actors.o connection.o config.o credit.o items.o log.o main.o misc.o privilege.o trade.o

storebot: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o storebot $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $*.c

clean:
	rm -fv $(OBJS) storebot
