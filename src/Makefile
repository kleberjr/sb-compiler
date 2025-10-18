# Makefile for SB Compiler
CXX = g++
CXXFLAGS = -std=c++11 -Wall -O2
TARGET = compiler
OBJS = compiler.o lexer.o preprocessor.o parser.o symbol_table.o code_generator.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

compiler.o: compiler.cpp lexer.h preprocessor.h parser.h code_generator.h
	$(CXX) $(CXXFLAGS) -c compiler.cpp

lexer.o: lexer.cpp lexer.h
	$(CXX) $(CXXFLAGS) -c lexer.cpp

preprocessor.o: preprocessor.cpp preprocessor.h lexer.h
	$(CXX) $(CXXFLAGS) -c preprocessor.cpp

parser.o: parser.cpp parser.h lexer.h symbol_table.h
	$(CXX) $(CXXFLAGS) -c parser.cpp

symbol_table.o: symbol_table.cpp symbol_table.h
	$(CXX) $(CXXFLAGS) -c symbol_table.cpp

code_generator.o: code_generator.cpp code_generator.h parser.h symbol_table.h
	$(CXX) $(CXXFLAGS) -c code_generator.cpp

clean:
	rm -f $(OBJS) $(TARGET) *.pre *.o1 *.o2

test: $(TARGET)
	./$(TARGET) teste.asm
	@echo "Generated files:"
	@ls -la teste.pre teste.o1 teste.o2 2>/dev/null || true
