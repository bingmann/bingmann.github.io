# B+ Tree Speed Tests
# $Id: Makefile.am 38 2007-04-27 12:49:17Z tb $

if BUILD_SPEEDTEST

noinst_PROGRAMS = speedtest

endif

speedtest_SOURCES = speedtest.cc

AM_CXXFLAGS = -W -Wall -Wold-style-cast -O3 -DNDEBUG -I$(top_srcdir)/include

EXTRA_DIST = speed-all.trt speed-all.txt speed-insert.trt speed-insert.txt \
	speedtest.gnuplot transpose.awk \
	speedtest.pdf \
	speedtest-plot-000001.png speedtest-plot-000002.png \
	speedtest-plot-000003.png speedtest-plot-000004.png \
	speedtest-plot-000005.png speedtest-plot-000006.png \
	speedtest-plot-000007.png speedtest-plot-000008.png

plots:
	USER=tb ./speedtest.gnuplot
	pdftoppm speedtest.pdf speedtest-plot
	for f in *.ppm; do convert $$f $${f%%.ppm}.png; rm $$f; done
