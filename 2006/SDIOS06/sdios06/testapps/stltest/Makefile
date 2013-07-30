SOURCES = $(wildcard *.cc)
OBJECTS = $(addsuffix .o, $(basename $(SOURCES)))
GOAL=stltest
CFLAGS = -fno-exceptions -fno-rtti -O0 -g3 -Wall -W -Werror -I. -nostdinc++ -nodefaultlibs
LFLAGS = -nostdinc++ -nodefaultlibs -lc -lm -lgcc -lgcc_s

all: $(GOAL)

$(GOAL): $(OBJECTS)
	gcc $(OBJECTS) $(LFLAGS) -o $(GOAL)

%.o: %.cc
	g++ $(CFLAGS) -c $*.cc -o $*.o

clean:
	rm -rf $(OBJECTS) $(GOAL)
