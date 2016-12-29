# Resources
# http://stackoverflow.com/questions/10189931/including-multiple-sub-directories-in-a-make-file

# Executable name
EXE := gameboy.out

# C++ compiler
CXX := clang++
RM := -rm -rf
# Compiler and dynamic link flags:
CXXFLAGS  := -std=c++14 -Wall
LDFLAGS   := -lsfml-graphics -lsfml-window -lsfml-system
# Directories
SRCDIR := src
OBJDIR := obj
BUILDDIR := build
# Files
CPP_FILES := $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/context/*.cpp)
OBJ_FILES := $(addprefix $(OBJDIR)/, $(notdir $(CPP_FILES:.cpp=.o)))

# Creates obj folder
dummy_make_folder := $(shell mkdir -p $(OBJDIR) $(BUILDDIR))

# This prints out variables OBJ_FILES and CPP_FILES
all: $(BUILDDIR)/$(EXE)

$(BUILDDIR)/$(EXE): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Object recipe for .cpp in source directory
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Object recipe for .cpp in source directory
$(OBJDIR)/%.o: $(SRCDIR)/context/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	$(RM) *.out
	$(RM) *.o
	$(RM) *.d
	$(RM) $(SRCDIR)/*.gch
	$(RM) $(OBJDIR)
	$(RM) $(BUILDDIR)

print:
	$(info $$OBJ_FILES is [$(OBJ_FILES)])
	$(info $$CPP_FILES is [${CPP_FILES}])
