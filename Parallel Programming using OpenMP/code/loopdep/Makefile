CC ?= gcc
all: recur_seq recur_omp_private recur_omp_unroll chunky

recur_seq: recur_seq.o walltime.o
	$(CC) -O3 $^ -lm -o $@

recur_seq.o: recur_seq.c walltime.h
	$(CC) -c -O3 $<

recur_omp_private: recur_omp_private.o walltime.o
	$(CC) -O3 -fopenmp $^ -lm -o $@

recur_omp_unroll: recur_omp_unroll.o walltime.o
	$(CC) -O3 -fopenmp $^ -lm -o $@

chunky: chunky.o walltime.o
	$(CC) -O3 -fopenmp $^ -lm -o $@

recur_omp_private.o: recur_omp_private.c walltime.h
	$(CC) -c -O3 -fopenmp $<

recur_omp_unroll.o: recur_omp_unroll.c walltime.h
	$(CC) -c -O3 -fopenmp $<

chunky.o: chunky.cpp walltime.h
	$(CC) -c -O3 -fopenmp $<

walltime.o: walltime.c walltime.h
	$(CC) -c $<

clear:
	$(RM) *.o recur_seq recur_omp_private recur_omp_unroll chunky

.PHONY: all clean