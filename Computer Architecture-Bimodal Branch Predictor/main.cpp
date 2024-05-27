/* @author: Benett Holmstrom
// @ID: 5475564
// @main.c: Variable-bit bimodal branch predictor with saturating counters
// 	    To compile: g++ -g -w -o main main.cpp
*/
#include <fstream>
#include <iostream>
#include <math.h>

using namespace std;

int main (int argc, char *argv[]) {
    bool saturatingPredictor (int * table, int idx, char outcome);



    if (argc != 4) {
        cerr << "Use case: ./main <bit_size> <input_file.txt> <output_file.txt>\n";
        return 0;
    }

    // Variable declarations
    // @int bit_size: bit size of the predictor
    // @int bit_mask: number of bits used from each address
    // @int table_size: amount of indecies in table
    // @table: array of ints containing jump predictions
    // @int read_address: the address to be accessed, determined by the input file
    // @int idx: table index to be accessed, containing a branch prediction decision
    // @ char outcome: the real outcome made by an actual branch predictor. This predictor compares its predictions against these real outcomes
    int bit_size = atoi(argv[1]);
    int bit_mask = pow(2, bit_size) - 1;
    int table_size = pow(2, bit_size);
    int *table = new int[table_size * sizeof(int)];
    int read_address, idx;
    char outcome;

    for (int i = 0; i < table_size; i++) {
        table[i] = 2;
    }

    FILE* trace = fopen(argv[2], "r");
    if (!trace) {
        cerr << "Error: Input file not detected, please ensure the file is in the current working directory.\n";
        return 1;
    }

    ofstream output(argv[3], std::ios::out | std::ios::app);
    if (!output.is_open()) {
        cerr << "Error: Output file cannot be found, please ensure the file is in the current working directory or that it is spelled and being addressed correctly. \n";
        return 2;
    }

    // @int hits: number of correct predictions
    // @int num_instructions: number of total instructions processed
    int hits = 0;
    int num_instructions = 0;

    while (!feof(trace)) {

	// Scan the input file
	fscanf(trace, "%x %c", &read_address, &outcome);
        if (trace == NULL) break;

        idx = (read_address >> 2) & bit_mask;

        // Bimodal Predictor with variable memory size
        hits += saturatingPredictor (table, idx, outcome);

        num_instructions++;
    }

    // Calculate the accuracy rate of predictor
    float accuracy = ((float)hits * 100 / num_instructions + 0.5f);

    output << "Bit size: " << bit_size << ", Percent accuracy: " << accuracy << endl;

    fclose(trace);
    output.close();

    delete [] table;
}

/**
 * @saturatingPredictor: variable sized bimodal predictor using switch logic and a corresponding
 * 			 memory table of branch predictions to determine whether or not a jump should occur
 * 			 note: (11 - Strongly Taken, 10 - Weakly Taken, 01 - Weakly Not-Taken, 00 - Strongly Not-Taken)
 *
 * @param table: prediction table akin to memory
 * @param idx: location of the entry in the table
 * @param outcome: of the actual branch direction
 * @return: 1 if the table index matched the actual branch outcome, 0 otherwise
 */
bool saturatingPredictor (int * table, int idx, char outcome) {
    bool correct = 0;
    if (outcome == 't') {
        switch (table[idx]) {
            case 0:
            case 1:
                table[idx]++;
                break;
            case 2:
                table[idx]++;
            case 3:
                correct = 1;
                break;
        }
    } else {
        switch (table[idx]) {
            case 0:
                correct = 1;
                break;
            case 1:
                correct = 1;
            case 2:
            case 3:
                table[idx]--;
                break;
        }
    }
    return correct;
}
