WX_CXXFLAGS = `wx-config --cxxflags`
WX_LIBS = `wx-config --libs`
CXXFLAGS = -std=c++17 $(WX_CXXFLAGS) -Ofast
CXX = clang++ -stdlib=libc++

# Define your source files
SRC = main.cpp \
		XML/TileData.cpp \
		XML/TaskListDocument.cpp \
		XML/XMLSerializer.cpp \
      # Add other .cpp files here

# Define object files from source files
OBJ = $(SRC:.cpp=.o)

# Define your target
app: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(WX_LIBS)

# Generic rule for compiling .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJ) app
