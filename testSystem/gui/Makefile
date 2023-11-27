CFLAGS += -I../..
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

buildWin: src/main.cpp src/util.cpp src/color.cpp tigr.c ../../teenyat.c
	g++ $^ -Os -o ../bin/system.exe $(CFLAGS) $(LDFLAGS) 

buildLinux: src/main.cpp tigr.c ../../teenyat.c
	g++ $^ -Os -o ../bin/system $(CFLAGS) $(LDFLAGS) 