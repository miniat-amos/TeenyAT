CC = gcc
AR = ar rcs

CFLAGS = -Wall -Wextra -Wpedantic

MSG = [ INFO ][ TEENYAT ]

ifeq ($(OS),Windows_NT)
	# Windows (MinGW)

	SHELL := cmd.exe
	CP_CMD = copy

	SHARED_LIB_SUFFIX = .dll
	SHARED_LIB_FLAGS = -shared -DTNY_BUILD_DLL
	EXE_EXT = .exe
else
	# Linux and macOS	

	CP_CMD = cp

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

.PHONY: all debug release .build


all: release


release: CFLAGS += -O2
release: .build


debug: CFLAGS += -g -O0
debug: MSG := $(MSG)[ DEBUG ]
debug: .build


#==============================================
.build:

	@#---------- DIRECTORIES ----------

	@echo $(MSG) Creating common output directories
ifeq ($(OS),Windows_NT)
	@if not exist out mkdir out
	@if not exist out/build mkdir out/build
	@if not exist out/lib mkdir out/lib
	@if not exist out/include mkdir out/include
	@if not exist out/bin mkdir out/bin
else
	@mkdir -p out/build out/lib out/include out/bin
endif

	@#---------- HEADER ----------

	@echo $(MSG) Leaving teenyat.h in out/include
	$(CP_CMD) teenyat.h out/include/

	@#---------- STATIC LIBRARY ----------

	@echo $(MSG) Building the static library
	$(CC) -c teenyat.c $(CFLAGS) -o out/build/teenyat.o
	$(AR) out/build/libteenyat.a out/build/teenyat.o

	@echo $(MSG) Leaving the static library in out/lib
	$(CP_CMD) out/build/libteenyat.a out/lib/

	@#---------- SHARED/DYNAMIC LIBRARY ----------

	@echo $(MSG) Building the shared/dynamic library
	$(CC) $(CFLAGS) $(SHARED_LIB_FLAGS) teenyat.c -o out/build/libteenyat_d$(SHARED_LIB_SUFFIX)

	@echo $(MSG) Leaving the shared/dynamic library in out/lib
	$(CP_CMD) out/build/libteenyat_d$(SHARED_LIB_SUFFIX) out/lib/

	@#---------- SUB-TOOLS ----------
	make -C tnasm
	@echo $(MSG) Leaving tnasm$(EXE_EXT) in out/bin
	$(CP_CMD) tnasm/tnasm$(EXE_EXT) out/bin/

	make -C lcd
	@echo $(MSG) Leaving lcd$(EXE_EXT) in out/bin
	$(CP_CMD) lcd/lcd$(EXE_EXT) out/bin/

	make -C edison
	@echo $(MSG) Leaving edison$(EXE_EXT) in out/bin
	$(CP_CMD) edison/edison$(EXE_EXT) out/bin/

	@#---------- SETUP MESSAGE ----------
	@echo =====================================================================
	@echo When creating your own systems with the TeenyAT, be sure to:
	@echo - Add out/include to your compiler\'s include path
	@echo - Add out/lib to your system\'s library path for dynamic linking
	@echo - Link with -lteenyat for static linking
	@echo - Link with -lteenyat_d for shared/dynamic linking
	@echo - Add out/bin to your system\'s PATH variable for quick access to
	@echo   the assembler, tnasm, along with the systems lcd and edison
	@echo =====================================================================


#==============================================
clean:
	@echo $(MSG) Removing output directories
ifeq ($(OS),Windows_NT)
	@if exist out rmdir /s /q out
else
	@rm -rf out
endif