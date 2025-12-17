#include <cstdlib>        //Includes pseudo-random number generation
#include <iostream>       //To read and write data from files as input/output streams
#include <fstream>
#include <cmath>
#include <stdio.h>
#include <cmath>
#include <math.h>
#include "header.h"


using namespace std;

void Initialize_lattice(char initial_lattice[LATTICE_X][LATTICE_Y][LATTICE_Z], short initial_age[LATTICE_X][LATTICE_Y][LATTICE_Z], 
    int lattice_point_element[LATTICE_X][LATTICE_Y][LATTICE_Z], float Young_modulus[NUMBER_ELEMS][NUMBER_ITERATIONS])
{
    // int elem;
    // int free_positions;
    // int points=0;
    // int r1,r2,r3;
    // int osteoblasts_to_seed;
    // int seeded_cells=0;
    int i,j,k;
    // int i1,i2;
	
	// Initialize bone regions based on material properties and seed MSCs
	for (i=0; i<LATTICE_X; i++) {
		for (j=0; j<LATTICE_Y; j++) {
			for (k=0; k<LATTICE_Z; k++) {
				// Check if this lattice point is inside an element
				if (lattice_point_element[i][j][k] > 0) {
					int elem = lattice_point_element[i][j][k] - 1; // Convert to 0-based index
					
					// Calculate seeding probability for mature osteoblasts based on Young's modulus
					float osteoblast_probability = Young_modulus[elem][0] / MAX_STIFFNESS;
					
					// Seed mature osteoblasts with probability based on Young's modulus
					if ((float)rand() / RAND_MAX < osteoblast_probability) {
						initial_lattice[i][j][k] = 2; // mature osteoblast
						initial_age[i][j][k] = 6; // with age 6
					}
					// If lattice point is still free (0) and inside an element, seed MSCs with xx/100 probability
					else if (initial_lattice[i][j][k] == 0 && nrand(100) < 30) { // xx% chance to seed MSC
						initial_lattice[i][j][k] = 1;  // seed one MSC
						initial_age[i][j][k] = 6; // with age 6
					}
				}
			}
		}
	}
	
	// // Initialize bone regions based on material properties and seed MSCs
	// for (i=0; i<LATTICE_X; i++) {
	// 	for (j=0; j<LATTICE_Y; j++) {
	// 		for (k=0; k<LATTICE_Z; k++) {
	// 			// Check if this lattice point is inside an element
	// 			if (lattice_point_element[i][j][k] > 0) {
	// 				int elem = lattice_point_element[i][j][k] - 1; // Convert to 0-based index
	// 				// If Young's modulus >= 7500, set as bone (case 2)
	// 				if (Young_modulus[elem][0] >= 7500.0) {
	// 					initial_lattice[i][j][k] = 2; // bone
	// 					initial_age[i][j][k] = 1; // with age 1
	// 				}
	// 				// If lattice point is still free (0) and inside an element, seed MSCs with 30% probability
	// 				else if (initial_lattice[i][j][k] == 0 && nrand(3) == 0) {
	// 					initial_lattice[i][j][k] = 1;  // seed one MSC
	// 					initial_age[i][j][k] = 1; // with age 1
	// 				}
	// 			}
	// 		}
	// 	}
	// }
}
