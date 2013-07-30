#!/usr/bin/env gnuplot

set terminal pdf dashed linewidth 1.5 size 5.0, 3.5
set output "flags-graph.pdf"

set yrange [0:37]

### Plot ###

set title "Compiler Flags Comparison"
set ylabel "Megabyte / Second Average"
set key off
set xtics rotate by -55
set rmargin 6

plot "maxtable.csv" using 0:($14 / 1024):xticlabels(1) with linespoints pt 2
