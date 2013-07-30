SOURCES = $(wildcard *.cc)
OBJECTS = $(addsuffix .o, $(basename $(SOURCES)))
GOAL=rbtest
CFLAGS = -O0 -g3 -Wall -W -Werror -I.
LFLAGS = 

all: $(GOAL)

$(GOAL): $(OBJECTS)
	g++ $(OBJECTS) $(LFLAGS) -o $(GOAL)

%.o: %.cc
	g++ $(CFLAGS) -c $*.cc -o $*.o

clean:
	rm -rf $(OBJECTS) $(GOAL)
