#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([loopdub], [0.2], [radarsat1@gmail.com])
AM_INIT_AUTOMAKE([foreign dist-bzip2])
AC_CONFIG_SRCDIR([def.h])
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_INSTALL

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h stdlib.h string.h unistd.h])


# Check the operating system

AC_CANONICAL_HOST([])
AC_CANONICAL_TARGET([])

case "${target_os}" in
  "")
    SYS=unknown
    ;;
  linux*)
    SYS=linux
    ;;
  bsdi*)
    SYS=bsdi
    ;;
  *bsd*)
    SYS="${target_os}"
    ;;
  darwin*)
    SYS=darwin
    LIBEXT=".dylib"
    ;;
  *mingw32* | *cygwin*)
    AC_CHECK_TOOL(WINDRES, windres, :)
    LIBEXT=".dll"

    case "${target_os}" in
      *mingw32*)
        SYS=mingw32
        ;;
      *cygwin*)
        dnl Check if we are using the mno-cygwin mode in which case we are
        dnl actually dealing with a mingw32 compiler.
        AC_EGREP_CPP(yes,
            [#ifdef WIN32
             yes
             #endif],
            SYS=mingw32, SYS=cygwin)
        ;;
    esac

    if test "${SYS}" = "mingw32"; then
      echo -n
    fi
    ;;
  *nto*)
    SYS=nto
    ;;
  solaris*)
    SYS=solaris
    ;;
  hpux*)
    SYS=hpux
    LIBEXT=".sl"
    ;;
  beos)
    SYS=beos
    ;;
  *)
    SYS="${target_os}"
    ;;
esac
AM_CONDITIONAL(HAVE_DARWIN, test "${SYS}" == "darwin")
AM_CONDITIONAL(HAVE_WIN32, test "${SYS}" == "mingw32")
AM_CONDITIONAL(HAVE_LINUX, test "${SYS}" == "linux")

echo "System is" ${SYS}

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_STAT
AC_CHECK_FUNCS([memset pow realpath strcasecmp strchr])

# Checks for libraries.
#CFLAGS="$CFLAGS -I$HOME/.local/include"
#LDFLAGS="$LDFLAGS -L$HOME/.local/lib"
#LIBS="$LIBS -lobjc"
AC_CHECK_LIB(SDL, SDL_Init, [],
	[echo "Error! You need to install SDL."; exit -1])
#AC_SUBST([SDL_LOCATION],[SDL])
AC_CHECK_HEADERS([/usr/include/SDL/SDL.h],
	[AC_SUBST([SDL_LOCATION],[/usr/include/SDL])],
	[AC_CHECK_HEADERS([SDL.h],
		[AC_SUBST([SDL_LOCATION],[])],
		[AC_CHECK_HEADERS([$HOME/.local/include/SDL/SDL.h],
			[AC_SUBST([SDL_LOCATION],[$HOME/.local/include/SDL])],
				[echo "Error! Cannot find SDL.h. You need to install SDL."; exit -1])
				])])

AC_CHECK_LIB(portaudio, Pa_Initialize, [],
	[echo "Error! You need to install PortAudio."; exit -1])

#note: AC_CHECK_LIB cannot be called conditionally.. sigh.
#if test "x${HAVE_LINUX}" == "xyes"; then
AC_CHECK_LIB(asound, Pa_Initialize, [],
	[echo "Error! You need to install ALSA."; exit -1])
#fi

AC_CONFIG_FILES([Makefile
                 gui/Makefile])
AC_OUTPUT

# patch the Makefile to unpack and compile port midi

echo -n "Patching Makefile to unpack and compile PortMidi: "

cp Makefile Makefile.tmp
if test "${SYS}" == "linux"; then
   cat Makefile | sed s/^all:/all:\ portmidi\\/pm_linux\\/libportmidi.a/ >Makefile.tmp
   echo "" >>Makefile.tmp
   echo "\# Patch: unzip and compile portmidi library" >>Makefile.tmp
   echo "portmidi/pm_linux/libportmidi.a: portmidi" >>Makefile.tmp
   echo '	\$(MAKE) -C portmidi' >>Makefile.tmp
   echo "" >>Makefile.tmp
fi

if test "${SYS}" == "darwin"; then
   cat Makefile | sed s/^all:/all:\ portmidi\\/pm_mac\\/build\\/libportmidi.a/ >Makefile.tmp
   echo "" >>Makefile.tmp
   echo "\# Patch: unzip portmidi library" >>Makefile.tmp
fi

echo "portmidi:" >>Makefile.tmp
echo '	unzip portmidi17nov04.zip' >>Makefile.tmp
mv -vf Makefile.tmp Makefile
echo "done."

