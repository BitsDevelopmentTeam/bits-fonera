
# Makefile for openwrt build only. To build for PC, use CMake

SRC=           \
base64.cpp     \
configfile.cpp \
logger.cpp     \
bitsd.cpp      \
core.cpp       \
main.cpp

OBJ=$(SRC:.cpp=.o)

all: bitsd

clean:
	rm bitsd *.o

bitsd: $(OBJ)
	$(CXX) $(LFLAGS) -Os -s -Wl,--gc-sections -o bitsd $(OBJ) \
	-lboost_system-mt $(LIBS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -Os -ffunction-sections -c $<
