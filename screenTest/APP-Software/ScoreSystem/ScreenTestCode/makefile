SOURCES= $(wildcard *.cpp)
OBJECTS= $(SOURCES:.cpp=.o)
#DEPS= $(SOURCES:.cpp=.d)
EXE=prog
BASEPATH=$(shell echo "$$(cd ../../ &&pwd)")
TARGET=target

# Making for target
# > make ARCH=target
CXX=arm-rpizw-g++
BUILD_DIR=./build/files
EXE_DIR=./build
$(shell mkdir -p $(BUILD_DIR))

ifdef DEBUG
CXXFLAGS=-ggdb -O0
LIBPATH=$(BASEPATH)/lib/$(TARGET)/debug
else
CXXFLAGS=-O2
LIBPATH=$(BASEPATH)/lib/$(TARGET)/release
endif
# Setup the CFLAGS to ensure that the relevant warnings, includes and defines.
CXXFLAGS+=-Wall -D_REENTRANT -DOS_LINUX -I$(BASEPATH)/inc

ifneq (${LIBS},)
LIB=-l${LIBS}
else
LIB=-lpthread
endif

all: $(EXE)

$(EXE): $(addprefix ${BUILD_DIR}/, $(OBJECTS))
	$(CXX) $(CXXFLAGS) -o $(addprefix ${EXE_DIR}/, $@) $(addprefix ${BUILD_DIR}/, $(OBJECTS)) -L$(LIBPATH) -lOSApi -lrt -lpthread

${BUILD_DIR}/%.o: %.cpp #${BUILD_DIR}/%.d
	$(CXX) $(CXXFLAGS) -c $< -o $@

#${BUILD_DIR}/%.d: %.cpp
	#$(CXX) -MT$(@: .d=.o) -MM $(CXXFLAGS) $^ > $@

#-include $(addprefix ${BUILD_DIR}/, $(DEPS))


.PHONY: clean
clean:
	rm -r -f ./build/files/* ./build/$(EXE)
