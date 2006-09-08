

SUBDIRS = gui
EXTRA_LIBS = $(PM_EXTRA)
LDADD = gui/libscrobui.a $(EXTRA_LIBS)
AM_CPPFLAGS = -I$(srcdir)/gui/ -I$(SDL_LOCATION) -Iportmidi/pm_common
bin_PROGRAMS = loopdub
loopdub_SOURCES = filter.cpp \
	ld_midi.cpp \
	loopdub.cpp \
	loopob.cpp \
	platform.cpp \
	player.cpp \
	program.cpp \
	sample.cpp \
	vumeter.cpp \
	config.h \
	def.h \
	filter.h \
	ld_logo.h \
	ld_midi.h \
	loopdub.h \
	loopob.h \
	platform.h \
	player.h \
	program.h \
	sample.h \
	vumeter.h

dist_pkgdata_DATA = Dotum10.raw loopdub.midi.conf.example programs.ini.example
EXTRA_DIST = portmidi17nov04.zip
