/** @file:     dna_match.c
 * @authors Benett Holmstrom, holms115@umn.edu and Gabe Hites, hites019@d.umn.edu
 * @section Description:  A program that implements the Needleman-Wunsch algorithm to find matching *   DNA Strings in parallel using OpenMP
 *
 * @section Compile:  gcc -g -Wall -fopenmp -o dna_match dna_match.c
 * @section Run:      ./dna_match <number of threads> <query_file> <db_file>
 *
 * @section Input:    A database file and a query file
 * @section Output:   The matching DNA Strings
 */

/** @section NOTES The command below can be used to generate the dna string files(db_file.txt and query_file.txt)
  < /dev/urandom tr -dc "ACGT" | head -c100000 | fold -w 2000 >> <file_name.txt> 
*/
#define match_size     200  //This is the maximum number of best matches that can be stored
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>

typedef struct {
  char match_string[2001];
  //char db_match_string[4001];
  int cost;
}match_data;

/*
How to parallelize this code fill the matrix M diagonally from right to left.
Within the diagonals multiple threads can work at once. Some diagonals will have different number of threads.
 */
//functions
int needleman_wunsch(char* db, char* query, int* M, int m, int n, char* D);
int score(char x, char y);
int Max(int a, int b, int c, char* D, int i, int j, int m);
int min(int a, int b);
char* reverse(char* string);

//global variable
int thread_count;

/*--------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
  if (argc != 4) Usage(argv[0]);
  thread_count = strtol(argv[1], NULL, 10);
  //@variables
   FILE* db_file;     /*Pointer to db_file*/
   FILE* query_file;   /* Pointer to query_file */
   char* query = NULL; /* Char* that stores the query_file string */
   char* line = NULL;  /* Char* that stores the db_file strings */
   size_t len = 0;	 /* The length of the string from db_file.txt */
   size_t query_len = 0; 	/* The length of the string from query_file.txt */
   size_t read; 		/* Used to store return value of getline(), corresponds to the number of bytes read or -1 if there was an error. */
   int dim_m, dim_n;             	/* Int values to be used for array dimenstions */
   //match_data matches[match_size]; //an array to store the best matches and their costs
   int match_cost;   /*stores the cost of the match from the algo*/

   clock_t start_t, end_t;
   double start, finish, total_t;

   /* Open the db_file, exit if failed */
   query_file = fopen(argv[2], "r");
   if (query_file == NULL)
     exit(EXIT_FAILURE);
   /* Open the query_file, exit if failed */
   db_file = fopen(argv[3], "r");
   if (db_file == NULL)
     exit(EXIT_FAILURE);

   /* Read query string from query_file */
   read = getline(&query, &query_len, query_file);
   if(read == -1) printf("Error: Failed to read query file.\n");

   /* Read one db string. The length of the string must be known before entering the do-while loop. Array M[][] needes to be delcared before the loop*/
   /* This is where the timer starts */
   start_t = clock();
   read = getline(&line, &len, db_file);
       dim_m = strlen(query);
       dim_n = strlen(line);
       //printf("query length: %d\ndb string length: %d\n", dim_m, dim_n);




match_data matches[match_size]; //an array to store the best matches and their costs

       /* Using dim_m and dim_n initialize M[][] to size dim_m x dim_n */
           //int M[dim_m][dim_n] = {0}; /*This way does not work unless array size is hardcoded*/

       int* M = malloc((dim_m) * (dim_n) *sizeof(int));
       for(int i=0; i<dim_m; i++){
	 for(int j=0; j<dim_n; j++){
	     M[dim_m*i + j] = 0;
	 }
       }



       char* D = malloc((dim_m) * (dim_n) *sizeof(char)); //matrix of same dimensions to M, whose characters correspond to the direction associated with the cost values in M 


       /*Add the base case for the 1st row and column  */
       for(int i=0; i<dim_n; i++)
        {
	   M[i] = -i;

	 }//end for

       for(int i=1; i<dim_m; i++)
	{
	 M[dim_n*i] = -i;
        }//end for



       int char_index = 0; //used to determine which location of the match_string[array] should be written to
       char dir; //character used to denote direction chosen
       int match_index = 0; //use this to count the number of strings in matches[]
       int matrix_cord;   //The coordinate the loop is currently on during the backtracking
       int row = dim_m-1; //used in for loop below to iterate over the rows
       int query_index, qi;
     /* Loop through the db_file line by line */
       start=omp_get_wtime();
   do{
     row = dim_m-1;

     //run algo to produce M

     //start = omp_get_wtime();
     match_cost = needleman_wunsch(line, query, M, dim_m, dim_n, D);
     finish = omp_get_wtime();
     //    printf("cost of match: %d\n", match_cost);
     matches[match_index].cost = match_cost;

     char_index = 0;  //reset char_index to zero
     query_index=0;

    //read M for best match, start at bottom right corner of matrix and trace backwards to find best match
     for(int col=dim_n-1; col>0; col--)
       {
	 matrix_cord = row*dim_n + col;
	dir = D[matrix_cord];

	if(dir == 'd')
	   {
	     qi = strlen(query)-2 - query_index; //-2 to account for the new line characters
	     matches[match_index].match_string[char_index] = query[qi];
	     //col--; loop does this
	     row--;
	     char_index++;
	     query_index++;
	   }
	 else if(dir == 'u')
	   {
	     //write gap to db string, project doesn't require for us to print it out though
	     row--;
	     col++;
	   }
	 else //if dir is l
	   {
	     matches[match_index].match_string[char_index] = '-';
	     //col--; loop does this
	     char_index++;
	   }
       }//end for

       match_index++;
       // }//end if
       }while ((read = getline(&line, &len, db_file)) != -1);
   /* End of db_file read do while loop*/


   int temp;
       for(int i=0; i<match_index; i++)
	 {
	   if(i==0) temp = matches[i].cost;
	   else if (temp < matches[i].cost)
	     {
	       temp = matches[i].cost;
	     }
	 }

       for(int i=0; i<match_index; i++)
	 {
	   if(matches[i].cost == temp) printf("C = %d    %s\n\n", matches[i].cost, reverse(matches[i].match_string));  
	 }

       //printf("OMPTime elapsed time: %f\n", finish-start);
   end_t = clock();
   total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
   printf("End of procedure, time elapsed: %f seconds\n", total_t/thread_count);

   /* Close files */
   fclose(query_file);
   fclose(db_file);

   /* Free pointers used in getline() */
   if (line)
     free(line);
   if (query)
     free(query);
   free(M);
   free(D);
   return 0;
}  /* main */

