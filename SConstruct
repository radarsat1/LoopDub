
if 'debug' in ARGUMENTS:
	CCFLAGS = '-g -ggdb '
else:
	CCFLAGS = ''

LIBS=['scrobui','SDL','SDLmain','sndfile','portmidi','porttime']
LIBPATH=['gui','portmidi/porttime']
LDFLAGS=[]
CPPPATH=['gui','.']

#cygwin
CCFLAGS += '-DWIN32 -DCYGWIN '
LIBS += ['winmm','rtaudio','rtmidi','SDL']
LIBPATH += ['portmidi','libdeps/rtaudio-3.0.3','libdeps/rtmidi-1.0.6','libdeps/SDL-1.2.11/build/.libs']
CPPPATH += ['libdeps/SDL-1.2.11/include','libdeps/rtaudio-3.0.3/']

#linux
#LIBS += ['jack']
#LIBPATH += ['portmidi/pm_linux']

SConscript(['gui/SConstruct'], 'CCFLAGS')

Program('loopdub',
	['loopdub.cpp', 'filter.cpp', 'ld_midi.cpp', 'loopob.cpp', 'player.cpp',
	 'program.cpp', 'sample.cpp', 'vumeter.cpp', 'platform.cpp'],
	CPPPATH=CPPPATH,
	CCFLAGS=CCFLAGS,
	LIBS=LIBS,
	LIBPATH=LIBPATH,
	LDFLAGS=LDFLAGS
	)
