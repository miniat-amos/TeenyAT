CC = gcc
AR = ar rcs

TARGET = teenyat
SRC = $(TARGET).c
HEADER = $(TARGET).h

STATIC_LIB_SUFFIX = .a
LIB_PREFIX = lib
DYNAMIC_SUFFIX = _d

ifeq ($(OS),Windows_NT)
    # Windows (MinGW)

	SHELL := cmd.exe
	CP_CMD = copy

	SEP = \\
    SHARED_LIB_SUFFIX = .dll
    SHARED_LIB_FLAGS = -shared -DTNY_BUILD_DLL
	EXE_EXT = .exe
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
	EXE_EXT = 

endif

OUT_DIR = out
BUILD_DIR = $(OUT_DIR)$(SEP)build
LIB_DIR = $(OUT_DIR)$(SEP)lib
INCLUDE_DIR = $(OUT_DIR)$(SEP)include
BIN_DIR = $(OUT_DIR)$(SEP)bin

STATIC_LIB_NAME = $(LIB_PREFIX)$(TARGET)$(STATIC_LIB_SUFFIX)
SHARED_LIB_NAME = $(LIB_PREFIX)$(TARGET)$(DYNAMIC_SUFFIX)$(SHARED_LIB_SUFFIX)

MSG = [ INFO ] :

export SHELL BUILD_DIR BIN_DIR SEP EXE_EXT MSG

.PHONY: all directories shared static clean install lcd

all: directories shared static tnasm lcd install

directories:
	@echo $(MSG) Creating common output directories
ifeq ($(OS),Windows_NT)
	@if not exist $(OUT_DIR) mkdir $(OUT_DIR)
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@if not exist $(LIB_DIR) mkdir $(LIB_DIR)
	@if not exist $(INCLUDE_DIR) mkdir $(INCLUDE_DIR)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
else
	@mkdir -p $(BUILD_DIR) $(LIB_DIR) $(INCLUDE_DIR) $(BIN_DIR)
endif

# Shared library
shared: $(BUILD_DIR)$(SEP)$(SHARED_LIB_NAME)

$(BUILD_DIR)$(SEP)$(SHARED_LIB_NAME): $(SRC)
	@echo $(MSG) Building the shared/dynamic library
	$(CC) $(SHARED_LIB_FLAGS) $< -o $@

# Static library
static: $(BUILD_DIR)$(SEP)$(STATIC_LIB_NAME)

$(BUILD_DIR)$(SEP)$(STATIC_LIB_NAME): $(SRC)
	@echo $(MSG) Building the static library
	$(CC) -c $< -o $(BUILD_DIR)$(SEP)$(LIB_PREFIX)$(TARGET).o
	$(AR) $@ $(BUILD_DIR)$(SEP)$(LIB_PREFIX)$(TARGET).o

tnasm: $(BIN_DIR)$(SEP)tnasm$(EXE_EXT)

$(BIN_DIR)$(SEP)tnasm$(EXE_EXT): static
	$(MAKE) -C tnasm

lcd: directories static
	$(MAKE) -C lcd

install: directories static shared tnasm lcd
	@echo $(MSG) Copying files to installable directories

	@echo $(MSG) Leaving the static library in $(LIB_DIR)
	$(CP_CMD) $(BUILD_DIR)$(SEP)$(STATIC_LIB_NAME) $(LIB_DIR)$(SEP)

	@echo $(MSG) Leaving the shared/dynamic library in $(LIB_DIR)
	$(CP_CMD) $(BUILD_DIR)$(SEP)$(SHARED_LIB_NAME) $(LIB_DIR)$(SEP)

	@echo $(MSG) Leaving $(HEADER) in $(INCLUDE_DIR)
	$(CP_CMD) $(HEADER) $(INCLUDE_DIR)$(SEP)

	@echo $(MSG) Leaving tnasm$(EXE_EXT) in $(BIN_DIR)
	$(CP_CMD) $(BUILD_DIR)$(SEP)tnasm$(EXE_EXT) $(BIN_DIR)$(SEP)

	@echo $(MSG) Leaving lcd$(EXE_EXT) in $(BIN_DIR)
	$(CP_CMD) $(BUILD_DIR)$(SEP)lcd$(SEP)lcd$(EXE_EXT) $(BIN_DIR)$(SEP)

clean:
	@echo $(MSG) Removing output directories
ifeq ($(OS),Windows_NT)
	@if exist $(OUT_DIR) rmdir /s /q $(OUT_DIR)
else
	@rm -rf $(OUT_DIR)
endif
