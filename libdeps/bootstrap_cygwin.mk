
PACKAGES = scons rtaudio rtmidi libsndfile sdl sdldx

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
	echo "StaticLibrary('rtaudio', ['RtAudio.cpp'], LIBS=['dsound'], CPPPATH=[ARGUMENTS['DXDIR']+'/Include'], LIBPATH=[ARGUMENTS['DXDIR']+'/Lib'],CXXFLAGS='-D__WINDOWS_DS__ -mno-cygwin')" >$(PKG_RTAUDIO_DIR)/SConstruct
	cd $(PKG_RTAUDIO_DIR); ../scons.py DXDIR="$(DXDIR)"

rtaudio.unpacked: rtaudio.verified
	@echo "Unpacking RtAudio..."
	tar -xzf $(PKG_RTAUDIO_TAR)
	touch rtaudio.unpacked

rtaudio.verified:
	@echo "Getting RtAudio..."
	if [ `md5sum $(PKG_RTAUDIO_TAR) | cut -d" " -f1`x != $(PKG_RTAUDIO_MD5)x ]; then wget $(PKG_RTAUDIO_URL) -O $(PKG_RTAUDIO_TAR); fi
	if [ `md5sum $(PKG_RTAUDIO_TAR) | cut -d" " -f1`x == $(PKG_RTAUDIO_MD5)x ]; then touch rtaudio.verified; else echo "MD5SUM error on $(PKG_RTAUDIO_TAR)"; false; fi


# RtMidi
rtmidi: $(PKG_RTMIDI_LIB)
	@if [ -f $(PKG_RTMIDI_LIB) ]; then echo "RtMidi verified."; else echo "Error processing RtMidi."; false; fi

$(PKG_RTMIDI_LIB): rtmidi.unpacked
#	Replace the RtMidi build system with Scons.
	echo "StaticLibrary('rtmidi', ['RtMidi.cpp'],CXXFLAGS='-D__WINDOWS_MM__ -mno-cygwin')" >$(PKG_RTMIDI_DIR)/SConstruct
	cd $(PKG_RTMIDI_DIR); ../scons.py DXDIR="$(DXDIR)"

rtmidi.unpacked: rtmidi.verified
	@echo "Unpacking RtMidi..."
	tar -xzf $(PKG_RTMIDI_TAR)
	touch rtmidi.unpacked

rtmidi.verified:
	@echo "Getting RtMidi..."
	if [ `md5sum $(PKG_RTMIDI_TAR) | cut -d" " -f1`x != $(PKG_RTMIDI_MD5)x ]; then wget $(PKG_RTMIDI_URL) -O $(PKG_RTMIDI_TAR); fi
	if [ `md5sum $(PKG_RTMIDI_TAR) | cut -d" " -f1`x == $(PKG_RTMIDI_MD5)x ]; then touch rtmidi.verified; else echo "MD5SUM error on $(PKG_RTMIDI_TAR)"; false; fi


# Scons - local version
scons: scons.py
	@if (./scons.py -v); then echo "Scons verified."; else echo "Error running Scons."; false; fi

scons.py: scons.verified
	@echo "Unpacking Scons..."
	tar -xzf $(PKG_SCONS_TAR)
	chmod +x scons.py
	if ! [ -h ../scons ]; then ln -s "$(shell cygpath -u $(shell pwd))/scons.py" ../scons; fi

scons.verified:
	@echo "Getting Scons..."
	wget $(PKG_SCONS_URL) -O $(PKG_SCONS_TAR)
	if [ `md5sum $(PKG_SCONS_TAR) | cut -d" " -f1`x == $(PKG_SCONS_MD5)x ]; then touch scons.verified; else echo "MD5SUM error on $(PKG_SCONS_TAR)"; false; fi


# libsndfile
libsndfile: $(PKG_LIBSNDFILE_LIB)
	@if [ -f "$(PKG_LIBSNDFILE_LIB)" ]; then echo "libsndfile verified."; else echo "Error processing libsndfile."; false; fi

$(PKG_LIBSNDFILE_LIB): $(PKG_LIBSNDFILE_DIR)/Makefile
# path conversion required here because the build system doesn't like paths with spaces in them
# (thanks to cygpath)
	cd $(shell cygpath -u '$(shell cygpath -asw "$(PKG_LIBSNDFILE_DIR)")'); make

$(PKG_LIBSNDFILE_DIR)/Makefile: libsndfile.unpacked
	cd $(PKG_LIBSNDFILE_DIR); ./configure --disable-shared

libsndfile.unpacked: libsndfile.verified
	@echo "Unpacking libsndfile..."
	tar -xzf $(PKG_LIBSNDFILE_TAR)
	touch libsndfile.unpacked

libsndfile.verified:
	@echo "Getting libsndfile..."
	wget $(PKG_LIBSNDFILE_URL) -O $(PKG_LIBSNDFILE_TAR)
	if [ `md5sum $(PKG_LIBSNDFILE_TAR) | cut -d" " -f1`x == $(PKG_LIBSNDFILE_MD5)x ]; then touch libsndfile.verified; else echo "MD5SUM error on $(PKG_LIBSNDFILE_TAR)"; false; fi


# SDL
sdl: $(PKG_SDL_LIB)
	@if [ -f $(PKG_SDL_LIB) ]; then echo "SDL verified."; else echo "Error processing SDL."; false; fi

$(PKG_SDL_LIB): $(PKG_SDL_DIR)/Makefile sdldx
	cd $(PKG_SDL_DIR); make

$(PKG_SDL_DIR)/Makefile: sdl.unpacked
#	disable a few features that we aren't using but are causing linking problems
	cd $(PKG_SDL_DIR); ./configure --disable-threads --disable-diskaudio --enable-directx --disable-shared

sdl.unpacked: sdl.verified
	@echo "Unpacking SDL..."
	tar -xzf $(PKG_SDL_TAR)
	touch sdl.unpacked

sdl.verified:
	@echo "Getting SDL..."
	wget $(PKG_SDL_URL) -O $(PKG_SDL_TAR)
	if [ `md5sum $(PKG_SDL_TAR) | cut -d" " -f1`x == $(PKG_SDL_MD5)x ]; then touch sdl.verified; else echo "MD5SUM error on $(PKG_SDL_TAR)"; false; fi

# SDL DirectX includes
sdldx: sdldx.unpacked
	@if [ -f $(PKG_SDLDX_LIB) ]; then echo "SDL DirectX verified."; else echo "Error processing SDL DirectX."; false; fi

sdldx.unpacked: sdldx.verified sdl.unpacked
	@echo "Unpacking SDL DirectX..."
	cd $(PKG_SDL_DIR); tar -xzf ../$(PKG_SDLDX_TAR)
	touch sdldx.unpacked

sdldx.verified:
	@echo "Getting SDL DirectX..."
	wget $(PKG_SDLDX_URL) -O $(PKG_SDLDX_TAR)
	if [ `md5sum $(PKG_SDLDX_TAR) | cut -d" " -f1`x == $(PKG_SDLDX_MD5)x ]; then touch sdldx.verified; else echo "MD5SUM error on $(PKG_SDLDX_TAR)"; false; fi
