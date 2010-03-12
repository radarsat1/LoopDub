
PACKAGES = libsndfile libsamplerate sdl sdldx
MD5 = md5sum

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
	cd $(shell cygpath -u '$(shell cygpath -asw "$(PKG_LIBSNDFILE_DIR)")'); make

$(PKG_LIBSNDFILE_DIR)/Makefile: libsndfile.unpacked
	cd $(PKG_LIBSNDFILE_DIR); CFLAGS='-mno-cygwin -D__CYGWIN__ -DS_ISSOCK\(m\) -DNDEBUG' CXXFLAGS="$$CFLAGS" ./configure --disable-shared

	# couple of patches
	sed 's/SF_COUNT_MAX.*/SF_COUNT_MAX 0x7FFFFFFF/' --in-place $(PKG_LIBSNDFILE_DIR)/src/sndfile.h
	sed 's/int64_t/__int64/' --in-place $(PKG_LIBSNDFILE_DIR)/tests/utils.h

libsndfile.unpacked: libsndfile.verified
	@echo "Unpacking libsndfile..."
	tar -xzf $(PKG_LIBSNDFILE_TAR)
	touch libsndfile.unpacked

libsndfile.verified:
	@echo "Getting libsndfile..."
	wget $(PKG_LIBSNDFILE_URL) -O $(PKG_LIBSNDFILE_TAR)
	if [ `$(MD5) $(PKG_LIBSNDFILE_TAR) | cut -d" " -f1`x == $(PKG_LIBSNDFILE_MD5)x ]; then touch libsndfile.verified; else echo "MD5 error on $(PKG_LIBSNDFILE_TAR)"; false; fi


# libsamplerate
libsamplerate: $(PKG_LIBSAMPLERATE_LIB)
	@if [ -f "$(PKG_LIBSAMPLERATE_LIB)" ]; then echo "libsamplerate verified."; else echo "Error processing libsamplerate."; false; fi

$(PKG_LIBSAMPLERATE_LIB): $(PKG_LIBSAMPLERATE_DIR)/Makefile
# path conversion required here because the build system doesn't like paths with spaces in them
# (thanks to cygpath)
	cd $(shell cygpath -u '$(shell cygpath -asw "$(PKG_LIBSAMPLERATE_DIR)")'); make

$(PKG_LIBSAMPLERATE_DIR)/Makefile: libsamplerate.unpacked
	cd $(PKG_LIBSAMPLERATE_DIR); CFLAGS='-mno-cygwin -D__CYGWIN__ -DS_ISSOCK\(m\) -DNDEBUG' CXXFLAGS="$$CFLAGS" ./configure --disable-shared

libsamplerate.unpacked: libsamplerate.verified
	@echo "Unpacking libsamplerate..."
	tar -xzf $(PKG_LIBSAMPLERATE_TAR)
	touch libsamplerate.unpacked

libsamplerate.verified:
	@echo "Getting libsamplerate..."
	wget $(PKG_LIBSAMPLERATE_URL) -O $(PKG_LIBSAMPLERATE_TAR)
	if [ `$(MD5) $(PKG_LIBSAMPLERATE_TAR) | cut -d" " -f1`x == $(PKG_LIBSAMPLERATE_MD5)x ]; then touch libsamplerate.verified; else echo "MD5 error on $(PKG_LIBSAMPLERATE_TAR)"; false; fi


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
	if [ `$(MD5) $(PKG_SDL_TAR) | cut -d" " -f1`x == $(PKG_SDL_MD5)x ]; then touch sdl.verified; else echo "MD5 error on $(PKG_SDL_TAR)"; false; fi

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
	if [ `$(MD5) $(PKG_SDLDX_TAR) | cut -d" " -f1`x == $(PKG_SDLDX_MD5)x ]; then touch sdldx.verified; else echo "MD5 error on $(PKG_SDLDX_TAR)"; false; fi
