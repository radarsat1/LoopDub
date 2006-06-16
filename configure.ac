#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([loopdub], [0.2], [radarsat1@gmail.com])
AM_INIT_AUTOMAKE(1.9 check-news [-Wall foreign dist-bzip2])
AC_CONFIG_SRCDIR([def.h])
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_INSTALL

# Checks for libraries.
AC_CHECK_LIB(SDL, SDL_Init, [],
	[echo "Error! You need to install SDL."; exit -1])
AC_CHECK_HEADERS([/usr/include/SDL/SDL.h],
	[AC_SUBST([SDL_LOCATION],[/usr/include/SDL])],
	[AC_CHECK_HEADERS([/usr/include/SDL.h],
		[AC_SUBST([SDL_LOCATION],[])],
			[echo "Error! Cannot find SDL.h. You need to install SDL."; exit -1])
			])
AC_CHECK_LIB(portaudio, Pa_Initialize, [],
	[echo "Error! You need to install PortAudio."; exit -1])
AC_CHECK_LIB(asound, Pa_Initialize, [],
	[echo "Error! You need to install ALSA."; exit -1])

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_STAT
AC_CHECK_FUNCS([memset pow realpath strcasecmp strchr])

AC_CONFIG_FILES([Makefile
                 gui/Makefile])
AC_OUTPUT

# patch the Makefile to unpack and compile port midi

echo -n "Patching Makefile to unpack and compile PortMidi: "
cat Makefile | sed s/^all:/all:\ portmidi\\/pm_linux\\/libportmidi.a/ >Makefile.tmp
echo "" >>Makefile.tmp
echo "\# Patch: unzip and compile portmidi library" >>Makefile.tmp
echo "portmidi/pm_linux/libportmidi.a: portmidi" >>Makefile.tmp
echo '	\$(MAKE) -C portmidi' >>Makefile.tmp
echo "" >>Makefile.tmp
echo "portmidi:" >>Makefile.tmp
echo '	unzip portmidi17nov04.zip' >>Makefile.tmp
mv -v Makefile.tmp Makefile
echo "done."

