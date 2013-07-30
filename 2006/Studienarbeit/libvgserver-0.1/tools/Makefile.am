# $Id: Makefile.am 204 2006-06-18 14:45:04Z bingmann $

bin_PROGRAMS = merge reorder snapshot snapinfo rtreefig randomquery randomqueryincremental

merge_SOURCES = merge.cpp

reorder_SOURCES = reorder.cpp

snapshot_SOURCES = snapshot.cc

snapinfo_SOURCES = snapinfo.cc

rtreefig_SOURCES = rtreefig.cc

randomquery_SOURCES = randomquery.cc

randomqueryincremental_SOURCES = randomqueryincremental.cc

AM_CXXFLAGS = -I$(top_builddir)

LDADD = $(top_builddir)/libvgserver.a
