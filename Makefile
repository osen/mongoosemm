CXX=c++
BIN=example
CFLAGS=-Isrc

MODULES =\
  src/mongoosemm/*.cpp \
  src/example/*.cpp

all:
	$(CXX) -o$(BIN) $(CFLAGS) $(MODULES)

clean:
	rm $(BIN)
