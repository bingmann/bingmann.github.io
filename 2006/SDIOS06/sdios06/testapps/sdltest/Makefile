SOURCES = $(wildcard *.cc)
OBJECTS = $(addsuffix .o, $(basename $(SOURCES)))
GOAL=sdltest
CFLAGS = -O0 -g3 -Wall -W -Werror -I. `sdl-config --cflags`
LFLAGS = `sdl-config --libs` -lSDL_image

all: $(GOAL)

$(GOAL): $(OBJECTS)
	g++ $(OBJECTS) $(LFLAGS) -o $(GOAL)

%.o: %.cc
	g++ $(CFLAGS) -c $*.cc -o $*.o

clean:
	rm -rf $(OBJECTS) $(GOAL)
