.PHONY: clean release

# tune these for your setup & compile options
OPTIONS=-DLINUX
PLATFORM=-march=i686

# basic compiling and linking - rarely changed
CC=cc
CPP=c++
CFLAGS=$(PLATFORM) -Wall -W -ggdb -pipe $(OPTIONS) $(shell sdl-config --cflags)
_CFLAGS=$(PLATFORM) -O3 -fomit-frame-pointer -ffast-math -pipe $(OPTIONS) $(shell sdl-config --cflags)
LDFLAGS=-lSDL_net
_LDFLAGS=-lSDL_net
STATICLIBS=libs/libSDL_net.a libs/libSDL.a
OBJS=connection.o elbot.o initvars.o log.o args.o actors.o stories.o local.o fortune.o pathfinder.o asc.o


clean: clean-custom
	rm -f $(OBJS) elbot.bin

# the compile targets
elbot.bin: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o elbot.bin $(OBJS)

release:
	make 'CFLAGS=$(_CFLAGS)' 'LDFLAGS=$(_LDFLAGS)'

static:
	make 'CFLAGS=$(_CFLAGS)' 'LDFLAGS=$(_LDFLAGS)' 'OBJS=$(OBJS) $(STATICLIBS)'

%.o: %.c
	$(CC) $(CFLAGS) -c $*.c
