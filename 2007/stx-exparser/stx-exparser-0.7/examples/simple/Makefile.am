# $Id: Makefile.am 59 2007-07-17 14:43:23Z tb $

noinst_PROGRAMS = exprcalc

exprcalc_SOURCES = exprcalc.cc

exprcalc_LDADD = $(top_srcdir)/libstx-exparser/libstx-exparser.a

AM_CFLAGS = -W -Wall -I$(top_srcdir)/libstx-exparser
AM_CXXFLAGS = -W -Wall -Wold-style-cast -I$(top_srcdir)/libstx-exparser
