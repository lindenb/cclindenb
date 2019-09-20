CXX      := clang++
CXXFLAGS := -Wall -Wextra -Werror -Wno-sign-compare -Wno-unused-variable -fexceptions -Wno-unused-function  -std=c++17 -ggdb
CFLAGS := -Wall -Wextra -Werror -Wno-sign-compare -Wno-unused-variable -fexceptions -Wno-unused-function 
LDFLAGS  := -L/usr/lib -lstdc++ -lm -Llibbase -Llibpcrecpp -Llibprocessgroup
LD := clang
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)
INCLUDEPATH  := -I../tarlib/
EXTSRC      := $(wildcard ../tarlib/*.cpp)
SRC      := $(wildcard *.cpp)

OBJECTS := $(EXTSRC:%.cpp=$(OBJ_DIR)/%.o)
PROGRAMS := $(SRC:%.cpp=%.exe)

all: $(PROGRAMS)
	@echo Done

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDEPATH) -o $@ -c $<

%.exe: %.cpp $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(INCLUDEPATH) $(LDFLAGS) $< $(OBJECTS)  -o $@

.PHONY: all build clean debug release

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(PROGRAMS)
	-@rm -fv $(OBJECTS)