/**-------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print the appropriate use case of this program on the command line 
 * @param prog_name, the name of this file found in argv[0]
*/
void Usage(char* prog_name) {
   fprintf(stderr, "Use case: %s <thread_count> <query_file> <db_file>\n", prog_name);
   fprintf(stderr, "Please ensure that your file names are accurate and that your provided thread count is suitable with your machine's CPU.\n");
   exit(0);
} /* Usage */

/**------------------------------------------------------------------
 * Function:    needleman_wunsch
 *
 * Purpose:      Use the needleman wunsch algorithm on two strings to manipulate matrix M
 * @param db          the database string
 * @param query       the query string
 * @param M           pointer to m x n matrix
 * @param m           number of rows in matrix
 * @param n           number of columns in matrix
 * @return   return the final array entry
 */
int needleman_wunsch(char* db, char* query, int* M, int m, int n, char* D){
  int g = -1; // change this value to change the value of the gap
  int d, l, u;
  //serial version
  /*
  for(int i=1; i<m; i++)
    {
      for(int j=1; j<n; j++)
	{
	  d = M[(i-1)*n + (j-1)] + score(query[i-1], db[j-1]);
	  u = M[(i-1)*n + j] + g;
	  l = M[i*n + (j-1)] + g;
	  //printf("d: %d u: %d l: %d\n", d, u, l);
	      M[i*n + j] = max(d, u, l, D, i, j, n);
	  //printf("%d\n",M[m*i +j]); sleep(1);
	}
    }
  */

  //parallel version




  int i,j, k;
  //first half of matrix computations
   #pragma omp parallel for num_threads(thread_count) \
     default(none) shared(db, query, M, D, m, n, g) private(l, u, d, i, j,k)
  for( k=1; k<m; k++)
    {
      i=k;
      j=1;

      //#pragma omp parallel for num_threads(thread_count)		\
					// default(none) shared(db, query, M, D, m, n) private(i)
      for(i=i; i>0; i--)
	{
	  //j=diag-i;


	      d = M[(i-1)*n + (j-1)] + score(query[i-1], db[j-1]);
	      u = M[(i-1)*n + j] + g;
	      l = M[i*n + (j-1)] + g;
	      M[i*n + j] = Max(d, u, l, D, i, j, n);
	      //printf("%d\n", M[i*n+(j-1)]);
	      j++;

	}//end for
    }//end for
  // printf("starting 2nd half\n");
  //second half of computations

  #pragma omp parallel for num_threads(thread_count)			\
      default(none) shared(db, query, M, D, m, n, g) private(l,u,d, i, j,k)
  for( k=2; k<n; k++)
    {
      i=m-1;
      j=k;
      //#pragma omp parallel for num_threads(thread_count)	\
  // default(none) shared(db, query, M, D, m, n) private(j)
      for(j=j; j<n; j++)
	{
	  //	    if(i<20 || i>1799)printf("ij %d %d         \n",i,j);
	  d = M[(i-1)*n + (j-1)] + score(query[i-1], db[j-1]);
	  u = M[(i-1)*n + j] + g;
	  l = M[i*n + (j-1)] + g;
	  M[i*n + j] = Max(d, u, l, D, i, j, n);


	  if(i>1) i--;

	}//end for
    }// end for

  //printf("returning\n");
  return M[(m-1)*n+(n-1)];   //return the final entry in the matrix aka the cost
} /*needleman_wunsch  */


/**------------------------------------------------------------------
 * Function:     score
 *
 * Purpose:      Compare two characters and return 1 if they match, else return -1
 * @param x           stores character for comparison
 * @param y          stores character for comparison
 * @return          returns 1 or -1 (match or no match)
 */
int score(char x, char y){
  // printf("x: %c y: %c\n", x,y);
  if(x == y) return 1;
  else return -1;

} /* score */


/**------------------------------------------------------------------
 * Function:     max
 *
 * Purpose:          Take three ints and return the one with the highest value.
 * @param a          One of the ints to be compared
 * @param b          One of the ints to be compared
 * @param a          One of the ints to be compared
 * @return           return the integer with the maximum value
 */
int Max(int d, int u, int l, char* D, int i, int j, int n){

  //printf("a: %d b: %d c: %d\n",a,b,c);
  if(d>=u && d>=l)
    {D[i*n + j] = 'd';
            return d;}
  else if(u>=l)
    {D[i*n + j] = 'u';
      return u;}
  else
    {D[i*n + j] = 'l';
      return l;}
} /* max */

/**
  *Function: reverse
  *Purpose: reverse input string
  *@param string     the string to reverse
  *@return     return the reversed string
  */
char* reverse(char* string){
  int  length=strlen(string)-1;
  char temp;
  for(int i=0; i<strlen(string)/2 ;i++){

    temp=string[i];

    string[i]=string[length];

    string[length--]=temp;
  }//end for
  return string;
} /* reverse */




