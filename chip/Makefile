BIN := CPU
OBJS := accessors.o control_path.o data_path.o FlipRegister.o microcontroller.o
CPPFLAGS = -I${ARCH_HEADERS_PATH}
CXXFLAGS := -std=c++98 -Wall -Wextra
LDFLAGS := -L.
LDLIBS := -larch2-5a

#CXXFLAGS += -g
#CPPFLAGS += -DDEBUG_ARCH
#CPPFLAGS += -DDEBUG_SWITCHES
#CPPFLAGS += -DDEBUG_DUMPGPRS
#CPPFLAGS += -DDEBUG_DUMPCNTL
#CPPFLAGS += -DDEBUG_DUMPMDR

${BIN}: ${OBJS}
FlipTest: FlipRegister.o

accessors.o: accessors.h control_path.h data_path.h FlipRegister.h
control_path.o: control_path.h data_path.h FlipRegister.h
data_path.o: data_path.h FlipRegister.h
FlipRegister.o: FlipRegister.h

microcontroller.o: accessors.h control_path.h data_path.h FlipRegister.h microcontroller.h

clean:
	rm -f ${OBJS}
wipe: clean
	- rm ${BIN}

include archlibpath.mk
