#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)

AC_INIT([sdlfractal], [0.1])
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_AUX_DIR([scripts/])

AC_CANONICAL_HOST

AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

if test "$CFLAGS" == ""; then
    CFLAGS="-g -I/usr/include"
fi
if test "$CXXFLAGS" == ""; then
    CXXFLAGS="-g -I/usr/include"
fi

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

dnl Check for SDL
SDL_VERSION=1.2.8
AM_PATH_SDL($SDL_VERSION,
            :,
	    AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])
)

dnl Check for fltk
m4_include([scripts/fltk.m4])

FLTK_VERSION=1.1.7
AM_PATH_FLTK($FLTK_VERSION,
	     :,
	      AC_MSG_ERROR([*** fltk version $FLTK_VERSION not found!])
)

dnl Check for libpng
m4_include([scripts/libpng.m4])

LIBPNG_VERSION=1.2.12
AM_PATH_LIBPNG($LIBPNG_VERSION,
               :,
	       AC_MSG_ERROR([*** libpng version $LIBPNG_VERSION not found!])
)

CFLAGS="$CFLAGS $SDL_CFLAGS $FLTK_CFLAGS $LIBPNG_CFLAGS"
CXXFLAGS="$CXXFLAGS $SDL_CFLAGS $FLTK_CFLAGS $LIBPNG_CFLAGS"
LIBS="$LIBS $SDL_LIBS $FLTK_LIBS $LIBPNG_LIBS"

dnl Win32 tools to add icons to the programs
case "${host}" in
*-*-cygwin* | *-*-mingw32*)
    AC_CHECK_TOOL(RESCOMP, windres)
    ;;
esac
AM_CONDITIONAL(GOT_RESCOMP, test x$RESCOMP != x)

dnl Create Makefiles
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

