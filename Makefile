# Copyright © 2011 Bart Massey
# Makefile for Linux arduinokit

CPP = g++
ARCH = -DLINUX
CPPFLAGS = `wx-config --cflags`
LIBS = `wx-config --libs`

OBJS = arduinokit.o serial.o toolkit.o

.cpp.o:
	$(CPP) $(ARCH) $(CPPFLAGS) -c $*.cpp

arduinokit: $(OBJS)
	$(CPP) $(CPPFLAGS) -o arduinokit $(OBJS) $(LIBS)

clean:
	-rm -f $(OBJS) arduinokit
