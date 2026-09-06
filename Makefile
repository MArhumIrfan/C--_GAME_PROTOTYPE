# Build with:  make
# Clean with:  make clean
# Run with:    make run
#
# This project is split across multiple .cpp files. If you try to compile
# just main.cpp (e.g. `g++ main.cpp -o game`), you will get "undefined
# reference" linker errors for everything declared in Game.h but implemented
# in Game_Init.cpp / Game_Update.cpp / Game_Render.cpp / Game_UI.cpp /
# Game_Loop.cpp / Player.cpp / Enemy.cpp / AudioState.cpp - every .cpp file
# has to be compiled and linked together. This Makefile does that for you.

CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra
LDFLAGS := -lSDL2

SOURCES := main.cpp \
           Game_Init.cpp \
           Game_Update.cpp \
           Game_Render.cpp \
           Game_UI.cpp \
           Game_Loop.cpp \
           Player.cpp \
           Enemy.cpp \
           AudioState.cpp

OBJECTS := $(SOURCES:.cpp=.o)
TARGET := game

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Every .cpp includes Game.h, so if Game.h (or the headers it pulls in)
# changes, everything needs to rebuild. Simplest reliable way to express that
# without hand-tracking every header dependency:
$(OBJECTS): Game.h GameTypes.h AudioState.h Player.h Enemy.h Font.h Sprites.h

run: all
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)
