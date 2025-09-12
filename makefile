# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 
LDFLAGS = -lraylib 

# Directories
SRCDIR = .
OBJDIR = obj
DATADIR = data

# Target executable
TARGET = game

# Source files - add all your .cpp files here
SOURCES = main.cpp

# Object files (automatically generated from sources)
OBJECTS = $(SOURCES:%.cpp=$(OBJDIR)/%.o)

# Default target
all: $(TARGET)

# Create the executable
$(TARGET): $(OBJECTS) | $(DATADIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create directories if they don't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(DATADIR):
	mkdir -p $(DATADIR)

# Clean build files
clean:
	rm -rf $(OBJDIR) $(TARGET)

# Clean everything including data directory
cleanall: clean
	rm -rf $(DATADIR)

# Rebuild everything
rebuild: clean all

# Install raylib (Ubuntu/Debian)
install-deps:
	sudo apt update
	sudo apt install libraylib-dev

# Run the game
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Release build (default is already optimized)
release: $(TARGET)

# Help
help:
	@echo "Available targets:"
	@echo "  all       - Build the game (default)"
	@echo "  clean     - Remove build files"
	@echo "  cleanall  - Remove build files and data directory"
	@echo "  rebuild   - Clean and build"
	@echo "  run       - Build and run the game"
	@echo "  debug     - Build with debug symbols"
	@echo "  release   - Build optimized release version"
	@echo "  install-deps - Install raylib dependencies (Ubuntu/Debian)"
	@echo "  help      - Show this help message"

# Phony targets
.PHONY: all clean cleanall rebuild install-deps run debug release help
