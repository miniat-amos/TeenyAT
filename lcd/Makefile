CFLAGS += -I.. -I./include -O3 
WFLAGS = -Wall -Wextra -Wpedantic
CPPFLAGS += $(CFLAGS) --std=c++17
ifeq ($(OS),Windows_NT)
	LDFLAGS += -s -lopengl32 -lgdi32
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		LDFLAGS += -framework OpenGL -framework Cocoa
	else ifeq ($(UNAME_S),Linux)
		LDFLAGS += -s -lGLU -lGL -lX11
	endif
endif

LCD_BUILD_DIR = ..$(SEP)$(BUILD_DIR)$(SEP)lcd
BIN = $(LCD_BUILD_DIR)$(SEP)lcd$(EXE_EXT)

.PHONY: all check_makelevel directories

all: check_makelevel directories
	@echo $(MSG) Building the LCD system
	gcc -c tigr.c -o $(LCD_BUILD_DIR)$(SEP)tigr.o $(CFLAGS) 
	gcc -c ../teenyat.c -o $(LCD_BUILD_DIR)$(SEP)teenyat.o $(CFLAGS) $(WFLAGS)
	g++ -c color.cpp -o $(LCD_BUILD_DIR)$(SEP)color.o $(CPPFLAGS) $(WFLAGS)
	g++ -c util.cpp -o $(LCD_BUILD_DIR)$(SEP)util.o $(CPPFLAGS) $(WFLAGS)
	g++ -c screen.cpp -o $(LCD_BUILD_DIR)$(SEP)screen.o $(CPPFLAGS) $(WFLAGS)
	g++ -c main.cpp -o $(LCD_BUILD_DIR)$(SEP)main.o $(CPPFLAGS) $(WFLAGS)
	g++ $(LCD_BUILD_DIR)$(SEP)tigr.o $(LCD_BUILD_DIR)$(SEP)screen.o \
		$(LCD_BUILD_DIR)$(SEP)teenyat.o $(LCD_BUILD_DIR)$(SEP)main.o \
		$(LCD_BUILD_DIR)$(SEP)color.o $(LCD_BUILD_DIR)$(SEP)util.o \
		-o $(BIN) $(LDFLAGS)

check_makelevel:
ifeq ($(MAKELEVEL), 0)
	# Attempting to build local to this directory
	$(error Please build lcd with the root Makefile)
endif

directories: check_makelevel
	@echo $(MSG) Creating LCD build directory
ifeq ($(OS),Windows_NT)
	@if not exist $(LCD_BUILD_DIR) mkdir $(LCD_BUILD_DIR)
else
	@mkdir -p $(LCD_BUILD_DIR)
endif
