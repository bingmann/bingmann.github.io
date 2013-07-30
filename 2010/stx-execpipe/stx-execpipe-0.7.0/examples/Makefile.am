# $Id: Makefile.am 4 2010-07-18 19:03:26Z tb $

noinst_PROGRAMS = simple1 simple2 simple3 functions1

simple1_SOURCES = simple1.cc

simple2_SOURCES = simple2.cc

simple3_SOURCES = simple3.cc

functions1_SOURCES = functions1.cc

AM_CXXFLAGS = -g -W -Wall -Wold-style-cast -I$(top_srcdir)/src

LDADD = $(top_builddir)/src/libstx-execpipe.a