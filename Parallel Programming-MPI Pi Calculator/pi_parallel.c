/*##
Name: Benett Holmstrom
ID: HOLMS115, 5475564

pi_parallel: This program will compute pi based off of the Monte Carlo Approximation.
The Monte Carlo Approximation calculates pi by generating a given amount of points
within a square with an engraved circle, where the circle has radius r and the square has
length and width 2r. The area of our circle divided by the area of our square is pi / 4.
The points generated within these shapes help us to approximate the area: 
pi = 4 x (total_num_inside / num_tosses), where a "toss" is a generated point.
*/

#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*## Local functions */
void Get_input(int argc, char* argv[], int my_rank, long* num_tosses_ptr);
long Toss (long num_task_tosses, int my_rank);

int main(int argc, char** argv) {

/*## Variable instance field
	- num_tasks: The number of processor cores MPI will use in this program (in my case with my home PC, I can provide up to 8)
	- my_rank: Incremental rank provided by MPI for all tasks running
	- num_tosses: Total number of points to be generated
	- num_task_tosses: Number of tosses being handled within 1 task
	- process_num_inside: The result of calling Toss() is stored in here, to be used within 1 task
	- total_num_inside: The total number of points generated in the circle
	- start: The real time this program was started on
	- finish: The real time this program ended on, used to calculate wall time with start
	- loc_elapsed, elapsed: Wall time taken to execute the program
	- monte_carlo_approx: The Monte Carlo approximation of pi
*/
   int my_rank, num_tasks;
   long num_tosses, num_task_tosses, process_num_inside, total_num_inside;
   double start, finish, loc_elapsed, elapsed, monte_carlo_approx;
   //## Using the actual value of pi can help us dictate how accurate our approximations are.
   double PI_ACTUAL = 3.141592653589793238462643;

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

   //## The number of tosses is provided by the user from the command line, upon executing pi_parallel.
   Get_input(argc, argv, my_rank, &num_tosses);

   num_task_tosses = num_tosses/num_tasks;

   //## MPI_Barrier: Communicates to all tasks in this program that they should remain blocked until each and every one reaches this routine. 
   MPI_Barrier(MPI_COMM_WORLD);
   start = MPI_Wtime();
   process_num_inside = Toss(num_task_tosses, my_rank);
   finish = MPI_Wtime();
   loc_elapsed = finish-start;

   MPI_Reduce(&loc_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
   MPI_Reduce(&process_num_inside, &total_num_inside, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

   if (my_rank == 0) {
	   monte_carlo_approx = (4*total_num_inside)/((double) num_tosses);
	   printf("Wall Time: %f seconds \n", elapsed);
	   printf("Monte Carlo approximation of pi: %.16f, Absolute Error: %.16f, Relative Error: %.16f\n", monte_carlo_approx, fabs(PI_ACTUAL - monte_carlo_approx), fabs(PI_ACTUAL - monte_carlo_approx) / PI_ACTUAL);
   }
   MPI_Finalize();
   return 0;
}

/*## @void Get_input: Command line input is used to get the user's number of tosses to calculate
	Input: Command line arguments and the user's number of tosses, num_tosses_ptr
	Output: Number of tosses per task is calculated, and MPI handles the number of tasks to execute */
void Get_input(int argc, char* argv[], int my_rank, long* num_tosses_ptr){
	if (my_rank == 0) {
		if (argc!= 2){
		    fprintf(stderr, "Call: mpiexec -np <num_tasks> %s <num_tosses> \n", argv[0]);
            fflush(stderr);
            *num_tosses_ptr = 0;
		} else {
			*num_tosses_ptr = atoi(argv[1]);
		}
	}
	//## Communicates num_tosses to all tasks/cores
	MPI_Bcast(num_tosses_ptr, 1, MPI_LONG, 0, MPI_COMM_WORLD);

	//## Program cancels if num_tosses == 0
    if (*num_tosses_ptr == 0) {
        MPI_Finalize();
        exit(-1);
    }
}

/*## @long Toss: Named toss, as in randomly tossing darts at a board, similar to points randomly generating on a circle.
	Input: The number of tosses to be calculated per-task, task_tosses, and the current task being handled, my_rank
	Output: Number of tosses inside the given region */
long Toss (long task_tosses, int my_rank){
	long toss, num_inside = 0;
	double x,y;
	unsigned int seed = (unsigned) time(NULL);
	srand(seed + my_rank);
	for (toss = 0; toss < task_tosses; toss++) {
	   x = rand_r(&seed)/(double)RAND_MAX;
	   y = rand_r(&seed)/(double)RAND_MAX;
	   if((x*x+y*y) <= 1.0 ) num_inside++;
    }
    return num_inside;
}

