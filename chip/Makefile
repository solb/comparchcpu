BIN := CPU
OBJS := data_path.o FlipRegister.o
CPPFLAGS = -I${ARCH_HEADERS_PATH}
CXXFLAGS := -Wall -Wextra
LDFLAGS := -L.
LDLIBS := -larch2-5a

${BIN}: ${OBJS}

data_path.o: data_path.h
FlipRegister.o: FlipRegister.h

clean:
	rm -f ${OBJS}
wipe: clean
	- rm ${BIN}

include archlibpath.mk
