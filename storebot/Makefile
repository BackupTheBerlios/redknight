CFLAGS=-Wall -ggdb
LDFLAGS=-lm
OBJS=actors.o config.o credit.o items.o main.o misc.o privilege.o protocol.o trade.o

storebot: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o storebot $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $*.c

clean:
	rm -fv $(OBJS) storebot
