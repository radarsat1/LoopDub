
SUBDIRS = gui rtaudio rtmidi

bin_PROGRAMS = loopdub

dist_doc_DATA = loopdub.conf.example loopdub.midi.conf.example	\
    docs/index.html docs/radarsat1_brussels_legazon6.jpg		\
    docs/screenshot.png

EXTRA_DIST = bootstrap.sh libdeps/bootstrap_cygwin.mk		\
    libdeps/bootstrap_darwin.mk libdeps/bootstrap_linux.mk	\
    README_win32.txt

loopdub_CXXFLAGS = -Wall -I$(top_srcdir)/gui \
	-I$(top_srcdir)/rtaudio \
	-I$(top_srcdir)/rtmidi
loopdub_SOURCES = loopdub.cpp filter.cpp ld_midi.cpp loopob.cpp			\
	platform.cpp player.cpp program.cpp sample.cpp settings.cpp			\
	vumeter.cpp def.h filter.h ld_logo.h ld_midi.h loopdub.h loopob.h	\
	platform.h player.h program.h sample.h settings.h vumeter.h
loopdub_LDADD = $(top_srcdir)/gui/libscrobui.la \
	$(top_srcdir)/rtaudio/librtaudio.la \
	$(top_srcdir)/rtmidi/librtmidi.la
