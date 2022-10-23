CC=gcc
CC_FLAGS=-O3 -std=gnu11 -Wno-unused-variable -Wall -Wextra -pedantic -march=native -mtune=native

DEPS = 
OBJFILES = monte_carlo_seq.o
TARGET = monte_carlo_seq

all: $(TARGET)

%.o: %.c $(DEPS)
	$(CC) -c $(CC_FLAGS) $< -o $@

$(TARGET): $(OBJFILES)
	$(CC) $(CC_FLAGS) -o $(TARGET) $(OBJFILES)

clean:
	rm -f $(OBJFILES) $(TARGET) *~
