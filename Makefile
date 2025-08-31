CC = gcc
AR = ar rcs
TARGET = teenyat
SRC = $(TARGET).c
STATIC_LIB_SUFFIX = .a
LIB_PREFIX = lib

ifeq ($(OS),Windows_NT)
    # Windows (MinGW or Cygwin)
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

all: shared static

# Shared library
shared: $(LIB_PREFIX)$(TARGET)$(SHARED_LIB_SUFFIX)

$(LIB_PREFIX)$(TARGET)$(SHARED_LIB_SUFFIX): $(SRC)
	$(CC) $(SHARED_LIB_FLAGS) $< -o $@

# Static library
static: $(LIB_PREFIX)$(TARGET)$(STATIC_LIB_SUFFIX)

$(LIB_PREFIX)$(TARGET)$(STATIC_LIB_SUFFIX): $(SRC)
	$(CC) -c $< -o $(LIB_PREFIX)$(TARGET).o
	$(AR) $@ $(LIB_PREFIX)$(TARGET).o
	rm $(LIB_PREFIX)$(TARGET).o

clean:
	rm -f $(LIB_PREFIX)$(TARGET).o
	rm -f $(LIB_PREFIX)$(TARGET)$(SHARED_LIB_SUFFIX)
	rm -f $(LIB_PREFIX)$(TARGET)$(STATIC_LIB_SUFFIX)
