SOURCES = $(wildcard *.cc)
OBJECTS = $(addsuffix .o, $(basename $(SOURCES)))
GOAL=minixtest
CFLAGS = -O0 -g3 -Wall -W -Werror -I.

all: $(GOAL)

$(GOAL): $(OBJECTS)
	g++ $(OBJECTS) -o $(GOAL)

%.o: %.cc
	g++ $(CFLAGS) -c $*.cc -o $*.o

clean:
	rm -rf $(OBJECTS) $(GOAL)
