CFLAGS += -I.. -I./include -Os 
CPPFLAGS += $(CFLAGS) --std=c++17
ifeq ($(OS),Windows_NT)
	LDFLAGS += -s -lopengl32 -lgdi32
	# for right now it rm since im using a windows powershell and (del) dosnt work
	RMCMD = rm 
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
	gcc -c ../teenyat.c -o teenyat.o $(CFLAGS)
	g++ -c src/color.cpp -o color.o $(CPPFLAGS)
	g++ -c src/util.cpp -o util.o $(CPPFLAGS)
	g++ -c src/screen.cpp -o screen.o $(CPPFLAGS)
	g++ -c src/main.cpp -o main.o $(CPPFLAGS)
	g++ tigr.o screen.o teenyat.o main.o color.o util.o -o ./bin/system $(LDFLAGS)

clean:
	$(RMCMD) ./bin/system *.o