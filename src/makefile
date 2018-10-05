BIN_DIR ?= ../bin
SRC_DIRS ?= .

#SRCS := $(shell find $(SRC_DIRS) -name *.cpp)
SRCS := $(shell find $(SRC_DIRS)/*.cpp)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CG_INCLUDE   =  -I .
CC=clang++
CCFLAGS = -O -Wdelete-incomplete
# CCFLAGS = -O -Wdelete-incomplete -Wreturn-type -Wparentheses -Wdelete-non-virtual-dtor
# CCFLAGS = -g

ENG_OBJS = $(BIN_DIR)/metaTest.o $(BIN_DIR)/genNode.o $(BIN_DIR)/genLang.o $(BIN_DIR)/genParser.o $(BIN_DIR)/metaGen.o $(BIN_DIR)/metaStream.o $(BIN_DIR)/metaLang.o
UTI_OBJS = $(BIN_DIR)/genArray.o $(BIN_DIR)/genGlossary.o $(BIN_DIR)/genString.o $(BIN_DIR)/genStreamable.o


all: walk_engine walk_lib test_utils uti_lib

test_utils:	$(BIN_DIR)/genTest.o $(UTI_OBJS)
					$(CC) $(CCFLAGS) $(BIN_DIR)/genTest.o $(UTI_OBJS) -o $(BIN_DIR)/TestUtils
walk_engine:	$(ENG_OBJS) $(UTI_OBJS) $(BIN_DIR)/PlugIn.o
				$(CC) $(CCFLAGS) $(ENG_OBJS) $(UTI_OBJS) $(BIN_DIR)/PlugIn.o -o $(BIN_DIR)/WalkEngine
uti_lib:
	ar -cr $(BIN_DIR)/libwalkuti.a $(UTI_OBJS)
walk_lib:
	ar -cr $(BIN_DIR)/libwalkeng.a $(ENG_OBJS)
# C++ source
$(BIN_DIR)/%.o:	%.cpp
	$(CC) $(CCFLAGS) $(CG_INCLUDE) -o $@ -c $<

.PHONY: clean

clean:
			rm $(BIN_DIR)/*

-include $(DEPS)

MKDIR_P ?= mkdir -p
