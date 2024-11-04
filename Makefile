CC = gcc
CFLAGS = -Wall -Wextra -g

# List of source files
SRCS = ex3.c boundedQueue.c queue.c

# List of object files
OBJS = $(SRCS:.c=.o)

# Name of the executable
TARGET = ex3.out

# Default rule
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) -pthread -lrt

# Rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)

