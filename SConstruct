
if 'debug' in ARGUMENTS:
	CCFLAGS = '-g -ggdb '
else:
	CCFLAGS = ''

SConscript(['gui/SConstruct'], 'CCFLAGS')

Program('loopdub',
	['loopdub.cpp', 'filter.cpp', 'ld_midi.cpp', 'loopob.cpp', 'player.cpp',
	 'program.cpp', 'sample.cpp', 'vumeter.cpp', 'platform.cpp'],
	CPPPATH=['/usr/include/SDL','/usr/include/stk',
			 '../stk-4.2.1/include/', 'gui','.'],
	LIBS=['scrobui','SDL','sndfile','stk','portmidi','porttime','jack'],
	LIBPATH=['gui','portmidi/pm_linux','portmidi/porttime','../stk-4.2.1/src'],
	CCFLAGS=CCFLAGS
	)
