# Makefile for streamingservice

# Compiler to use
CC = g++

# Compiler flags
CFLAGS = -I./inc -Wall -g
LDFLAGS = `pkg-config --libs opencv4` -lopencv_dnn

# Target directory and executable
BINDIR = bin
TARGET = $(BINDIR)/videoservice

# Source files
SRCS = src/VideoService.cpp src/logger.cpp

# Headers
HEADERS = -I/usr/include/jsoncpp/ \
		  -I/usr/include/gstreamer-1.0/ \
		  -I/usr/include/glib-2.0/ \
		  -I/usr/lib/x86_64-linux-gnu/glib-2.0/include/ \
		  -I/usr/include/opencv4 \

LIBRARIES = -ljsoncpp -lgstreamer-1.0 -lglib-2.0 -lgobject-2.0

# Default target
all: $(TARGET)

# Create bin directory if it doesn't exist
$(BINDIR):
	@echo "Creating directory $(BINDIR)"
	@mkdir -p $(BINDIR)

# Compile and link in one step, output to bin/
$(TARGET): $(SRCS) | $(BINDIR)
	@echo "Compiling and linking $(TARGET)"
	$(CC) $(SRCS) -o $(TARGET) $(CFLAGS) $(HEADERS) $(LIBRARIES) $(LDFLAGS)

# Clean up
clean:
	@echo "Cleaning up..."
	rm -f $(TARGET)
	@if [ -d $(BINDIR) ] && [ -z "$$(ls -A $(BINDIR))" ]; then \
		rmdir $(BINDIR); \
		echo "Removed empty $(BINDIR) directory"; \
	fi

# Phony targets
.PHONY: all clean