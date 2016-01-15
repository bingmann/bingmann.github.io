# Bispanning Graph Connectivity Testing Framework

This archive contains the **source code developed alongside my [diploma thesis](http://panthema.net/2016/0114-diploma-thesis/)** "On the Structure of the Graph of Unique Symmetric Base Exchanges of Bispanning Graphs". The source code is not a clean program which one can simply run, it is more a toolset for testing theories about bispanning graphs. Hence, many source code fragments which were written for specific hypothesis, which later turned out correct or incorrect, are still contained or were commented out.

Nevertheless, the code contains a reasonably **elegant, efficient, and flexible framework to represent and analyze directed and undirected graphs**, and to enumerate small graphs using [nauty/geng](http://users.cecs.anu.edu.au/~bdm/nauty/). The framework outputs graphs in the graphviz format, which automatically layouts and draws graphs.

The source code version 0.5 is available for [download from this webpage](http://panthema.net/2016/0114-diploma-thesis/) or from [a github repository](https://github.com/bingmann/bispanning-graph-framework.git), under the GNU General Public License Version 3.

## Compilation and Sample Run/Output

To compile the source code on Ubuntu Linux, install at least the following required packages:

```
sudo apt-get install build-essential g++ cmake libexpat-dev
sudo apt-get install libboost-regex-dev libboost-program-options-dev libigraph-dev graphviz
```

Then you need to build [nauty/geng](http://users.cecs.anu.edu.au/~bdm/nauty/), which is a separate package to enumerate graphs:

```
tar xvzf nauty25r5.tar.gz
cd nauty25r5
./configure
make
cd ..
```

And then use cmake to build the bispanning graph framework, and copy geng and genbg over.

```
mkdir build
cmake ..
make
cd src/
cp ../../nauty25r5/geng .
cp ../../nauty25r5/genbg .
```

Once done building, you can enumerate bispanning graphs using the main `play_bispanning` program. Prior to running it, you have to add the directory containing `geng` to `$PATH` such that the program will find it.

```
export PATH=$PATH:.
./play_bispanning -h
```

The program `play_bispanning` takes a list of arguments, which determine which graphs are enumerate. A simple number, like "6", enumerates all small graphs with six vertices. Additionally, one can specify any combination of the following constraints: `atomic`, `composite`, `triangle-free`, `square-free`, `general` (for parallel edges), `vconn=1`, `vconn=2`, `vconn=3`, `econn=2`, `econn=3`, and `maxdeg=4`. The constraints must precede the simple number "6", since the number triggers processing.

The output of `play_bispanning` is determined by command line flags such as `-g` and `-G`, and it not mean to be read directly. Instead, it is input for the `fdp` graph layout tool, which is part of the [graphviz](http://www.graphviz.org) program suite.

For example:

- `./play_bispanning -gG 6 | fdp -Tps -o bispanning-6-vertices.ps` <br/>
calculates the unique exchange graphs for all bispanning graphs with six vertices.

- `./play_bispanning -gG atomic 8 | fdp -Tps -o bispanning-8-vertices-atomic.ps` <br/>
calculates the unique exchange graphs for all atomic bispanning graphs with eight vertices.

## Overview of the Source Code

- `play_bispanning.cpp`: The main program used to enumerate graph and apply algorithmic tests. See above for some examples.

- `graph.h`: A generic graph implementation with many simple super-generic algorithms like BFS. This is the base class for the more specific bispanning graph and edge exchange graph implementations. For calling more advanced graph algorithms, there exist three "bridges" to external libraries: `graph_igraph.h` to [iGraph](http://igraph.org), e.g. for calculating vertex and edge connectivity, `graph_boost.h` for planarity testing, `graph_eigen.h` for calculating eigenvalues.

- `bispanning.h`: An instantiation of the generic graph for bispanning graphs. It contains algorithms to test whether a bispanning graph is atomic or composite, and methods to calculate the cuts and cycles.

- `alg_bispanning.h`: An implementation of Roskind and Tarjan's algorithm for finding two disjoint spanning trees in a graph.

- `alg_game.h`: An instantiation of the generic graph for the edge/base exchange graphs. It contains a simple algorithm to explore the edge/base exchange graph breadth-first and construct AlgGame which contains the result. This result is always tested for connectivity and optionally, the diameter is calculated.

- `decomposition.h`: Functions to test the composition theorems found in section 5 of the thesis.

Additional more general purpose code can be found in `combinatorics.h` and `debug.h`.

## Exits

Written 2016-01-13 by Timo Bingmann
