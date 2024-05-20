output: pi_parallel.o
	mpicc pi_parallel.o -o pi_parallel

pi_parallel.o: pi_parallel.c
	mpicc -Wall -g -c pi_parallel.c

clean:
	rm *.o output
