# Configure paths for LIBPNG

dnl AM_PATH_LIBPNG([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for LIBPNG, and define LIBPNG_CFLAGS and LIBPNG_LIBS
dnl
AC_DEFUN([AM_PATH_LIBPNG],
[dnl 
dnl Get the cflags and libraries from the libpng-config script
dnl
AC_ARG_WITH(libpng-prefix,[  --with-libpng-prefix=PFX   Prefix where libpng is installed (optional)],
            libpng_prefix="$withval", libpng_prefix="")
AC_ARG_ENABLE(libpngtest, [  --disable-libpngtest       Do not try to compile and run a test libpng program],
              , enable_libpngtest=yes)

  if test x$libpng_prefix != x ; then
     libpng_args="$libpng_args --prefix=$libpng_prefix"
     if test x${LIBPNG_CONFIG+set} != xset ; then
        LIBPNG_CONFIG=$libpng_prefix/bin/libpng-config
     fi
  fi

  AC_REQUIRE([AC_CANONICAL_TARGET])
  PATH="$prefix/bin:$prefix/usr/bin:$PATH"
  AC_PATH_PROG(LIBPNG_CONFIG, libpng-config, no, [$PATH])
  min_libpng_version=ifelse([$1], ,1.2.0,$1)
  AC_MSG_CHECKING(for libpng - version >= $min_libpng_version)
  no_libpng=""
  if test "$LIBPNG_CONFIG" = "no" ; then
    no_libpng=yes
  else
    LIBPNG_CFLAGS=`$LIBPNG_CONFIG $libpngconf_args --cflags`
    LIBPNG_LIBS=`$LIBPNG_CONFIG $libpngconf_args --static --ldflags`

    libpng_major_version=`$LIBPNG_CONFIG $libpng_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    libpng_minor_version=`$LIBPNG_CONFIG $libpng_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    libpng_micro_version=`$LIBPNG_CONFIG $libpng_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_libpngtest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $LIBPNG_CFLAGS"
      LIBS="$LIBS $LIBPNG_LIBS"
dnl
dnl Now check if the installed LIBPNG is sufficiently new. (Also sanity
dnl checks the results of libpng-config to some extent
dnl
      rm -f conf.libpngtest
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "png.h"

char*
my_strdup (char *str)
{
  char *new_str;
  
  if (str)
    {
      new_str = (char *)malloc ((strlen (str) + 1) * sizeof(char));
      strcpy (new_str, str);
    }
  else
    new_str = NULL;
  
  return new_str;
}

int main (int argc, char *argv[])
{
  int major, minor, micro;
  char *tmp_version;

  /* This hangs on some systems (?)
  system ("touch conf.libpngtest");
  */
  { FILE *fp = fopen("conf.libpngtest", "a"); if ( fp ) fclose(fp); }

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = my_strdup("$min_libpng_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_libpng_version");
     exit(1);
   }

   if ((PNG_LIBPNG_VER_MAJOR > major) ||
      ((PNG_LIBPNG_VER_MAJOR == major) && (PNG_LIBPNG_VER_MINOR > minor)) ||
      ((PNG_LIBPNG_VER_MAJOR == major) && (PNG_LIBPNG_VER_MINOR == minor) && (PNG_LIBPNG_VER_RELEASE >= micro)))
    {
      return 0;
    }
  else
    {
      printf("\n*** 'libpng-config --version' returned %d.%d.%d, but the minimum version\n", PNG_LIBPNG_VER_MAJOR, PNG_LIBPNG_VER_MINOR, PNG_LIBPNG_VER_RELEASE);
      printf("*** of libpng required is %d.%d.%d. If libpng-config is correct, then it is\n", major, minor, micro);
      printf("*** best to upgrade to the required version.\n");
      printf("*** If libpng-config was wrong, set the environment variable LIBPNG_CONFIG\n");
      printf("*** to point to the correct copy of libpng-config, and remove the file\n");
      printf("*** config.cache before re-running configure\n");
      return 1;
    }
}

],, no_libpng=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_libpng" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$LIBPNG_CONFIG" = "no" ; then
       echo "*** The libpng-config script installed by LIBPNG could not be found"
       echo "*** If LIBPNG was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the LIBPNG_CONFIG environment variable to the"
       echo "*** full path to libpng-config."
     else
       if test -f conf.libpngtest ; then
        :
       else
          echo "*** Could not run LIBPNG test program, checking why..."
          CFLAGS="$CFLAGS $LIBPNG_CFLAGS"
          LIBS="$LIBS $LIBPNG_LIBS"
          AC_TRY_LINK([
#include <stdio.h>
#include "png.h"

int main(int argc, char *argv[])
{ return 0; }
#undef  main
#define main K_and_R_C_main
],      [ return 0; ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding LIBPNG or finding the wrong"
          echo "*** version of LIBPNG. If it is not finding LIBPNG, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means LIBPNG was incorrectly installed"
          echo "*** or that you have moved LIBPNG since it was installed. In the latter case, you"
          echo "*** may want to edit the libpng-config script: $LIBPNG_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     LIBPNG_CFLAGS=""
     LIBPNG_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(LIBPNG_CFLAGS)
  AC_SUBST(LIBPNG_LIBS)
  rm -f conf.libpngtest
])
