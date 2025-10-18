# Makefile for SB Compiler
CXX = g++
CXXFLAGS = -std=c++11 -Wall -O2
TARGET = compiler
SRCDIR = src
OBJDIR = obj
# Exclude simulador.cpp from compiler sources
COMPILER_SOURCES = $(filter-out $(SRCDIR)/simulador.cpp, $(wildcard $(SRCDIR)/*.cpp))
COMPILER_OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(COMPILER_SOURCES))

# Create obj directory if it doesn't exist
$(shell mkdir -p $(OBJDIR))

all: $(TARGET)

$(TARGET): $(COMPILER_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(COMPILER_OBJECTS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Specific dependencies for header files
$(OBJDIR)/compiler.o: $(SRCDIR)/compiler.cpp $(SRCDIR)/lexer.h $(SRCDIR)/preprocessor.h $(SRCDIR)/parser.h $(SRCDIR)/code_generator.h
$(OBJDIR)/lexer.o: $(SRCDIR)/lexer.cpp $(SRCDIR)/lexer.h
$(OBJDIR)/preprocessor.o: $(SRCDIR)/preprocessor.cpp $(SRCDIR)/preprocessor.h $(SRCDIR)/lexer.h
$(OBJDIR)/parser.o: $(SRCDIR)/parser.cpp $(SRCDIR)/parser.h $(SRCDIR)/lexer.h $(SRCDIR)/symbol_table.h
$(OBJDIR)/symbol_table.o: $(SRCDIR)/symbol_table.cpp $(SRCDIR)/symbol_table.h
$(OBJDIR)/code_generator.o: $(SRCDIR)/code_generator.cpp $(SRCDIR)/code_generator.h $(SRCDIR)/parser.h $(SRCDIR)/symbol_table.h
$(OBJDIR)/simulador.o: $(SRCDIR)/simulador.cpp

simulador: $(OBJDIR)/simulador.o
	$(CXX) $(CXXFLAGS) -o simulador $(OBJDIR)/simulador.o

clean:
	rm -rf $(OBJDIR) $(TARGET) simulador
	rm -f *.pre *.o1 *.o2
	rm -f $(SRCDIR)/*.pre $(SRCDIR)/*.o1 $(SRCDIR)/*.o2

test: $(TARGET)
	./$(TARGET) $(SRCDIR)/teste.asm
	@echo "Generated files:"
	@ls -la $(SRCDIR)/teste.pre $(SRCDIR)/teste.o1 $(SRCDIR)/teste.o2 2>/dev/null || true

test-macro: $(TARGET)
	./$(TARGET) $(SRCDIR)/teste_macro.asm
	@echo "Testing macro expansion..."
	@cat $(SRCDIR)/teste_macro.pre

test-complete: $(TARGET) simulador
	./$(TARGET) $(SRCDIR)/teste_completo.asm
	@echo "Running simulation with input 3..."
	@echo "3" | ./simulador $(SRCDIR)/teste_completo.o2

.PHONY: all clean test test-macro test-complete
