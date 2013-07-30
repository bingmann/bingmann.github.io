# STX Expression Parser Test Cases
# $Id: Makefile.am 59 2007-07-17 14:43:23Z tb $

noinst_PROGRAMS = testsuite

TESTS = testsuite

if HAVE_CPPUNIT

testsuite_SOURCES = TestRunner.cc

testsuite_SOURCES += AnyScalarTest.cc ExpressionParserTest.cc

else

testsuite_SOURCES = TestTrue.cc

endif

AM_CXXFLAGS = -W -Wall -I$(top_srcdir)/libstx-exparser @CPPUNIT_CFLAGS@
LDADD = @CPPUNIT_LIBS@ $(top_srcdir)/libstx-exparser/libstx-exparser.a
