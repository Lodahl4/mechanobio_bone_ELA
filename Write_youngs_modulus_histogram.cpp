#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <cmath>
#include <vector>
#include <algorithm>
#include "header.h"

using namespace std;

void Write_youngs_modulus_histogram(const float Young_modulus[NUMBER_ELEMS][NUMBER_ITERATIONS], int iteration, float bin_spacing)
{
    // Use only valid iteration data or initial values for iteration -1
    int iter_index = (iteration == -1) ? 0 : iteration;
    
    // Use fixed range from 0 to MAX_STIFFNESS
    float min_E = 0.0f;
    float max_E = static_cast<float>(MAX_STIFFNESS);
    
    // Calculate number of bins needed for the full range
    int num_bins = static_cast<int>((max_E - min_E) / bin_spacing) + 1;
    
    // Create histogram bins
    vector<int> histogram(num_bins, 0);
    vector<float> bin_centers(num_bins);
    
    // Calculate bin centers
    for (int i = 0; i < num_bins; i++) {
        bin_centers[i] = min_E + i * bin_spacing + bin_spacing / 2.0f;
    }
    
    // Count elements in each bin
    for (int elem = 0; elem < NUMBER_ELEMS; elem++) {
        float E = Young_modulus[elem][iter_index];
        int bin_index = static_cast<int>((E - min_E) / bin_spacing);
        
        // Ensure bin_index is within bounds
        if (bin_index >= num_bins) bin_index = num_bins - 1;
        if (bin_index < 0) bin_index = 0;
        
        histogram[bin_index]++;
    }
    
    // Check if file is empty to write header
    bool write_header = false;
    ifstream check_file("output\\youngs_modulus_histogram.txt");
    if (!check_file || check_file.peek() == ifstream::traits_type::eof()) {
        write_header = true;
    }
    check_file.close();
    
    // Append to summary file
    ofstream file("output\\youngs_modulus_histogram.txt", ios::app);
    if (file) {
        // Write header if file is empty
        if (write_header) {
            file << "# Young's modulus histogram over time" << endl;
            file << "# Bin spacing: " << bin_spacing << " MPa" << endl;
            file << "# Range: 0 to " << MAX_STIFFNESS << " MPa (" << num_bins << " bins)" << endl;
            file << "# Format: Iteration followed by element counts for each bin" << endl;
            file << "# Bin centers (MPa):";
            for (int i = 0; i < num_bins; i++) {
                file << " " << bin_centers[i];
            }
            file << endl;
        }
        
        // Write data for this iteration
        file << iteration;
        for (int i = 0; i < num_bins; i++) {
            file << " " << histogram[i];
        }
        file << endl;
        
        cout << "Young's modulus histogram written for iteration " << iteration << endl;
    } else {
        cout << "ERROR: Impossible to open Young's modulus histogram file" << endl;
    }
    
    file.close();
}