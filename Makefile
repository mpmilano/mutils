.PHONY : clean

TARGET=libmutils.so

CPPFLAGS=-fPIC -g --std=c++14 
LDFLAGS= -lgc -shared -Wl,-rpath=$(shell pwd)/ --enable-new-dtags

SOURCES = utils.cpp GC_pointer.cpp

HEADERS = AtScopeEnd.hpp ObjectBuilder.hpp backtrace.hpp compile-time-tuple.hpp macro_utils.hpp restrict.hpp type_utils.hpp Hertz.hpp args-finder.hpp compile-time-lambda.hpp filter-varargs.hpp mutils.hpp tuple_extras.hpp FunctionalMap.hpp GC_pointer.hpp
OBJECTS=$(SOURCES:.cpp=.o)

all: $(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CXX)  $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

