#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <cmath>
#include "header.h"

using namespace std;

void Write_lattice_file_gnuplot(const char lattice[LATTICE_X][LATTICE_Y][LATTICE_Z], int iteration)
{
    // Write one slice in the middle of LATTICE_X (LATTICE_Y x LATTICE_Z slice) to a text file for gnuplot visualization
    int middle_x = LATTICE_X / 2;      // Middle position in X direction
    
    ostringstream f;
    f << "output\\lattice_slice_" << iteration << ".txt";
    string filename = f.str();
    
    ofstream file(filename.c_str());
    
    if (file) 
    {
        // Write header comment
        file << "# Lattice slice at iteration " << iteration << endl;
        file << "# Slice at X=" << middle_x << " (middle of LATTICE_X)" << endl;
        file << "# Matrix format: " << LATTICE_Y << " x " << LATTICE_Z << endl;
        file << "# Cell types: 0=empty, 1=MSC, 2=osteoblast, 3=immature_osteoblast, 4=chondrocyte, 5=fibroblast" << endl;
        file << endl;
        
        // Write slice at middle X position (LATTICE_Y x LATTICE_Z)
        file << "# Slice (X=" << middle_x << ")" << endl;
        for (int j = 0; j < LATTICE_Y; j++) 
        {
            for (int k = 0; k < LATTICE_Z; k++) 
            {
                file << (int)lattice[middle_x][j][k];
                if (k < LATTICE_Z - 1) file << " "; // Add space between values except for last column
            }
            file << endl; // New line after each row
        }
        
        file.close();
        cout << "Written lattice slice to: " << filename << endl;
    }
    else 
    {
        cout << "ERROR: Impossible to open file " << filename << endl;
    }
}