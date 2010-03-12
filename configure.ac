AC_PREREQ(2.57)

AC_DEFUN([LOOPDUB_VER],[m4_esyscmd([util/version.sh | tr -d '\n'])])
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

PKG_CHECK_MODULES([alsa], [alsa >= 1.0.20],
  [CFLAGS="$CFLAGS $alsa_CFLAGS"
   CPPFLAGS="$CPPFLAGS $alsa_CFLAGS"
   LDFLAGS="$LDFLAGS $alsa_LIBS"],
  [AC_CHECK_LIB(asound,snd_config_load, [],
    [AC_MSG_ERROR([ALSA is required.])])
   AC_CHECK_HEADER(asoundlib.h, [],
    [AC_MSG_ERROR([ALSA is required.])])])

PKG_CHECK_MODULES([jack], [jack >= 0.116.1],
  [CFLAGS="$CFLAGS $jack_CFLAGS"
   CPPFLAGS="$CPPFLAGS $jack_CFLAGS"
   LDFLAGS="$LDFLAGS $jack_LIBS"],
  [AC_CHECK_LIB(jack,jack_connect, [],
    [AC_MSG_ERROR([Jack is required.])])
   AC_CHECK_HEADER(jack.h, [],
    [AC_MSG_ERROR([Jack is required.])])])

PKG_CHECK_MODULES([sndfile], [sndfile >= 0.116.1],
  [CFLAGS="$CFLAGS $sndfile_CFLAGS"
   CPPFLAGS="$CPPFLAGS $sndfile_CFLAGS"
   LDFLAGS="$LDFLAGS $sndfile_LIBS"],
  [AC_CHECK_LIB(sndfile,sf_open, [],
    [AC_MSG_ERROR([libsndfile is required.])])
   AC_CHECK_HEADER(sndfile.h, [],
    [AC_MSG_ERROR([libsndfile is required.])])])

# detect 64-bit system
AC_CHECK_SIZEOF(long)
if test $ac_cv_sizeof_long = 8; then
   BITSIZE=64
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
echo ======================================
echo
