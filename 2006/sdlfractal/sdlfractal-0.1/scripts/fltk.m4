# Configure paths for FLTK

dnl AM_PATH_FLTK([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for FLTK, and define FLTK_CFLAGS and FLTK_LIBS
dnl
AC_DEFUN([AM_PATH_FLTK],
[dnl 
dnl Get the cflags and libraries from the fltk-config script
dnl
AC_ARG_WITH(fltk-prefix,[  --with-fltk-prefix=PFX   Prefix where FLTK is installed (optional)],
            fltk_prefix="$withval", fltk_prefix="")
AC_ARG_ENABLE(fltktest, [  --disable-fltktest       Do not try to compile and run a test FLTK program],
		    , enable_fltktest=yes)

  if test x$fltk_prefix != x ; then
     fltk_args="$fltk_args --prefix=$fltk_prefix"
     if test x${FLTK_CONFIG+set} != xset ; then
        FLTK_CONFIG=$fltk_prefix/bin/fltk-config
     fi
  fi

  AC_REQUIRE([AC_CANONICAL_TARGET])
  PATH="$prefix/bin:$prefix/usr/bin:$PATH"
  AC_PATH_PROG(FLTK_CONFIG, fltk-config, no, [$PATH])
  min_fltk_version=ifelse([$1], ,1.1.0,$1)
  AC_MSG_CHECKING(for FLTK - version >= $min_fltk_version)
  no_fltk=""
  if test "$FLTK_CONFIG" = "no" ; then
    no_fltk=yes
  else
    FLTK_CFLAGS=`$FLTK_CONFIG $fltkconf_args --cflags`
    FLTK_LIBS=`$FLTK_CONFIG $fltkconf_args --ldstaticflags`

    fltk_major_version=`$FLTK_CONFIG $fltk_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    fltk_minor_version=`$FLTK_CONFIG $fltk_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    fltk_micro_version=`$FLTK_CONFIG $fltk_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_fltktest" = "xyes" ; then
      ac_save_CXXFLAGS="$CXXFLAGS"
      ac_save_LIBS="$LIBS"
      CXXFLAGS="$CXXFLAGS $FLTK_CFLAGS"
      LIBS="$LIBS $FLTK_LIBS"
dnl
dnl Now check if the installed FLTK is sufficiently new. (Also sanity
dnl checks the results of fltk-config to some extent
dnl
      rm -f conf.fltktest
      AC_LANG_PUSH([C++])
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FL/Fl.H"

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
  system ("touch conf.fltktest");
  */
  { FILE *fp = fopen("conf.fltktest", "a"); if ( fp ) fclose(fp); }

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = my_strdup("$min_fltk_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_fltk_version");
     exit(1);
   }

   if ((FL_MAJOR_VERSION > major) ||
      ((FL_MAJOR_VERSION == major) && (FL_MINOR_VERSION > minor)) ||
      ((FL_MAJOR_VERSION == major) && (FL_MINOR_VERSION == minor) && (FL_PATCH_VERSION >= micro)))
    {
      return 0;
    }
  else
    {
      printf("\n*** 'fltk-config --version' returned %d.%d.%d, but the minimum version\n", FL_MAJOR_VERSION, FL_MINOR_VERSION, FL_PATCH_VERSION);
      printf("*** of FLTK required is %d.%d.%d. If fltk-config is correct, then it is\n", major, minor, micro);
      printf("*** best to upgrade to the required version.\n");
      printf("*** If fltk-config was wrong, set the environment variable FLTK_CONFIG\n");
      printf("*** to point to the correct copy of fltk-config, and remove the file\n");
      printf("*** config.cache before re-running configure\n");
      return 1;
    }
}

],, no_fltk=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CXXFLAGS="$ac_save_CXXFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_fltk" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$FLTK_CONFIG" = "no" ; then
       echo "*** The fltk-config script installed by FLTK could not be found"
       echo "*** If FLTK was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the FLTK_CONFIG environment variable to the"
       echo "*** full path to fltk-config."
     else
       if test -f conf.fltktest ; then
        :
       else
          echo "*** Could not run FLTK test program, checking why..."
          CXXFLAGS="$CXXFLAGS $FLTK_CFLAGS"
          LIBS="$LIBS $FLTK_LIBS"
          AC_TRY_LINK([
#include <stdio.h>
#include "FL/Fl.H"

int main(int argc, char *argv[])
{ return 0; }
#undef  main
#define main K_and_R_C_main
],      [ return 0; ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding FLTK or finding the wrong"
          echo "*** version of FLTK. If it is not finding FLTK, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means FLTK was incorrectly installed"
          echo "*** or that you have moved FLTK since it was installed. In the latter case, you"
          echo "*** may want to edit the fltk-config script: $FLTK_CONFIG" ])
          CXXFLAGS="$ac_save_CXXFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     FLTK_CFLAGS=""
     FLTK_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_LANG_POP([C++])
  AC_SUBST(FLTK_CFLAGS)
  AC_SUBST(FLTK_LIBS)
  rm -f conf.fltktest
])
