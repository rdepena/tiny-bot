CC = cc
CFLAGS = -g -Wall -Iinclude $(shell pkg-config --cflags sdl2 sdl2_ttf)
LDFLAGS = $(shell pkg-config --libs sdl2 sdl2_ttf) -ledit
SOURCES = src/main.c src/serial.c src/find_arduino.c
EXECUTABLEDEST = bin
EXECUTABLE = program
DEBUGGER = lldb

build:
	mkdir -p $(EXECUTABLEDEST)
	$(CC) $(CFLAGS) $(SOURCES) $(LDFLAGS) -o $(EXECUTABLEDEST)/$(EXECUTABLE)

clean:
	rm -f $(EXECUTABLEDEST)/$(EXECUTABLE)

debug: build
	$(DEBUGGER) $(EXECUTABLEDEST)/$(EXECUTABLE)