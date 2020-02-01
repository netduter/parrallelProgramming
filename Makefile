CC     = gcc
CLIBS  =
CFLAGS = -Wall -Wextra -std=gnu99
MPICC  = mpicc
MPIRUN = mpirun
HOSTS  = ~/hostfiles
PROCS  = 4
HYPERF = hyperfine
MKDIR  = mkdir -p
CLEAN  = rm -rfv

CFLAGS += $(if $(DEBUG),-O0 -g3 -DDEBUG,-O2)

all: main mpi omp

main: src/main.c
	@$(MKDIR) build
	$(CC) -o build/$@ $(CFLAGS) $< $(CLIBS)

mpi: export OMPI_CC ?= $(CC)
mpi: src/mpi.c
	@$(MKDIR) build
	$(MPICC) -o build/$@ $(CFLAGS) $< $(CLIBS)

omp: CFLAGS += -fopenmp -DTHREADS=$(PROCS)
omp: src/omp.c
	@$(MKDIR) build
	$(CC) -o build/$@ $(CFLAGS) $< $(CLIBS)

bench: all
	$(if $(filter $(FASTQ),100 10K 100K),,\
		$(error FASTQ must be set to one of 100,10K,100K))
	@$(HYPERF) -w 1 -r 5 \
		--export-csv docs/benchmarks/$(FASTQ).csv \
		"build/main data/$(FASTQ).fastq build/main-$(FASTQ).tsv" \
		"build/omp data/$(FASTQ).fastq build/omp-$(FASTQ).tsv" \
		"$(MPIRUN) -np $(PROCS) -hostfile $(HOSTS) \
			build/mpi data/$(FASTQ).fastq build/mpi-$(FASTQ).tsv"

.PHONY: clean
clean: ; @$(CLEAN) build
