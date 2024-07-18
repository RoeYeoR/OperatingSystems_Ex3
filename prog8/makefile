CXX = g++
CXXFLAGS = -Wall -std=c++11
SOURCES = prog8
OBJECTS = $(SOURCES:.cpp=.o)


TARGET = prog8

.PHONY: all clean coverage

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS)  -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

coverage: $(TARGET)
	
	gcov $(SOURCES)

clean:
	rm -f *.o $(TARGET) *.gcno *.gcda *.gcov


