CC=mpicc
CC_FLAGS=-O3 -std=gnu11 -Wno-unused-variable -Wall -Wextra -pedantic -msse4.1 -march=native -mtune=native

DEPS = xorshiftSSE.h
OBJFILES = xorshiftSSE.o monte_carlo_mpi.o
TARGET = monte_carlo_mpi

all: $(TARGET)

%.o: %.c $(DEPS)
	$(CC) -c $(CC_FLAGS) $< -o $@

$(TARGET): $(OBJFILES)
	$(CC) $(CC_FLAGS) -o $(TARGET) $(OBJFILES)

clean:
	rm -f $(OBJFILES) $(TARGET) *~
