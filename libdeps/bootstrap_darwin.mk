
PACKAGES = scons rtaudio rtmidi libsndfile sdl
MD5 = md5

all: loopdub

loopdub: $(PACKAGES)
	@echo
	@echo
	@echo "Dependancies are all compiled. LoopDub should now be able to compile and link. Type './scons' to proceed."
	@echo
	@echo

# RtAudio
rtaudio: $(PKG_RTAUDIO_LIB)
	@if [ -f $(PKG_RTAUDIO_LIB) ]; then echo "RtAudio verified."; else echo "Error processing RtAudio."; false; fi

$(PKG_RTAUDIO_LIB): rtaudio.unpacked
#	Replace the RtAudio build system with Scons.
#	This script can be pointed to the dsound header.
	echo "StaticLibrary('rtaudio', ['RtAudio.cpp'], CCFLAGS='-D__MACOSX_CORE__')" >$(PKG_RTAUDIO_DIR)/SConstruct
	cd $(PKG_RTAUDIO_DIR); ../scons.py

rtaudio.unpacked: rtaudio.verified
	@echo "Unpacking RtAudio..."
	tar -xzf $(PKG_RTAUDIO_TAR)
	touch rtaudio.unpacked

rtaudio.verified:
	@echo "Getting RtAudio..."
	curl $(PKG_RTAUDIO_URL) -o $(PKG_RTAUDIO_TAR)
	if [ `$(MD5) $(PKG_RTAUDIO_TAR) | cut -d" " -f4`x = $(PKG_RTAUDIO_MD5)x ]; then touch rtaudio.verified; else echo "MD5 error on $(PKG_RTAUDIO_TAR)"; false; fi


# RtMidi
rtmidi: $(PKG_RTMIDI_LIB)
	@if [ -f $(PKG_RTMIDI_LIB) ]; then echo "RtMidi verified."; else echo "Error processing RtMidi."; false; fi

$(PKG_RTMIDI_LIB): rtmidi.unpacked
#	Replace the RtMidi build system with Scons.
	echo "StaticLibrary('rtmidi', ['RtMidi.cpp'], CCFLAGS='-D__MACOSX_CORE__')" >$(PKG_RTMIDI_DIR)/SConstruct
	cd $(PKG_RTMIDI_DIR); ../scons.py

rtmidi.unpacked: rtmidi.verified
	@echo "Unpacking RtMidi..."
	tar -xzf $(PKG_RTMIDI_TAR)
	touch rtmidi.unpacked

rtmidi.verified:
	@echo "Getting RtMidi..."
	curl $(PKG_RTMIDI_URL) -o $(PKG_RTMIDI_TAR)
	if [ `$(MD5) $(PKG_RTMIDI_TAR) | cut -d" " -f4`x = $(PKG_RTMIDI_MD5)x ]; then touch rtmidi.verified; else echo "MD5 error on $(PKG_RTMIDI_TAR)"; false; fi


# Scons - local version
scons: scons.py
	@if (./scons.py -v); then echo "Scons verified."; else echo "Error running Scons."; false; fi

scons.py: scons.verified
	@echo "Unpacking Scons..."
	tar -xzf $(PKG_SCONS_TAR)
	chmod +x scons.py
	if ! [ -h ../scons ]; then ln -s "$(shell pwd)/scons.py" ../scons; fi

scons.verified:
	@echo "Getting Scons..."
	curl $(PKG_SCONS_URL) -o $(PKG_SCONS_TAR)
	if [ `$(MD5) $(PKG_SCONS_TAR) | cut -d" " -f4`x = $(PKG_SCONS_MD5)x ]; then touch scons.verified; else echo "MD5 error on $(PKG_SCONS_TAR)"; false; fi


# libsndfile
libsndfile: $(PKG_LIBSNDFILE_LIB)
	@if [ -f "$(PKG_LIBSNDFILE_LIB)" ]; then echo "libsndfile verified."; else echo "Error processing libsndfile."; false; fi

$(PKG_LIBSNDFILE_LIB): $(PKG_LIBSNDFILE_DIR)/Makefile
# path conversion required here because the build system doesn't like paths with spaces in them
# (thanks to cygpath)
	cd "$(PKG_LIBSNDFILE_DIR)"; make

$(PKG_LIBSNDFILE_DIR)/Makefile: libsndfile.unpacked
	cd $(PKG_LIBSNDFILE_DIR); ./configure --disable-shared

libsndfile.unpacked: libsndfile.verified
	@echo "Unpacking libsndfile..."
	tar -xzf $(PKG_LIBSNDFILE_TAR)
	touch libsndfile.unpacked

libsndfile.verified:
	@echo "Getting libsndfile..."
	curl $(PKG_LIBSNDFILE_URL) -o $(PKG_LIBSNDFILE_TAR)
	if [ `$(MD5) $(PKG_LIBSNDFILE_TAR) | cut -d" " -f4`x = $(PKG_LIBSNDFILE_MD5)x ]; then touch libsndfile.verified; else echo "MD5 error on $(PKG_LIBSNDFILE_TAR)"; false; fi

# libsamplerate
libsamplerate: $(PKG_LIBSAMPLERATE_LIB)
	@if [ -f "$(PKG_LIBSAMPLERATE_LIB)" ]; then echo "libsamplerate verified."; else echo "Error processing libsamplerate."; false; fi

$(PKG_LIBSAMPLERATE_LIB): $(PKG_LIBSAMPLERATE_DIR)/Makefile
# path conversion required here because the build system doesn't like paths with spaces in them
# (thanks to cygpath)
	cd "$(PKG_LIBSAMPLERATE_DIR)"; make

$(PKG_LIBSAMPLERATE_DIR)/Makefile: libsamplerate.unpacked
	cd $(PKG_LIBSAMPLERATE_DIR); ./configure --disable-shared

libsamplerate.unpacked: libsamplerate.verified
	@echo "Unpacking libsamplerate..."
	tar -xzf $(PKG_LIBSAMPLERATE_TAR)
	touch libsamplerate.unpacked

libsamplerate.verified:
	@echo "Getting libsamplerate..."
	curl $(PKG_LIBSAMPLERATE_URL) -o $(PKG_LIBSAMPLERATE_TAR)
	if [ `$(MD5) $(PKG_LIBSAMPLERATE_TAR) | cut -d" " -f4`x = $(PKG_LIBSAMPLERATE_MD5)x ]; then touch libsamplerate.verified; else echo "MD5 error on $(PKG_LIBSAMPLERATE_TAR)"; false; fi


# SDL
sdl: $(PKG_SDL_LIB)
	@if [ -f $(PKG_SDL_LIB) ]; then echo "SDL verified."; else echo "Error processing SDL."; false; fi

$(PKG_SDL_LIB): $(PKG_SDL_DIR)/Makefile
	cd $(PKG_SDL_DIR); make

$(PKG_SDL_DIR)/Makefile: sdl.unpacked
#	disable X11, which we aren't using
	cd $(PKG_SDL_DIR); ./configure --disable-video-x11 --disable-shared

sdl.unpacked: sdl.verified
	@echo "Unpacking SDL..."
	tar -xzf $(PKG_SDL_TAR)
	touch sdl.unpacked

sdl.verified:
	@echo "Getting SDL..."
	curl $(PKG_SDL_URL) -o $(PKG_SDL_TAR)
	if [ `$(MD5) $(PKG_SDL_TAR) | cut -d" " -f4`x = $(PKG_SDL_MD5)x ]; then touch sdl.verified; else echo "MD5 error on $(PKG_SDL_TAR)"; false; fi

