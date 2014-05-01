CFLAGS = -std=c11 -m64 $(DEBUG_FLAGS)
OPFLAGS = -O0
LDFLAGS = -m64

#fucking linking errors go fuck yourselves
CONSOLE = 1

ifdef DEBUG
CC = clang
CFLAGS += -Weverything -Werror
OBJDIR = DEBUG
else
CC = gcc
OPFLAGS = -flto -O3
OBJDIR = RELEASE
endif
ifdef PARALLEL
CFLAGS += -DPARALLEL_NEGAMAX -DUSE_MAX_THREADS -fopenmp
endif

SRCS = board.c brain.c globals.c main.c pregame.c

#this isn't working and I don't know why yet but I'm tired of dicking around
#	with it and there's no server to connect to anyways
#TODO: FIX THIS
ifndef CONSOLE
SRCS += net.c
LDFLAGS += -l:libcurl.a -L curl/lib64/ -l:libjansson.a -L jansson/lib/
CFLAGS += -Icurl/include -Ijansson/src/ -static -DCURL_STATICLIB
else
CFLAGS += -DCONSOLE
endif

SRCDIR = src
vpath %.c $(SRCDIR)
vpath %.o $(OBJDIR)

OBJS = $(SRCS:%.c=$(OBJDIR)/%.o)
EXECUTABLE = chess.0

all: $(SRCS) $(EXECUTABLE)

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(EXECUTABLE): $(OBJS) 
	$(CC) $(OPFLAGS) $(LDFLAGS) $(OBJS) -o $(OBJDIR)/$@

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) $(OPFLAGS) -c $< -o $@

clean:
	rm $(OBJDIR)/*.o
