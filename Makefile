CC = gcc
AR = ar rcs

TARGET = teenyat
SRC = $(TARGET).c
HEADER = $(TARGET).h

STATIC_LIB_SUFFIX = .a
LIB_PREFIX = lib

BUILD_DIR = build
LIB_DIR = lib
INCLUDE_DIR = include

STATIC_LIB_NAME = $(LIB_PREFIX)$(TARGET)$(STATIC_LIB_SUFFIX)
SHARED_LIB_NAME = $(LIB_PREFIX)$(TARGET)$(SHARED_LIB_SUFFIX)

ifeq ($(OS),Windows_NT)
    # Windows (MinGW)

	CP_CMD = copy

	SEP = \\
    SHARED_LIB_SUFFIX = .dll
    SHARED_LIB_FLAGS = -shared -DTNY_BUILD_DLL
else
    # Linux and macOS	

	CP_CMD = cp

	SEP = /
    ifeq ($(shell uname -s),Darwin)
        # macOS
        SHARED_LIB_SUFFIX = .dylib
        SHARED_LIB_FLAGS = -shared -undefined dynamic_lookup
    else
        # Linux
        SHARED_LIB_SUFFIX = .so
        SHARED_LIB_FLAGS = -shared -fPIC
    endif
endif

.PHONY: all directories shared static clean install

all: directories shared static install

directories:
	@echo Creating output directories...
ifeq ($(OS),Windows_NT)
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@if not exist $(LIB_DIR) mkdir $(LIB_DIR)
	@if not exist $(INCLUDE_DIR) mkdir $(INCLUDE_DIR)
else
	@mkdir -p $(BUILD_DIR) $(LIB_DIR) $(INCLUDE_DIR)
endif
	@echo Done

# Shared library
shared: $(BUILD_DIR)$(SEP)$(SHARED_LIB_NAME)

$(BUILD_DIR)$(SEP)$(SHARED_LIB_NAME): $(SRC)
	$(CC) $(SHARED_LIB_FLAGS) $< -o $@

# Static library
static: $(BUILD_DIR)$(SEP)$(STATIC_LIB_NAME)

$(BUILD_DIR)$(SEP)$(STATIC_LIB_NAME): $(SRC)
	$(CC) -c $< -o $(BUILD_DIR)$(SEP)$(LIB_PREFIX)$(TARGET).o
	$(AR) $@ $(BUILD_DIR)$(SEP)$(LIB_PREFIX)$(TARGET).o

install:
	@echo Leaving static and shared libraries in .$(SEP)$(LIB_DIR)
	$(CP_CMD) $(BUILD_DIR)$(SEP)$(STATIC_LIB_NAME) $(LIB_DIR)$(SEP)
	$(CP_CMD) $(BUILD_DIR)$(SEP)$(SHARED_LIB_NAME) $(LIB_DIR)$(SEP)
	@echo Leaving $(HEADER) in .$(SEP)$(INCLUDE_DIR)
	$(CP_CMD) $(HEADER) $(INCLUDE_DIR)$(SEP)

clean:
	@echo Removing output directories...
ifeq ($(OS),Windows_NT)
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	@if exist $(LIB_DIR) rmdir /s /q $(LIB_DIR)
	@if exist $(INCLUDE_DIR) rmdir /s /q $(INCLUDE_DIR)
else
	@rm -rf $(BUILD_DIR) $(LIB_DIR) $(INCLUDE_DIR)
endif
	@echo Done
