CC     = gcc
CLIBS  =
CFLAGS = -Wall -Wextra -std=gnu99
MPICC  = mpicc
MPIRUN = mpirun
HOSTS  = ~/hostfiles
NPROC  = 10
HYPERF = hyperfine
MKDIR  = mkdir -p
CLEAN  = rm -rf

CFLAGS += $(if $(DEBUG),-O0 -g3 -DDEBUG,-O2)

## Compile all implementations
all: main mpi omp

## Compile the serial implementation
main: src/main.c
	@$(MKDIR) build
	$(CC) -o build/$@ $(CFLAGS) $< $(CLIBS)

## Compile the MPI implementation
mpi: export OMPI_CC ?= $(CC)
mpi: src/mpi.c
	@$(MKDIR) build
	$(MPICC) -o build/$@ $(CFLAGS) $< $(CLIBS)

## Compile the OpenMP implementation
omp: CFLAGS += -fopenmp -DNPROC=$(NPROC)
omp: src/omp.c
	@$(MKDIR) build
	$(CC) -o build/$@ $(CFLAGS) $< $(CLIBS)

## Benchmark the implementations
bench: all
	$(if $(filter $(FASTQ),100 10K 100K),,\
		$(error FASTQ must be set to one of 100,10K,100K))
	@$(HYPERF) -w 1 -r 10 \
		--export-csv docs/benchmarks/$(FASTQ).csv \
		"build/main data/$(FASTQ).fastq out/main-$(FASTQ).tsv" \
		"build/omp data/$(FASTQ).fastq out/omp-$(FASTQ).tsv" \
		"$(MPIRUN) -np $(NPROC) -hostfile $(HOSTS) \
			build/mpi data/$(FASTQ).fastq out/mpi-$(FASTQ).tsv"

## Remove build files
.PHONY: clean
clean: ; @$(CLEAN) build
