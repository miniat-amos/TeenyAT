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
BIN = $(BUILD_DIR)$(SEP)tnasm$(EXE_EXT)

all: $(BIN) 

$(BIN): $(SOURCES)
	@echo $(MSG) Building tnasm
	$(CXX) -o $(BIN) $(WFLAGS) $(CXX_FLAGS) $(SOURCES)

clean:
	@echo $(MSG) Removing output directories
ifeq ($(OS),Windows_NT)
	@if exist $(OUT_DIR) rmdir /s /q $(OUT_DIR)
else
	@rm -rf $(OUT_DIR)
endif



	rm -f tnasm
