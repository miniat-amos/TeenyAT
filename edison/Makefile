CFLAGS += -I.. -I./include -Os 
WFLAGS = -Wall -Wextra -Wpedantic
CPPFLAGS += $(CFLAGS) --std=c++17
ifeq ($(OS),Windows_NT)
	LDFLAGS += -s -lopengl32 -lgdi32
	# Check if rm exists, otherwise use del
	ifneq ($(shell where rm 2>nul.o),)
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
	gcc -c audio.c -o audio.o $(CFLAGS) $(WFLAGS) -Wno-unused-result 
	g++ -c utils.cpp -o utils.o $(CPPFLAGS) $(WFLAGS)
	g++ -c lcd_screen.cpp -o lcd.o $(CPPFLAGS) $(WFLAGS)
	g++ -c segment.cpp -o seg.o $(CPPFLAGS) $(WFLAGS)
	g++ -c leds.cpp -o leds.o $(CPPFLAGS) $(WFLAGS)
	g++ -c edison_board.cpp -o board.o $(CPPFLAGS) $(WFLAGS)
	g++ -c main.cpp -o main.o $(CPPFLAGS) $(WFLAGS)
	g++ tigr.o teenyat.o seg.o lcd.o audio.o leds.o utils.o board.o main.o -o edison $(LDFLAGS)

clean:
	$(RMCMD) edison *.o
