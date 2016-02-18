CC = g++

CFLAGS = -stdlib=libc++ -Wall -D__MACOSX_CORE__ -framework CoreMidi -framework CoreAudio -framework CoreFoundation

TARGET = boutique
DEP = lib/RtMidi

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp $(DEP).cpp

midiprobe: midiprobe.cpp
	$(CC) $(CFLAGS) -o midiprobe midiprobe.cpp $(DEP).cpp