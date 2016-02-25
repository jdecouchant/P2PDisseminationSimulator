# Declaration of variables
CC = g++ 
CC_FLAGS = -w -O2 -std=c++11 
LD_FLAGS = -lpthread

# File names
EXEC = run
SOURCES = $(wildcard *.cc)
HEADERS = $(wildcard *.hh)
OBJECTS = $(SOURCES:.cc=.o)

# Main target
$(EXEC): $(OBJECTS) $(HEADERS)
	$(CC) $(OBJECTS) $(LD_FLAGS) -o $(EXEC)

# To obtain object files
%.o: %.cc
	$(CC) -c $(CC_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)
