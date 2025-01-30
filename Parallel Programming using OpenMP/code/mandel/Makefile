all: mandel_seq

mandel_seq: mandel_seq.o pngwriter.o walltime.o
	gcc -O3 -fopenmp -o mandel_seq mandel_seq.o pngwriter.o walltime.o -lpng

mandel_seq.o: mandel_seq.c consts.h pngwriter.h walltime.h
	gcc -O3 -fopenmp -c mandel_seq.c

pngwriter.o: pngwriter.c pngwriter.h
	gcc -O3 -fopenmp -c pngwriter.c

walltime.o: walltime.c walltime.h
	gcc -O3 -fopenmp -c walltime.c

clean:
	rm -f *.o mandel_seq

.PHONY: all clean