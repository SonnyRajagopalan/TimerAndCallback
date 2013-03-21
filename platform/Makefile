# 03/25/2012.
# Painstakingly put together from a lot of research. Some reference material is 
# included below:
# (1) http://make.paulandlesley.org/autodep.html 
# (2) http://www.cprogramming.com/tutorial/makefiles_continued.html

include ../_include.mk

BASEDIR = ..
SOURCES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))

all: $(OBJECTS)

%.o: %.cpp
	$(CPP) $(INCLUDE) $(CPPFLAGS) -c $*.cpp -o $@
#
# Generate the dependencies: Draws heavily from http://make.paulandlesley.org/autodep.html
#
%.P : %.cpp
	$(MAKEDEPEND);
	@sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' < $*.depends > $@; \
	rm -f $*.depends; [ -s $@ ] || rm -f $@

# The following works just fine, but throws a number of errors of the form
# Makefile:24: TimerCallback.P: No such file or directory.
# include $(SOURCES:.cpp=.P)
# To prevent that, we do "-" before the include. This tells the Makefile to 
# not bother if the file *.P does not exist.

-include $(SOURCES:.cpp=.P)

clean:
	rm -rf *.o *~ *.depends *.P