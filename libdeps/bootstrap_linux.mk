
PACKAGES = scons rtaudio rtmidi
MD5 = md5sum

all: loopdub

loopdub: $(PACKAGES)
	@echo
	@echo
	@echo "Dependancies are all compiled. LoopDub should now be able to compile and link. Type 'libdeps/scons.py' to proceed."
	@echo
	@echo

# RtAudio
rtaudio: $(PKG_RTAUDIO_LIB)
	@if [ -f $(PKG_RTAUDIO_LIB) ]; then echo "RtAudio verified."; else echo "Error processing RtAudio."; false; fi

$(PKG_RTAUDIO_LIB): rtaudio.unpacked
#	Replace the RtAudio build system with Scons.
#	This script can be pointed to the dsound header.
	echo "StaticLibrary('rtaudio', ['RtAudio.cpp'], CCFLAGS='-D__LINUX_ALSA__ -D__LINUX_JACK__')" >$(PKG_RTAUDIO_DIR)/SConstruct
	cd $(PKG_RTAUDIO_DIR); ../scons.py

rtaudio.unpacked: rtaudio.verified
	@echo "Unpacking RtAudio..."
	tar -xzf $(PKG_RTAUDIO_TAR)
	touch rtaudio.unpacked

rtaudio.verified:
	@echo "Getting RtAudio..."
	wget $(PKG_RTAUDIO_URL) -O $(PKG_RTAUDIO_TAR)
	if [ `$(MD5) $(PKG_RTAUDIO_TAR) | cut -d" " -f1`x = $(PKG_RTAUDIO_MD5)x ]; then touch rtaudio.verified; else echo "MD5 error on $(PKG_RTAUDIO_TAR)"; false; fi


# RtMidi
rtmidi: $(PKG_RTMIDI_LIB)
	@if [ -f $(PKG_RTMIDI_LIB) ]; then echo "RtMidi verified."; else echo "Error processing RtMidi."; false; fi

$(PKG_RTMIDI_LIB): rtmidi.unpacked
#	Replace the RtMidi build system with Scons.
	echo "StaticLibrary('rtmidi', ['RtMidi.cpp'], CCFLAGS='-D__LINUX_ALSASEQ__ -D__LINUX_JACK__')" >$(PKG_RTMIDI_DIR)/SConstruct
	cd $(PKG_RTMIDI_DIR); ../scons.py

rtmidi.unpacked: rtmidi.verified
	@echo "Unpacking RtMidi..."
	tar -xzf $(PKG_RTMIDI_TAR)
	touch rtmidi.unpacked

rtmidi.verified:
	@echo "Getting RtMidi..."
	wget $(PKG_RTMIDI_URL) -O $(PKG_RTMIDI_TAR)
	if [ `$(MD5) $(PKG_RTMIDI_TAR) | cut -d" " -f1`x = $(PKG_RTMIDI_MD5)x ]; then touch rtmidi.verified; else echo "MD5 error on $(PKG_RTMIDI_TAR)"; false; fi

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
	curl -L $(PKG_SCONS_URL) -o $(PKG_SCONS_TAR)
	if [ `$(MD5) $(PKG_SCONS_TAR) | cut -d" " -f1`x = $(PKG_SCONS_MD5)x ]; then touch scons.verified; else echo "MD5 error on $(PKG_SCONS_TAR)"; false; fi
