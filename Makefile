CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = backtest_system
OBJS = main.o user_management.o stock_data.o backtest.o

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lm

# Compile main.c
main.o: main.c structures.h user_management.h stock_data.h backtest.h
	$(CC) $(CFLAGS) -c main.c

# Compile user_management.c
user_management.o: user_management.c user_management.h structures.h
	$(CC) $(CFLAGS) -c user_management.c

# Compile stock_data.c
stock_data.o: stock_data.c stock_data.h structures.h
	$(CC) $(CFLAGS) -c stock_data.c

# Compile backtest.c
backtest.o: backtest.c backtest.h structures.h
	$(CC) $(CFLAGS) -c backtest.c

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)

# Clean all generated files including data files
cleanall: clean
	rm -f stock_data.csv users.csv

# Run the program
run: $(TARGET)
	./$(TARGET)

# Help message
help:
	@echo "Available targets:"
	@echo "  all      - Build the program (default)"
	@echo "  clean    - Remove object files and executable"
	@echo "  cleanall - Remove all generated files including data"
	@echo "  run      - Build and run the program"
	@echo "  help     - Show this help message"

.PHONY: all clean cleanall run help