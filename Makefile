CXX = g++
CXXFLAGS = -std=c++17 -g -Wall -Wextra
BUILDDIR = build
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:%.cpp=$(BUILDDIR)/%.o)
TARGET = $(BUILDDIR)/mechanobio.exe

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BUILDDIR)
	$(CXX) $(OBJECTS) -o $@

$(BUILDDIR)/%.o: %.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR):
	mkdir $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)

.PHONY: all clean