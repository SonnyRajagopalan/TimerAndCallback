BASEDIR = $(shell pwd)
CPP = g++
CPPFLAGS = -Wall -O3 -g
LIB = -luuid -lpthread -lrt -lstdc++
INCLUDE = -I $(BASEDIR)/include
MAKEDEPEND = g++ -MM $(INCLUDE) -o $*.depends $<
#BUILDID = $(shell date +%Y%m%d-%H%M%S)
BUILDID = `date +%Y%m%d-%H%M%S`
DIRS += $(BASEDIR)/common
DIRS += $(BASEDIR)/platform
DIRS += $(BASEDIR)/test
