CFLAGS += -I.. -I./include -O3 
WFLAGS = -Wall -Wextra -Wpedantic
CPPFLAGS += $(CFLAGS) --std=c++17
EXE_EXT :=
ifeq ($(OS),Windows_NT)
	EXE_EXT := .exe
	LDFLAGS += -s -lopengl32 -lgdi32
	# Check if rm exists, otherwise use del
	ifneq ($(shell where rm 2>tigr.o),)
		RMCMD = rm -f
	else
		RMCMD = del
	endif
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
	gcc -c tigr.c -o tigr.o $(CFLAGS) 
	gcc -c ../teenyat.c -o teenyat.o $(CFLAGS) $(WFLAGS)
	gcc -c audio.c -o audio.o $(CFLAGS) $(WFLAGS) -Wno-unused-result -Wno-c2x-extensions

	g++ -o generate_assets generate_assets.cpp $(CPPFLAGS) $(WFLAGS)
	./generate_assets exp_board_images/*.png

	g++ -c bin_assets.cpp -o bin_assets.o $(CPPFLAGS) $(WFLAGS)
	g++ -c utils.cpp -o utils.o $(CPPFLAGS) $(WFLAGS)
	g++ -c lcd_screen.cpp -o lcd.o $(CPPFLAGS) $(WFLAGS)
	g++ -c segment.cpp -o seg.o $(CPPFLAGS) $(WFLAGS)
	g++ -c leds.cpp -o leds.o $(CPPFLAGS) $(WFLAGS)
	g++ -c edison_board.cpp -o board.o $(CPPFLAGS) $(WFLAGS)
	g++ -c main.cpp -o main.o $(CPPFLAGS) $(WFLAGS)
	g++ tigr.o teenyat.o seg.o lcd.o audio.o bin_assets.o leds.o utils.o board.o main.o -o edison $(LDFLAGS)

clean:
	$(RMCMD) edison$(EXE_EXT) *.o generate_assets$(EXE_EXT)
