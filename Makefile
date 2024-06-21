# Define variables
CC = gcc
CFLAGS = -Wall -lm -lgsl -lgslcblas
TARGET = lambert
PYTHON_SCRIPT = visualize_lambert.py
OUTPUT_FILE = output.txt

# Default target
all: install_deps $(TARGET) $(OUTPUT_FILE) run

# Install C and Python dependencies
install_deps:
	@echo "Installing C dependencies using brew..."
	brew update
	brew install gsl
	@echo "Installing Python dependencies using pip..."
	pip install matplotlib numpy

# Compile the C program
$(TARGET): lambert.c
	$(CC) $(CFLAGS) -o $(TARGET) lambert.c

# Generate the output.txt file
$(OUTPUT_FILE): $(TARGET)
	./$(TARGET)

# Run the Python script
run: $(OUTPUT_FILE)
	python $(PYTHON_SCRIPT)

# Clean the build
clean:
	rm -f $(TARGET) $(OUTPUT_FILE)

# Phony targets
.PHONY: all install_deps run clean
