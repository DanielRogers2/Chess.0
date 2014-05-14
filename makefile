CFLAGS = -std=c11 -m64 $(DEBUG_FLAGS)
OPFLAGS = -O0
LDFLAGS = -m64

ifdef DEBUG
CC = clang
CFLAGS += -Weverything -Werror -DDEBUG
OBJDIR = DEBUG
else
CC = gcc
OPFLAGS = -flto -O3
OBJDIR = RELEASE
CFLAGS += -DNDEBUG
endif
ifdef PARALLEL
CFLAGS += -DPARALLEL_NEGAMAX -DUSE_MAX_THREADS -fopenmp
LDFLAGS += -fopenmp
endif

SRCS = board.c brain.c globals.c main.c pregame.c

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
