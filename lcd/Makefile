CFLAGS += -I../.. -Os 
CPPFLAGS += $(CFLAGS) --std=c++17
ifeq ($(OS),Windows_NT)
	LDFLAGS += -s -lopengl32 -lgdi32
	RMCMD = del
else
	RMCMD = rm -f
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		LDFLAGS += -framework OpenGL -framework Cocoa
	else ifeq ($(UNAME_S),Linux)
		LDFLAGS += -s -lGLU -lGL -lX11
	endif
endif

all:
	gcc -c src/tigr.c -o tigr.o $(CFLAGS) 
	gcc -c ../../teenyat.c -o teenyat.o $(CFLAGS)
	g++ -c src/*.cpp $(CPPFLAGS)
	g++ *.o tigr.o teenyat.o -o ../bin/system $(LDFLAGS)

clean:
	$(RMCMD) ../bin/system tigr.o teenyat.o 