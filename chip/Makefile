BIN := CPU
OBJS := control_path.o data_path.o FlipRegister.o
CPPFLAGS = -I${ARCH_HEADERS_PATH}
CXXFLAGS := -std=c++98 -Wall -Wextra
LDFLAGS := -L.
LDLIBS := -larch2-5a

${BIN}: ${OBJS}

control_path.o: control_path.h data_path.h FlipRegister.h
data_path.o: data_path.h FlipRegister.h
FlipRegister.o: FlipRegister.h

clean:
	rm -f ${OBJS}
wipe: clean
	- rm ${BIN}

include archlibpath.mk
