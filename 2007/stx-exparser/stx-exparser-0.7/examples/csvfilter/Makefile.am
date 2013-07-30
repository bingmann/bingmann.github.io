# $Id: Makefile.am 59 2007-07-17 14:43:23Z tb $

noinst_PROGRAMS = csvfilter

csvfilter_SOURCES = csvfilter.cc

csvfilter_LDADD = $(top_srcdir)/libstx-exparser/libstx-exparser.a

AM_CFLAGS = -W -Wall -I$(top_srcdir)/libstx-exparser
AM_CXXFLAGS = -W -Wall -Wold-style-cast -I$(top_srcdir)/libstx-exparser

EXTRA_DIST = mysql-world-city.csv mysql-world-country.csv cia-world-factbook.csv
