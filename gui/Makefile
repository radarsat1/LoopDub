
CC = g++
LIBS = -lSDL
CPPFLAGS = -fPIC -g -ggdb
# note: need to fix the deprecated dependancy on <fstream.h>
CPPFLAGS += -Wno-deprecated
LDFLAGS = $(LIBS)
SRC = scrobui.cpp toolkit.cpp
OBJ = $(SRC:.cpp=.o)

FLAT_GUI=1
ifeq ($(FLAT_GUI),1)
CPPFLAGS += -DFLAT_GUI
endif

all: libscrobui.a

.o: .cpp

libscrobui.a: $(OBJ)
	ar -ruv libscrobui.a $^

clean:
	-rm *.o *~ *.a
