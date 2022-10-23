CC=gcc
MPI_CC=mpicc
CC_FLAGS=-O3 -std=gnu11 -Wno-unused-variable -Wall -Wextra -pedantic -march=native -mtune=native

.PHONY: all
all: heat_stencil_1D_seq heat_stencil_1D_par


heat_stencil_1D_seq: heat_stencil_1D_seq.c
	@$(CC) $(CC_FLAGS) $< -o heat_stencil_1D_seq

heat_stencil_1D_par: heat_stencil_1D_par.c
	@$(MPI_CC) $(CC_FLAGS) $< -o heat_stencil_1D_par

.PHONY: clean
clean:
	@rm heat_stencil_1D_seq
	@rm heat_stencil_1D_par
