CXX = g++ -std=c++11 # -Wall -pedantic
MAIN_BINARIES = $(basename $(wildcard *Main.cpp))
TEST_BINARIES = $(basename $(wildcard *Test.cpp))
HEADERS = $(wildcard *.h)
OBJECTS = $(addsuffix .o, $(basename $(filter-out %Main.cpp %Test.cpp, $(wildcard *.cpp))))
LIBRARIES =

.PRECIOUS: %.o
.SUFFIXES:
.PHONY: all compile checkstyle test

all: compile checkstyle test

compile: $(MAIN_BINARIES) $(TEST_BINARIES)

test: $(TEST_BINARIES)
	for T in $(TEST_BINARIES); do ./$$T; done

checkstyle:
	python3 ../cpplint.py --repository=. *.h *.cpp

clean:
	rm -f *.o
	rm -f $(MAIN_BINARIES)
	rm -f $(TEST_BINARIES)

%Main: %Main.o $(OBJECTS)
	$(CXX) -o $@ $^ $(LIBRARIES) -lncurses -lpthread

%Test: %Test.o $(OBJECTS)
	$(CXX) -o $@ $^ $(LIBRARIES) -lgtest -lgtest_main -lpthread -lncurses

%.o: %.cpp $(HEADERS)
	$(CXX) -c $<