CXX := g++
CXXFLAGS := -std=c++20

SRC := $(shell find . -name "*.cpp")
OBJ  := $(patsubst %.cpp, %.o, $(SRC))

all: extractor

extractor: $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o extractor $(OBJ) $(LDLIBS)

depend: .depend

.depend: $(SRC)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend;

clean:
	rm -f $(OBJ) driver
	rm -f *~ .depend

include .depend