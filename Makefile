CC = gcc
CXX = g++
ECHO = echo
RM = rm -f

TERM = "\"S2021\""

CFLAGS = -Wall -Werror -ggdb -funroll-loops -DTERM=$(TERM)
CXXFLAGS = -Wall -Werror -ggdb -funroll-loops -DTERM=$(TERM)

LDFLAGS = -lncurses

BIN = rlg327
OBJS = rlg327.o dungeon.o npc.o parsers.o pc.o \
	   printers.o move.o pathfinder.o
HEAP = heap.o

all: $(BIN) etags

$(BIN): $(HEAP)
	@$(ECHO) Linking $@
	@$(CC) $^ -o $@ $(LDFLAGS)

$(BIN): $(OBJS)
	@$(ECHO) Linking $@
	@$(CXX) $^ -o $@ $(LDFLAGS)

-include $(OBJS:.o=.d)

%.o: %.c
	@$(ECHO) Compiling $<
	@$(CC) $(CFLAGS) -MMD -MF $*.d -c $<

%.o: %.cpp
	@$(ECHO) Compiling $<
	@$(CXX) $(CXXFLAGS) -MMD -MF $*.d -c $<

.PHONY: all clean clobber etags

clean:
	@$(ECHO) Removing all generated files
	@$(RM) *.o $(BIN) *.d TAGS core vgcore.* gmon.out *.stackdump

clobber: clean
	@$(ECHO) Removing backup files
	@$(RM) *~ \#* *pgm

etags:
	@$(ECHO) Updating TAGS
	@etags *.[ch]