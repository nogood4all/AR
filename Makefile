CC = g++
CPP_FLAGS = -std=c++11 -Wall
FLEX = flex
YACC = bison


EXECUTABLE = BirkhoffInteractiveSolver

SOLVERDIR = src/solver
PARSERDIR = src/parser
OBJDIR = obj
BUILDDIR = bin

SOURCES_PARSER = src/parser/parser.cpp 	src/parser/lexer.cpp
SOURCES_SOLVER = $(wildcard src/solver/*.cpp)

OBJECTS_SOLVER = $(patsubst $(SOLVERDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES_SOLVER))
OBJECTS_PARSER = $(patsubst $(PARSERDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES_PARSER))


.PHONY: all clean

all: parser $(BUILDDIR)/$(EXECUTABLE)

$(BUILDDIR)/$(EXECUTABLE): $(OBJECTS_PARSER) $(OBJECTS_SOLVER)
	$(CC) $(CPP_FLAGS) -o $@ $^

$(OBJECTS_SOLVER): $(OBJDIR)/%.o : $(SOLVERDIR)/%.cpp
	$(CC) $(CPP_FLAGS) -c -o $@ $<

$(OBJECTS_PARSER): $(OBJDIR)/%.o : $(PARSERDIR)/%.cpp
	$(CC) $(CPP_FLAGS) -Wno-unused-function -c -o $@ $<

parser: $(PARSERDIR)/parser.cpp $(PARSERDIR)/lexer.cpp

$(PARSERDIR)/parser.cpp: $(PARSERDIR)/parser.ypp
	$(YACC) -d -o $@ $<

$(PARSERDIR)/lexer.cpp: $(PARSERDIR)/lexer.lpp
	$(FLEX) -o $@ $<

clean:
	rm -f $(BUILDDIR)/* $(OBJDIR)/* $(SOLVERDIR)/*~ $(PARSERDIR)/lexer.cpp $(PARSERDIR)/parser.cpp $(PARSERDIR)/parser.hpp
