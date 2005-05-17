
# change this to install to a different folder
PREFIX = /usr/share
LAUNCHER_PREFIX = /usr/bin

CC = g++
GUIDIR = gui
GUILIB = -L$(GUIDIR) -lscrobui
PMDIR = portmidi
PMLIB = -Lportmidi/pm_linux/ -lportmidi -Lportmidi/porttime -lporttime
PALIB =  -L/usr/lib/ -lportaudio
SDLLIB = -lSDL
ALSALIB = -lasound
LIBS = $(SDLLIB) $(GUILIB) $(PMLIB) $(PALIB) $(ALSALIB) 
CPPFLAGS = -g -ggdb -I $(GUIDIR) -I /usr/include/SDL
LDFLAGS = $(LIBS)
SRC = loopdub.cpp loopob.cpp sample.cpp player.cpp ld_midi.cpp filter.cpp
OBJ = $(SRC:.cpp=.o)

all: loopdub
#	./loopdub

libscrobui.a: $(shell ls $(GUIDIR)/*{.h,.cpp})
	$(MAKE) -C $(GUIDIR)

portmidi: portmidi
	unzip -q portmidi17nov04.zip

libportmidi.a: portmidi
	$(MAKE) -C $(PMDIR)

loopdub: libscrobui.a libportmidi.a $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@

.PHONY: clean
clean:
	$(MAKE) -C $(GUIDIR) clean
	-rm *.o *~
	-rm -rf portmidi

install: loopdub
	sudo bash install.sh "$(PREFIX)" "$(LAUNCHER_PREFIX)"

