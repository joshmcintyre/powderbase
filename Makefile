# This file contains a make script for the PowderBase library and associated tests and tools
#
# Author: Josh McIntyre
#
#

# This block defines makefile variables
#
#
INCLUDE_API=src/api
API_FILES=src/api/*.cpp
API_INCLUDE_FILES=src/api/*.h
SAMPLE_FILE=src/tools/sample.cpp
PERF_FILE=src/tools/perf.cpp
FILEVIEWER_FILE=src/tools/file_viewer.cpp

BUILD_DIR=lib
BUILD_OBJ=*.o
BUILD_LIB=DB.a
TOOLS_DIR=bin
SAMPLE_BIN=sample
PERF_BIN=perf
FILEVIEWER_BIN=fileviewer

INSTALL_DIR=/usr/lib

CC=g++
FLAGS=-c -I$(INCLUDE_API)
SAMPLE_FLAGS=$(BUILD_DIR)/$(BUILD_LIB) -I$(BUILD_DIR)
PERF_FLAGS=$(BUILD_DIR)/$(BUILD_LIB) -I$(BUILD_DIR) -std=c++11
LIB=ar
LIB_FLAGS=rvs

# This rule builds the library
#
#
build: $(API_FILES)
	mkdir -p $(BUILD_DIR)
	$(CC) $(FLAGS) $(API_FILES)
	$(LIB) $(LIB_FLAGS) $(BUILD_DIR)/$(BUILD_LIB) $(BUILD_OBJ)
	rm $(BUILD_OBJ)
	cp $(API_INCLUDE_FILES) $(BUILD_DIR)

# This rule builds tools such as the sample driver, performance utility, and fileviewer utility
#
#
tools: $(SAMPLE_FILE) $(PERF_FILE)
	mkdir -p $(TOOLS_DIR)
	$(CC) -o $(TOOLS_DIR)/$(SAMPLE_BIN) $(SAMPLE_FILE) $(SAMPLE_FLAGS)
	$(CC) -o $(TOOLS_DIR)/$(PERF_BIN) $(PERF_FILE) $(PERF_FLAGS)
	$(CC) -o $(TOOLS_DIR)/$(FILEVIEWER_BIN) $(FILEVIEWER_FILE)
	
# This rule installs the library to the library directory
#
#
install: $(API_FILES)
	mkdir -p $(BUILD_DIR)
	$(CC) $(FLAGS) $(API_FILES)
	$(LIB) $(LIB_FLAGS) $(BUILD_DIR)/$(BUILD_LIB) $(BUILD_OBJ)
	rm $(BUILD_OBJ)
	cp $(BUILD_DIR)/*.h $(BUILD_DIR)/*.a $(INSTALL_DIR)

# This rule cleans the build directory
#
#
clean: $(BUILD_DIR)
	rm $(BUILD_DIR)/* $(TOOLS_DIR)/*
	rmdir $(BUILD_DIR) $(TOOLS_DIR)
