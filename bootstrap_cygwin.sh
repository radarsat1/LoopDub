#!/bin/bash

DXDIR="$1"

if [ "$DXDIR"x == "x" ]; then
	echo
	echo "This script bootstraps a development environment for LoopDub under"
B	echo "under Cygwin on Windows. That is, it will download tarballs of"
	echo "needed libraries from various places on the web and compile them."
	echo "It won't install anything outside the current folder, so you do"
	echo "not need to worry about damaging your system."
	echo
	echo "It needs the following packages  pre-installed:"
	echo
	echo "make"
	echo "gcc"
	echo "python"
	echo
	echo "You must also install the DirectX SDK somewhere, and provide"
	echo "the location as the first argument to this script."
	echo 'For example, "/cygdrive/c/Program Files/DXSDK",'
	echo 'or possibly, "/cygdrive/c/Program Files/Microsoft DirectX SDK (August 2006)"'
	echo
	echo "Usage: ./bootstrap_cygwin.sh <location of DirectX SDK root>"
	echo
	exit
fi


# Package locations
PKG_SCONS_TAR="scons-local-0.96.1.tar.gz"
PKG_SCONS_DIR="scons-local-0.96.1"
PKG_SCONS_URL="http://superb-west.dl.sourceforge.net/sourceforge/scons/scons-local-0.96.1.tar.gz"
PKG_SCONS_MD5="78754efc02b4a374d5082a61509879cd"

PKG_RTAUDIO_TAR="rtaudio-3.0.3.tar.gz"
PKG_RTAUDIO_MD5="dcc08fa6b81971eabacc872acd56319a"
PKG_RTAUDIO_DIR="rtaudio-3.0.3"
PKG_RTAUDIO_URL="http://music.mcgill.ca/~gary/rtaudio/release/rtaudio-3.0.3.tar.gz"
PKG_RTAUDIO_LIB="$PKG_RTAUDIO_DIR/librtaudio.a"

PKG_RTMIDI_TAR="rtmidi-1.0.6.tar.gz"
PKG_RTMIDI_MD5="3f433b3d398c6ee73fccf218438f796e"
PKG_RTMIDI_DIR="rtmidi-1.0.6"
PKG_RTMIDI_URL="http://music.mcgill.ca/~gary/rtmidi/release/rtmidi-1.0.6.tar.gz"
PKG_RTMIDI_LIB="$PKG_RTMIDI_DIR/librtmidi.a"

PKG_LIBSNDFILE_TAR="libsndfile-1.0.17.tar.gz"
PKG_LIBSNDFILE_MD5="2d126c35448503f6dbe33934d9581f6b"
PKG_LIBSNDFILE_DIR="libsndfile-1.0.17"
PKG_LIBSNDFILE_URL="http://www.mega-nerd.com/libsndfile/libsndfile-1.0.17.tar.gz"
PKG_LIBSNDFILE_LIB="$PKG_LIBSNDFILE_DIR/src/.libs/libsndfile.a"

PKG_SDL_TAR="SDL-1.2.11.tar.gz"
PKG_SDL_MD5="418b42956b7cd103bfab1b9077ccc149"
PKG_SDL_DIR="SDL-1.2.11"
PKG_SDL_LIB="$PKG_SDL_DIR/build/.libs/libSDL.a"
PKG_SDL_URL="http://www.libsdl.org/release/SDL-1.2.11.tar.gz"

# Build everything
make -C libdeps -f ./bootstrap_cygwin.mk DXDIR="$DXDIR" \
 PKG_SCONS_TAR="$PKG_SCONS_TAR" PKG_SCONS_DIR="$PKG_SCONS_DIR" PKG_SCONS_URL="$PKG_SCONS_URL" PKG_SCONS_MD5="$PKG_SCONS_MD5" \
 PKG_RTAUDIO_TAR="$PKG_RTAUDIO_TAR" PKG_RTAUDIO_DIR="$PKG_RTAUDIO_DIR" PKG_RTAUDIO_URL="$PKG_RTAUDIO_URL" PKG_RTAUDIO_MD5="$PKG_RTAUDIO_MD5" PKG_RTAUDIO_LIB="$PKG_RTAUDIO_LIB" \
 PKG_RTMIDI_TAR="$PKG_RTMIDI_TAR" PKG_RTMIDI_DIR="$PKG_RTMIDI_DIR" PKG_RTMIDI_URL="$PKG_RTMIDI_URL" PKG_RTMIDI_MD5="$PKG_RTMIDI_MD5" PKG_RTMIDI_LIB="$PKG_RTMIDI_LIB" \
 PKG_LIBSNDFILE_TAR="$PKG_LIBSNDFILE_TAR" PKG_LIBSNDFILE_DIR="$PKG_LIBSNDFILE_DIR" PKG_LIBSNDFILE_URL="$PKG_LIBSNDFILE_URL" PKG_LIBSNDFILE_MD5="$PKG_LIBSNDFILE_MD5" PKG_LIBSNDFILE_LIB="$PKG_LIBSNDFILE_LIB" \
 PKG_SDL_TAR="$PKG_SDL_TAR" PKG_SDL_DIR="$PKG_SDL_DIR" PKG_SDL_URL="$PKG_SDL_URL" PKG_SDL_MD5="$PKG_SDL_MD5" PKG_SDL_LIB="$PKG_SDL_LIB"

