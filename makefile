CFLAGS = -std=c11 -m64 -Icurl/include -Ijansson/src/

ifdef DEBUG
CC = clang
CFLAGS += -Weverything -Werror
OBJDIR = DEBUG
else
CC = gcc
CFLAGS += -flto -O3 -fopenmp
OBJDIR = RELEASE
ifdef PARALLEL
CFLAGS += -DPARALLEL_NEGAMAX -DUSE_MAX_THREADS
endif
endif

SRCS = board.c brain.c globals.c main.c pregame.c
ifndef CONSOLE
SRCS += net.c
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
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJDIR)/*.o
