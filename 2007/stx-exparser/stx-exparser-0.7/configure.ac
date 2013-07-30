#                                               -*- Autoconf -*-
# $Id: configure.ac 59 2007-07-17 14:43:23Z tb $
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(stx-exparser, 0.7)
AC_CONFIG_SRCDIR([libstx-exparser/ExpressionParser.cc])
AC_CONFIG_AUX_DIR(scripts)
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

# enable full optimization by configure switch
AC_ARG_ENABLE(optimize,
	AS_HELP_STRING([--enable-optimize],
		       [Build with full optimization @<:@default=no@:>@]),
        [ case "${enableval}" in
          yes)
                CFLAGS="$CFLAGS -O3 -DNDEBUG";
                CXXFLAGS="$CXXFLAGS -O3 -DNDEBUG";
          ;;
          no) ;;
          *) AC_MSG_ERROR(bad value ${enableval} for --enable-optimize) ;;
          esac ],
        [ optimize=false ])

# set debug info flag if no optimization flags are set.
if test "$CFLAGS" == ""; then
    CFLAGS="-g"
fi
if test "$CXXFLAGS" == ""; then
    CXXFLAGS="-g"
fi

AC_ARG_ENABLE(gcov,
	      AS_HELP_STRING([--enable-gcov],
                             [enable test coverage with gcov @<:@default=no@:>@]),
              [case "${enableval}" in
                 yes) gcov=true ;;
                 no)  gcov=false ;;
                 *)   AC_MSG_ERROR([bad value ${enableval} for --enable-gcov]) ;;
               esac],
              [gcov=false])

AM_CONDITIONAL(GCOV, test x"$gcov" = "xtrue")

if test x"$gcov" = "xtrue"; then
    CFLAGS="$CFLAGS -fprofile-arcs -ftest-coverage"
    CXXFLAGS="$CXXFLAGS -fprofile-arcs -ftest-coverage"
fi

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_LANG([C++])

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDC
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_FUNC_STRTOD
AC_CHECK_FUNCS([pow sqrt strcasecmp strtol strtoul strtoull])

# Boost Base autoconf check
AX_BOOST_BASE([1.33.0])

# check for Boost.Spirit
AC_CHECK_HEADER([boost/spirit/core.hpp], [],
    AC_MSG_ERROR([
*** could not find the boost headers ***
*** install the libboost-dev package (http://www.boost.org) ***]))

AC_CHECK_HEADER([boost/spirit/phoenix.hpp], [],
    AC_MSG_ERROR([
*** could not find the boost headers ***
*** install the libboost-dev package (http://www.boost.org) ***]))

# check whether cppunit is available
AM_PATH_CPPUNIT(1.10.0)

AM_CONDITIONAL(HAVE_CPPUNIT, test x$CPPUNIT_CONFIG != xno)

if test x$CPPUNIT_CONFIG == xno; then
    AC_MSG_WARN([
	cppunit does not seem to be installed on your system.
	The testsuite will not be built!
    ])
fi

# Check for wxWidgets 2.6.0 or later
AM_OPTIONS_WXCONFIG

AM_PATH_WXCONFIG(2.6.0, wxWin=1)

AM_CONDITIONAL(HAVE_WXWIDGETS, test "$wxWin" == 1)

if test "$wxWin" != 1; then
    AC_MSG_WARN([
	wxWidgets does not seem to be installed on your system.
	The wxParserDemo will not be built!

        If you think wxWidgets >= 2.6.0 is installed,
	please check that wx-config is in path, the directory
	where wxWidgets libraries are installed (returned by
	'wx-config --libs' command) is in LD_LIBRARY_PATH or
	equivalent variable.
    ])
fi

# Win32 tools to add icons to the demo program
case "${host}" in
*-*-cygwin* | *-*-mingw32*)
    AC_CHECK_TOOL(RESCOMP, windres)
    ;;
esac
AM_CONDITIONAL(GOT_RESCOMP, test x$RESCOMP != x)

# Check for SWIG
AC_PROG_SWIG(1.3.29)

SWIG_ENABLE_CXX
AC_CHECK_PROG(PERL,perl,perl)

AM_CONDITIONAL(HAVE_SWIG, test x"$SWIG_LIB" != x)

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 libstx-exparser/Makefile
	         testsuite/Makefile
		 wxparserdemo/Makefile
		 perl-binding/Makefile
                 examples/Makefile
		 examples/simple/Makefile
                 examples/csvfilter/Makefile
                 examples/csvtool/Makefile])
AC_OUTPUT
