.PHONY : clean

CPPFLAGS=-fPIC -g --std=c++14 
LDFLAGS=-shared 

SOURCES = utils.cpp
HEADERS = AtScopeEnd.hpp  Hertz.hpp  args-finder.hpp  macro_utils.hpp  type_utils.hpp  utils.hpp
OBJECTS=$(SOURCES:.cpp=.o)

TARGET=mutils.so

all: $(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CXX) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

