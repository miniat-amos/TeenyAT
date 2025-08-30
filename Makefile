CC = gcc
AR = ar rcs
TARGET = teenyat
SRC = $(TARGET).c
STATIC_LIB_SUFFIX = .a

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
shared: $(TARGET)$(SHARED_LIB_SUFFIX)

$(TARGET)$(SHARED_LIB_SUFFIX): $(SRC)
	$(CC) $(SHARED_LIB_FLAGS) $< -o $@

# Static library
static: $(TARGET)$(STATIC_LIB_SUFFIX)

$(TARGET)$(STATIC_LIB_SUFFIX): $(SRC)
	$(CC) -c $< -o $(TARGET).o
	$(AR) $@ $(TARGET).o
	rm $(TARGET).o

clean:
	rm -f $(TARGET).o $(TARGET)$(SHARED_LIB_SUFFIX) $(TARGET)$(STATIC_LIB_SUFFIX)