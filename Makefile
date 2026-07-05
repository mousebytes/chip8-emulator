# compiler settings
CXX = g++
CXXFLAGS = -Wall -O3 -IE:/projects/c++/resources/SDL3-devel-3.4.12-mingw/x86_64-w64-mingw32/include

#linker settings
LDFLAGS = -LE:/projects/c++/resources/SDL3-devel-3.4.12-mingw/x86_64-w64-mingw32/lib -lmingw32 -lSDL3

#files
SRCS = main.cpp chip8.cpp platform.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = chip8.exe

#default build target
all: $(TARGET)

#linking final exe
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

#compiling src files to obj files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

#cleanup
clean:
	del /q /f *.o $(TARGET)