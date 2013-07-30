# $Id: Makefile.am 99 2006-04-23 07:53:53Z bingmann $

bin_PROGRAMS = testsuite

TESTS = testsuite

if HAVE_CPPUNIT

testsuite_SOURCES = TestRunner.cc

testsuite_SOURCES += AnyTypeTest.cc AttributeBlobTest.cc GraphDataTest.cc GraphContainerTest.cc GraphPropertiesTest.cc GraphLoaderTest.cc AttributeParserTest.cc

else

testsuite_SOURCES = TestTrue.cc

endif

AM_CXXFLAGS = -I$(top_srcdir) @CPPUNIT_CFLAGS@
LDADD = $(top_builddir)/libvgserver.a @CPPUNIT_LIBS@
