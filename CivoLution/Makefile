# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra -Werror -pedantic -O3 -g

SFML_LIB = -lsfml-graphics -lsfml-window -lsfml-system

# Source files
SRCS = main.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable
TARGET = CivOlution

# Build target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(SFML_LIB)

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
	# $(CXX) main.o -o $(TARGET) $(SFML_LIB)


# Clean
clean:
	rm -f $(OBJS) $(TARGET)

all: $(TARGET)
	./$(TARGET)
