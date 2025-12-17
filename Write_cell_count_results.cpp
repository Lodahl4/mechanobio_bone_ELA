#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <cmath>
#include "header.h"

using namespace std;

void Write_cell_count_results(const char lattice[LATTICE_X][LATTICE_Y][LATTICE_Z], int iteration)
{
    // Count different cell types
    int empty_count = 0;          // 0: Empty/free space
    int msc_count = 0;            // 1: MSCs (Mesenchymal Stem Cells)
    int osteoblast_count = 0;     // 2: Mature osteoblasts
    int chondrocyte_count = 0;    // 4: Chondrocytes
    int fibroblast_count = 0;     // 5: Fibroblasts
    int other_count = 0;          // Any other values
    int total_lattice_points = LATTICE_X * LATTICE_Y * LATTICE_Z;
    
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
    
    // Create filename for this iteration
    ostringstream filename;
    filename << "output\\cell_count_" << iteration << ".txt";
    string name = filename.str();
    
    // Write results to file
    ofstream file(name.c_str());
    if (file) {
        file << "Cell Count Results for Iteration " << iteration << endl;
        file << "========================================" << endl;
        file << "Total lattice points: " << total_lattice_points << endl;
        file << "Empty spaces (0): " << empty_count << endl;
        file << "MSCs (1): " << msc_count << endl;
        file << "Mature osteoblasts (2): " << osteoblast_count << endl;
        file << "Chondrocytes (4): " << chondrocyte_count << endl;
        file << "Fibroblasts (5): " << fibroblast_count << endl;
        file << "Other cell types: " << other_count << endl;
        file << endl;
        file << "Cell type percentages:" << endl;
        file << "Empty spaces: " << (100.0 * empty_count / total_lattice_points) << "%" << endl;
        file << "MSCs: " << (100.0 * msc_count / total_lattice_points) << "%" << endl;
        file << "Mature osteoblasts: " << (100.0 * osteoblast_count / total_lattice_points) << "%" << endl;
        file << "Chondrocytes: " << (100.0 * chondrocyte_count / total_lattice_points) << "%" << endl;
        file << "Fibroblasts: " << (100.0 * fibroblast_count / total_lattice_points) << "%" << endl;
        file << "Other cell types: " << (100.0 * other_count / total_lattice_points) << "%" << endl;
        file << endl;
        file << "Raw data (space-separated):" << endl;
        file << empty_count << " " << msc_count << " " << osteoblast_count << " " 
             << chondrocyte_count << " " << fibroblast_count << " " << other_count << endl;
        
        cout << "Cell count results written to " << name << endl;
    } else {
        cout << "ERROR: Impossible to open file " << name << endl;
    }
    
    file.close();
}