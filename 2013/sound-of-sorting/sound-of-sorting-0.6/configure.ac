# -*- mode: autoconf -*-

AC_PREREQ(2.59)

AC_INIT(sound-of-sorting, 0.6)
AC_CONFIG_SRCDIR(src/WMain.cpp)
AC_CONFIG_AUX_DIR(acscripts)

AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(foreign)
#AM_MAINTAINER_MODE

# check for Win32 system and set compilation flag.

AC_MSG_CHECKING(building for Win32)
case "$target_os" in
    *cygwin* | *mingw32*)
        ON_WIN32=true
        AC_MSG_RESULT(yes)
        ;;
    *)
        ON_WIN32=false
        AC_MSG_RESULT(no)
        ;;
esac
AM_CONDITIONAL(ON_WIN32, $ON_WIN32)

# set debug info flag if no optimization flags are set.

if test "$CFLAGS" == ""; then
    CFLAGS="-g"
fi
if test "$CXXFLAGS" == ""; then
    CXXFLAGS="-g"
fi

# enable full optimization by configure switch

AC_ARG_ENABLE(optimize,
	AS_HELP_STRING([--enable-optimize],
		       [Build with full optimization @<:@default=no@:>@]),
        [ case "${enableval}" in
          yes)
                CFLAGS="$CFLAGS -O3";
                CXXFLAGS="$CXXFLAGS -O3";
          ;;
          no) ;;
          *) AC_MSG_ERROR(bad value ${enableval} for --enable-optimize) ;;
          esac ],
        [ optimize=false ])

# check for programs.

AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB

AC_LANG([C++])

# check for SDL

SDL_VERSION=1.2.8
AM_PATH_SDL($SDL_VERSION,
            :,
	    AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])
)

# check for wxWidgets 2.8.9 or later

AM_OPTIONS_WXCONFIG

AM_PATH_WXCONFIG(2.8.9, [wxWin=1], [wxWin=0], [core,base,adv])

if test "$wxWin" != 1; then
    AC_MSG_ERROR([
	wxWidgets does not seem to be installed on your system.

        If you think wxWidgets >= 2.8.9 is installed,
	please check that wx-config is in path, the directory
	where wxWidgets libraries are installed (returned by
	'wx-config --libs' command) is in LD_LIBRARY_PATH or
	equivalent variable.
    ])
fi

# Win32 tools to add icons to the programs

if $ON_WIN32; then
   AM_CONDITIONAL(GOT_RESCOMP, test "$WX_RESCOMP" != "")
else
   AM_CONDITIONAL(GOT_RESCOMP, false)
fi

# transform Makefiles

AC_OUTPUT([Makefile src/Makefile])
