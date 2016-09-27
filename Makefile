TARGET=run

count_IPs0=$(shell wc -l ~/all_IPs | sed 's/^ *//' | cut -f1 -d' ')
count_IPs=$(shell echo $$(($(count_IPs0) - 1)))

ALL=-D NUMBER_OF_NODES=$(count_IPs)

FLAGS=-Wall -Wextra -Werror -pedantic -pthread $(ARGS) -std=c++11 -g -g3 -rdynamic
INCLUDE=-I include
CXXFLAGS=$(INCLUDE) $(ALL) $(FLAGS) 

output/%.o: src/%.cpp
	g++ $(CXXFLAGS) -c -o $@ $<  -lncurses

CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix output/,$(notdir $(CPP_FILES:.cpp=.o)))

$(TARGET): $(OBJ_FILES)
	g++ $(FLAGS) -o $@ $^  -lncurses

clean:
	rm -rf $(TARGET) src/*~ $(OBJ_FILES)

debug: $(TARGET)
	gdb ./$(TARGET)
