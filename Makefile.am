
SUBDIRS = gui rtaudio rtmidi

bin_PROGRAMS = loopdub

loopdub_CXXFLAGS = -Wall -I$(top_srcdir)/gui \
	-I$(top_srcdir)/rtaudio \
	-I$(top_srcdir)/rtmidi
loopdub_SOURCES = loopdub.cpp filter.cpp ld_midi.cpp loopob.cpp	\
	platform.cpp player.cpp program.cpp sample.cpp settings.cpp	\
	vumeter.cpp
loopdub_LDADD = $(top_srcdir)/gui/libscrobui.la \
	$(top_srcdir)/rtaudio/librtaudio.la \
	$(top_srcdir)/rtmidi/librtmidi.la
