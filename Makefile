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

.PHONY: all shared static clean install

ifeq ($(OS),Windows_NT)
    # Windows (MinGW)
    SHARED_LIB_SUFFIX = .dll
    SHARED_LIB_FLAGS = -shared
else
    # Linux and macOS
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

all: $(BUILD_DIR) $(LIB_DIR) $(INCLUDE_DIR) shared static install

$(BUILD_DIR):
	mkdir -p $@

$(LIB_DIR):
	mkdir -p $@

$(INCLUDE_DIR):
	mkdir -p $@

# Shared library
shared: $(BUILD_DIR)/$(LIB_PREFIX)$(TARGET)$(SHARED_LIB_SUFFIX)

$(BUILD_DIR)/$(LIB_PREFIX)$(TARGET)$(SHARED_LIB_SUFFIX): $(SRC)
	$(CC) $(SHARED_LIB_FLAGS) $< -o $@

# Static library
static: $(BUILD_DIR)/$(LIB_PREFIX)$(TARGET)$(STATIC_LIB_SUFFIX)

$(BUILD_DIR)/$(LIB_PREFIX)$(TARGET)$(STATIC_LIB_SUFFIX): $(SRC)
	$(CC) -c $< -o $(BUILD_DIR)/$(LIB_PREFIX)$(TARGET).o
	$(AR) $@ $(BUILD_DIR)/$(LIB_PREFIX)$(TARGET).o
#rm $(BUILD_DIR)/$(LIB_PREFIX)$(TARGET).o

install:
	cp $(BUILD_DIR)/$(LIB_PREFIX)$(TARGET)$(STATIC_LIB_SUFFIX) $(LIB_DIR)/
	cp $(BUILD_DIR)/$(LIB_PREFIX)$(TARGET)$(SHARED_LIB_SUFFIX) $(LIB_DIR)/
	cp $(HEADER) $(INCLUDE_DIR)/

clean:
	rm -rf $(BUILD_DIR) $(LIB_DIR) $(INCLUDE_DIR)