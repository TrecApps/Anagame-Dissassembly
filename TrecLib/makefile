# Instructions for compiling Anagame's TrecLib in a Linux Environment

CC	= g++
CFLAGS  = -Wall -Wextra -fpermissive -std=c++17 -g -c
LD	= ld

SOURCE_FILES := $(wildcard *.cpp)
# Set up All Object files in this directory
OBJFILES := $(patsubst %.cpp,%.o,$(SOURCE_FILES))

.o.cpp:
	$(CC) $(CFLAGS) -o $@ $<
	
all: TrecLib.o

TrecLib.o: $(OBJFILES)
	$(LD) -r -o TrecLib.o $(OBJFILES) ; mv TrecLib.o ../Debug/TrecLib.o 

clean:
	rm $(OBJFILES)
