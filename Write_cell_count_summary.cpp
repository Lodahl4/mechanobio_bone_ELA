#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <cmath>
#include "header.h"

using namespace std;

void Write_cell_count_summary(const char lattice[LATTICE_X][LATTICE_Y][LATTICE_Z], int iteration)
{
    // Count different cell types
    int empty_count = 0;          // 0: Empty/free space
    int msc_count = 0;            // 1: MSCs (Mesenchymal Stem Cells)
    int osteoblast_count = 0;     // 2: Mature osteoblasts
    int chondrocyte_count = 0;    // 4: Chondrocytes
    int fibroblast_count = 0;     // 5: Fibroblasts
    int other_count = 0;          // Any other values
    
    // Iterate through the entire lattice
    for (int i = 0; i < LATTICE_X; i++) {
        for (int j = 0; j < LATTICE_Y; j++) {
            for (int k = 0; k < LATTICE_Z; k++) {
                switch (lattice[i][j][k]) {
                    case 0:
                        empty_count++;
                        break;
                    case 1:
                        msc_count++;
                        break;
                    case 2:
                        osteoblast_count++;
                        break;
                    case 4:
                        chondrocyte_count++;
                        break;
                    case 5:
                        fibroblast_count++;
                        break;
                    default:
                        other_count++;
                        break;
                }
            }
        }
    }
    
    // Append to summary file
    ofstream file("output\\cell_count_summary.txt", ios::app);
    if (file) {
        // If this is the first iteration (iteration -1), write header
        if (iteration == -1) {
            file << "# Cell count summary over time" << endl;
            file << "# Columns: Iteration Empty MSCs Osteoblasts Chondrocytes Fibroblasts Other" << endl;
        }
        
        // Write data for this iteration
        file << iteration << " " << empty_count << " " << msc_count << " " 
             << osteoblast_count << " " << chondrocyte_count << " " 
             << fibroblast_count << " " << other_count << endl;
    } else {
        cout << "ERROR: Impossible to open cell count summary file" << endl;
    }
    
    file.close();
}