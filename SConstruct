
import commands

OS=commands.getoutput('uname')

CCFLAGS = ''
if 'debug' in ARGUMENTS:
	CCFLAGS += '-g -ggdb '

LIBS=['scrobui','SDL','sndfile','rtaudio','rtmidi']
LIBPATH=['gui']
LDFLAGS=[]
CPPPATH=['gui','.']

if OS=='Cygwin':
	print 'Cygwin detected.'
	CCFLAGS += '-DWIN32 -DCYGWIN '
	LIBS += ['winmm','user32','gdi32','ntoskrnl']
	LIBPATH += ['libdeps/rtaudio-3.0.3',
				'libdeps/rtmidi-1.0.6',
				'libdeps/SDL-1.2.11/build/.libs',
				'libdeps/SDL-1.2.11/build/',
				'libdeps/libsndfile-1.0.17/src/.libs']
	CPPPATH += ['libdeps/SDL-1.2.11/include',
				'libdeps/rtaudio-3.0.3/',
				'libdeps/libsndfile-1.0.17/src']

if OS=='Linux':
	print 'Linux detected.'
	LIBS += ['asound']
	LIBPATH += ['libdeps/rtaudio-3.0.3',
				'libdeps/rtmidi-1.0.6']
	CPPPATH += ['libdeps/rtaudio-3.0.3/',
				'/usr/include/SDL']

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

