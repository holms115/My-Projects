This directory is my final project for CS 4995: Parallel Programming. It is an implementation of the Needleman-Wunsch algorithm used for parsing DNA strings. 
My lab partner and I wrote this project using OpenMP and ran the project on the Minnesota Supercomputing Institute's suite of high performance computing clusters. The directory comes with a compiled and ready-to-run executable
dna_match which can be called by ./dna_match <thread_count> query_file.txt db_file.txt, where thread_count is a number representing the number of compute cores on your CPU. 
