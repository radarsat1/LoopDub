AC_PREREQ(2.57)

m4_define([LOOPDUB_VER],[m4_esyscmd([util/version.sh | tr -d '\n'])])
AC_INIT([loopdub], [LOOPDUB_VER], [radarsat1@gmail.com])

AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

# nicer build output
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_ENABLE_STATIC(yes)
AC_ENABLE_SHARED(no)

# libtool
AC_CONFIG_MACRO_DIR([m4])

# Check for programs
AC_PROG_CC
AC_PROG_CXX
AM_PROG_LIBTOOL
AM_PROG_CC_C_O

# Check C compiler properties
AC_C_CONST
AC_TYPE_SIZE_T

AC_CHECK_SIZEOF(char *)
AM_CONDITIONAL([X64], [test $ac_cv_sizeof_char_p = 8])

# Check for headers
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Check for libraries

PKG_CHECK_MODULES([SDL], [sdl >= 1.2],
  [CFLAGS="$CFLAGS $SDL_CFLAGS"
   CPPFLAGS="$CPPFLAGS $SDL_CFLAGS"
   LDFLAGS="$LDFLAGS $SDL_LIBS"],
  [AC_CHECK_LIB(SDL,SDL_Init, [], [AC_MSG_ERROR([SDL is required.])])
   AC_CHECK_HEADER(SDL.h, [], [AC_MSG_ERROR([SDL is required.])])])

PKG_CHECK_MODULES([samplerate], [samplerate >= 0.1.7],
  [CFLAGS="$CFLAGS $samplerate_CFLAGS"
   CPPFLAGS="$CPPFLAGS $samplerate_CFLAGS"
   LDFLAGS="$LDFLAGS $samplerate_LIBS"],
  [AC_CHECK_LIB(samplerate,src_new, [],
    [AC_MSG_ERROR([libsamplerate is required.])])
   AC_CHECK_HEADER(samplerate.h, [],
    [AC_MSG_ERROR([libsamplerate is required.])])])

PKG_CHECK_MODULES([sndfile], [sndfile >= 0.116.1],
  [CFLAGS="$CFLAGS $sndfile_CFLAGS"
   CPPFLAGS="$CPPFLAGS $sndfile_CFLAGS"
   LDFLAGS="$LDFLAGS $sndfile_LIBS"],
  [AC_CHECK_LIB(sndfile,sf_open, [],
    [AC_MSG_ERROR([libsndfile is required.])])
   AC_CHECK_HEADER(sndfile.h, [],
    [AC_MSG_ERROR([libsndfile is required.])])])

# Optional libraries

HAVE_ALSA=true
AC_ARG_ENABLE(alsa,
  [AC_HELP_STRING([--enable-alsa],[compile in ALSA support])],
  [test x$enableval = xno && HAVE_ALSA=false])
AS_IF($HAVE_ALSA,
  [PKG_CHECK_MODULES([alsa], [alsa >= 1.0.20],
   [CFLAGS="$CFLAGS $alsa_CFLAGS"
    CPPFLAGS="$CPPFLAGS $alsa_CFLAGS"
    LDFLAGS="$LDFLAGS $alsa_LIBS"],
   [AC_CHECK_LIB(asound,snd_config_load, [], [HAVE_ALSA=false])
    AC_CHECK_HEADER(asoundlib.h, [], [HAVE_ALSA=false])])])

HAVE_JACK=true
AC_ARG_ENABLE(jack,
  [AC_HELP_STRING([--enable-jack],[compile in Jack support])],
  [test x$enableval = xno && HAVE_JACK=false])
AS_IF($HAVE_JACK,
  [PKG_CHECK_MODULES([jack], [jack >= 0.116.1],
    [CFLAGS="$CFLAGS $jack_CFLAGS"
     CPPFLAGS="$CPPFLAGS $jack_CFLAGS"
     LDFLAGS="$LDFLAGS $jack_LIBS"],
    [AC_CHECK_LIB(jack,jack_connect, [], [HAVE_JACK=false])
     AC_CHECK_HEADER(jack.h, [], [HAVE_JACK=false])])])

# RtAudio / RtMidi
AS_IF([$HAVE_ALSA],
  [RTAUDIOFLAGS="-D__LINUX_ALSA__";
   RTMIDIFLAGS="-D__LINUX_ALSASEQ__"])
AS_IF([$HAVE_JACK],
  [RTAUDIOFLAGS="$RTAUDIOFLAGS -D__UNIX_JACK__"])

AC_SUBST(RTAUDIOFLAGS)
AC_SUBST(RTMIDIFLAGS)

# detect 64-bit system
AC_CHECK_SIZEOF(long)
if test $ac_cv_sizeof_long = 8; then
   BITSIZE=64
fi

# debug mode
AC_ARG_ENABLE(debug,
  [AC_HELP_STRING([--enable-debug],[compile in debug mode])]
  [debug=yes])
if test x$debug = xyes; then
  CPPFLAGS="-g -O0 -Wall -DDEBUG $CPPFLAGS"
fi

# Generate Makefiles
AC_CONFIG_FILES([
    Makefile
    gui/Makefile
    rtmidi/Makefile
    rtaudio/Makefile
])

# Add version string and other flags
CFLAGS="$CFLAGS -D_BUILDVER=\\\"LOOPDUB_VER\\\" -DFLAT_GUI"
CPPFLAGS="$CPPFLAGS -D_BUILDVER=\\\"LOOPDUB_VER\\\" -DFLAT_GUI"

AC_OUTPUT()

echo
echo ======================================
echo Done configuring loopdub.
AS_IF($HAVE_ALSA, [echo Compiling with ALSA support.])
AS_IF($HAVE_JACK, [echo Compiling with Jack support.])
AS_IF($HAVE_ALSA || $HAVE_JACK, [],
  [echo Warning: No sound libraries are included, LoopDub will be silent!])
echo ======================================
echo
