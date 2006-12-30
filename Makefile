
.PHONY: loopdub clean debug
loopdub:
	-scons
#	@echo g++ -o loopdub.exe loopdub.o filter.o ld_midi.o loopob.o player.o program.o sample.o vumeter.o platform.o -Lgui -Lportmidi/porttime "-L/cygdrive/c/Documents and Settings/SSteve/My Documents/projects/stk-4.2.1/src" -Lportmidi -lscrobui -lSDL -lSDLmain -lsndfile -lstk -lportmidi -lporttime -lwinmm ../stk-4.2.1/src/RtAudio.o -ldsound -lole32;
#	if g++ -o loopdub.exe loopdub.o filter.o ld_midi.o loopob.o player.o program.o sample.o vumeter.o platform.o -Lgui -Lportmidi/porttime "-L/cygdrive/c/Documents and Settings/SSteve/My Documents/projects/stk-4.2.1/src" -Lportmidi -lscrobui -lSDL -lSDLmain -lsndfile -lstk -lportmidi -lporttime -lwinmm ../stk-4.2.1/src/RtAudio.o -ldsound -lole32; then echo "Linked successfully."; fi

debug:
	-scons debug=1
#	@echo g++ -o loopdub.exe loopdub.o filter.o ld_midi.o loopob.o player.o program.o sample.o vumeter.o platform.o -Lgui -Lportmidi/porttime "-L/cygdrive/c/Documents and Settings/SSteve/My Documents/projects/stk-4.2.1/src" -Lportmidi -lscrobui -lSDL -lSDLmain -lsndfile -lstk -lportmidi -lporttime -lwinmm ../stk-4.2.1/src/RtAudio.o -ldsound -lole32;
#	if g++ -o loopdub.exe loopdub.o filter.o ld_midi.o loopob.o player.o program.o sample.o vumeter.o platform.o -Lgui -Lportmidi/porttime "-L/cygdrive/c/Documents and Settings/SSteve/My Documents/projects/stk-4.2.1/src" -Lportmidi -lscrobui -lSDL -lSDLmain -lsndfile -lstk -lportmidi -lporttime -lwinmm ../stk-4.2.1/src/RtAudio.o -ldsound -lole32; then echo "Linked successfully."; fi

clean:
	scons -c
