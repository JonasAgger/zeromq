SOURCES= $(wildcard *.cpp)
OBJECTS= $(SOURCES:.cpp=.o)
DEPS= $(SOURCES:.cpp=.d)
EXE=prog
CXXFLAGS=-I.

# Making for target
# > make ARCH=host
ifeq (${ARCH},)
CXX=g++
BUILD_DIR=./host/files
EXE_DIR=./host
$(shell mkdir -p $(BUILD_DIR))
endif

# Making for target
# > make ARCH=target
ifeq (${ARCH},target)
CXX=arm-rpizw-g++
BUILD_DIR=./target/files
EXE_DIR=./target
$(shell mkdir -p $(BUILD_DIR))
endif

ifneq (${LIBS},)
LIB=-l${LIBS}
else
LIB=-lpthread
endif

all: $(EXE)

$(EXE): $(addprefix ${BUILD_DIR}/, $(DEPS)) $(addprefix ${BUILD_DIR}/, $(OBJECTS))
	$(CXX) $(CXXFLAGS) -o $(addprefix ${EXE_DIR}/, $@) $(addprefix ${BUILD_DIR}/, $(OBJECTS)) $(LIB)

${BUILD_DIR}/%.o: %.cpp ${BUILD_DIR}/%.d
	$(CXX) $(CXXFLAGS) -c $< -o $@

${BUILD_DIR}/%.d: %.cpp
	$(CXX) -MT$(@: .d=.o) -MM $(CXXFLAGS) $^ > $@

-include $(addprefix ${BUILD_DIR}/, $(DEPS))


.PHONY: clean
clean:
	rm -r -f ./host/files/* ./target/files/* ./host/$(EXE) ./target/$(EXE)
