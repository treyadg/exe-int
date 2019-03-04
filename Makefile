
CC=gcc
CXX=g++
RM=rm

CFLAGS = -O3
CXXFLAGS  = -O3
CXXFLAGS += -std=c++17

INCLUDES  = -I.
INCLUDES += -Iinclude

LD_LIBS += -lcryptopp
LD_LIBS += -lstdc++fs
LD_LIBS += -lcurl

CXXSRCS  = $(wildcard src/*.cpp)
CSRCS += $(wildcard src/*.c)

CXXOBJS = $(subst .cpp,.o,$(CXXSRCS))
COBJS = $(subst .c,.o,$(CSRCS))

all: ExeInt

ExeInt: $(CXXOBJS) $(COBJS)
	g++ $(LDFLAGS) -o ExeInt $(CXXOBJS) $(COBJS) $(LD_LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDES)

clean:
	$(RM) $(CXXOBJS) $(COBJS)

distclean: clean
	$(RM) ExeInt
