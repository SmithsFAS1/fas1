CC=gcc
CXX=g++
CFLAGS=-Wall -O2
ifneq (,$(findstring CYGWIN,$(shell uname)))
CFLAGS+=-mno-cygwin
endif
CXXFLAGS=$(CFLAGS)

all: bmp15bit bmp2raw fontconvert bin2c

fontconvert: fontconvert.cpp
	$(CXX) $(CXXFLAGS) `freetype-config --cflags --libs` -I../include $^ -o $@
