CXX = g++

SOURCES = parser.cpp lexer.cpp listing.cpp warnings.cpp

ifneq ($(MAKELEVEL), 0)
	# This is a sub-Makefile in the project
	CXX_FLAGS = -I.. --std=c++20
else
	# Attempting to build local to this directory
	$(error Please build with the root Makefile)
endif

WFLAGS = -Wall -Wextra -Wpedantic
BIN = ..$(SEP)$(BUILD_DIR)$(SEP)tnasm$(EXE_EXT)

all: $(BIN) 

$(BIN): $(SOURCES)
	@echo $(MSG) Building tnasm
	$(CXX) -o $(BIN) $(WFLAGS) $(CXX_FLAGS) $(SOURCES)
