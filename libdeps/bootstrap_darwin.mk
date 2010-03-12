
PACKAGES = libsndfile libsamplerate sdl
MD5 = md5

all: loopdub

loopdub: $(PACKAGES)
	@echo
	@echo
	@echo "Dependancies are all compiled. LoopDub should now be able to compile and link. Type './configure' to proceed."
	@echo
	@echo


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

