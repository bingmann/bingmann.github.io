#!/usr/bin/env gnuplot

set terminal pdf monochrome size 3,3
set output 'Algebra2Par29-pic2.pdf'

set parametric
set dummy x
set autoscale

set xrange [-1.5:1.5]
set yrange[-1.5:1.5]
set zeroaxis

set xtics axis
set ytics axis
unset border

set samples 5000
set key off

plot x, sqrt(x**3 + x**2) linestyle 1, \
     x, -sqrt(x**3 + x**2) linestyle 1
