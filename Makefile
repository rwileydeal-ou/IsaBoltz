FC=gfortran
CC=g++

INC=./include
SRC_DIR := ./src
OBJ_DIR := ./obj
BIN_DIR := ./bin
LDFLAGS=-L/usr/lib/x86_64-linux-gnu/

SOURCES := $(shell find $(SRC_DIR) \( -name "*.cpp" \)) 
FORT_FILES := $(shell find $(SRC_DIR)/fort \( -name "*.f" -o -name "*.f95" -o -name "*.F" \))
SOURCES_LOCAL := $(filter-out $(SRC_DIR)/main/clustermain.cpp, $(SOURCES))
SOURCES_CLUSTER := $(filter-out $(SRC_DIR)/main/mainboltz.cpp, $(SOURCES))
OBJ_FILES := $(shell find $(OBJ_DIR) \( -name "*.o" \)) 
OBJ_FILES_LOCAL := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES_LOCAL))
OBJ_FILES_CLUSTER := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES_CLUSTER))

CXXFLAGS=-I$(INC) -I/usr/include -I/usr/include/boost -I/usr/include/sqlite3
LIBS=-lboost_system -lboost_filesystem -lboost_regex -lboost_log -lboost_thread -ldl -lstdc++ -pthread -std=c++14 -lsqlite3 -DBOOST_LOG_DYN_LINK -DBOOST_STACKTRACE_USE_ADDR2LINE
FORTFLAGS=-shared -fPIC --no-gnu-unique

LIST=$(BIN_DIR)/fortLib.so $(BIN_DIR)/isaboltz $(BIN_DIR)/clusterboltz
all: $(LIST)

$(BIN_DIR)/isaboltz: $(OBJ_FILES_LOCAL)
	$(CC) $(LDFLAGS) -Wall -Wpedantic  -g -o $@ $^ $(LIBS) -fsanitize=address,undefined 

$(BIN_DIR)/clusterboltz: $(OBJ_FILES_CLUSTER)
	$(CC) $(LDFLAGS) -Wall -Wpedantic  -g -o $@ $^ $(LIBS) -fsanitize=address,undefined 

$(BIN_DIR)/fortLib.so:
	$(CC) -fPIC $(LDFLAGS) -c $(SRC_DIR)/fort/BoostInterface.cpp -o $(SRC_DIR)/fort/BoostInterface.o $(LIBS)
	$(FC) $(FORTFLAGS) -g -o $(BIN_DIR)/fortLib.so -Wl,--whole-archive $(SRC_DIR)/fort/libisared.a $(SRC_DIR)/fort/libisajet.a $(FORT_FILES) $(SRC_DIR)/fort/BoostInterface.o -Wl,--no-whole-archive

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CXXFLAGS) $(LDFLAGS) -Wall -Wpedantic  -g -c -o $@ $< $(LIBS) -fsanitize=address,undefined 

.PHONY : clean 
clean: 
	-rm $(OBJ_FILES)