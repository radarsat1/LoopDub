
import commands
import os
import SCons.Util

"""
if os.environ.has_key('CC'):
	env['CC'] = os.environ['CC']
if os.environ.has_key('CFLAGS'):
	env['CCFLAGS'] += SCons.Util.CLVar(os.environ['CFLAGS'])
if os.environ.has_key('CC'):
	env['CC'] = os.environ['CC']
if os.environ.has_key('CCFLAGS'):
	env['CCFLAGS'] += SCons.Util.CLVar(os.environ['CCFLAGS'])
if os.environ.has_key('LDFLAGS'):
	env['LINKFLAGS'] += SCons.Util.CLVar(os.environ['LDFLAGS'])
"""

OS=commands.getoutput('uname')
if ('CYGWIN' in OS): OS='Cygwin'

CCFLAGS = ''
if 'debug' in ARGUMENTS:
	CCFLAGS += '-g -ggdb '
else:
	CCFLAGS += '-O3 '

LIBS=['scrobui','SDL','sndfile','rtaudio','rtmidi','samplerate']
LINKFLAGS=''
LIBPATH=['gui']
CPPPATH=['gui','.']

if OS=='Cygwin':
	print 'Cygwin detected.'
	CCFLAGS += '-DWIN32 -DCYGWIN -mno-cygwin '
	LINKFLAGS += '-mno-cygwin '
	LIBS += ['winmm','user32','gdi32','ntoskrnl','dsound','ole32']

if OS=='Darwin':
	print 'Darwin detected.'
	LINKFLAGS += (	'-framework CoreAudio -framework CoreMIDI -framework Carbon ' +
			'-framework AppKit -framework AudioUnit -framework QuickTime ' +
			'-framework IOKit -framework OpenGL'  )
	LIBS += ['SDLmain']

if OS=='Cygwin' or OS=='Darwin':
	LIBPATH += ['libdeps/rtaudio-3.0.3',
				'libdeps/rtmidi-1.0.6',
				'libdeps/SDL-1.2.11/build/.libs',
				'libdeps/SDL-1.2.11/build/',
				'libdeps/libsndfile-1.0.17/src/.libs',
				'libdeps/libsamplerate-0.1.2/src/.libs']
	CPPPATH += ['libdeps/SDL-1.2.11/include',
				'libdeps/rtaudio-3.0.3/',
				'libdeps/libsndfile-1.0.17/src',
				'libdeps/libsamplerate-0.1.2/src']

if OS=='Linux':
	print 'Linux detected.'
	LIBS += ['asound','jack']
	LIBPATH += ['libdeps/rtaudio-3.0.3',
				'libdeps/rtmidi-1.0.6']
	CPPPATH += ['libdeps/rtaudio-3.0.3/',
				'/usr/include/SDL']

SConscript(['gui/SConstruct'], 'CCFLAGS')

Program('loopdub',
	['loopdub.cpp', 'filter.cpp', 'ld_midi.cpp', 'loopob.cpp', 'player.cpp',
	 'program.cpp', 'sample.cpp', 'vumeter.cpp', 'platform.cpp', 'settings.cpp'],
	CPPPATH=CPPPATH,
	CCFLAGS=CCFLAGS,
	LIBS=LIBS,
	LIBPATH=LIBPATH,
	LINKFLAGS=LINKFLAGS
	)

