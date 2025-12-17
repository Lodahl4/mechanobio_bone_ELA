#include <iostream>
#include <fstream>
#include <sstream>
#include "header.h"

using namespace std;

void Read_material_file(float Young_mod_elem[NUMBER_ELEMS][NUMBER_ITERATIONS], float Poison_rat_elem[NUMBER_ELEMS][NUMBER_ITERATIONS])
{
    ifstream infile("material_file.txt");
    
    if (!infile.is_open())
    {
        cout << "Warning: Cannot open material_file.txt, using default values" << endl;
        return;
    }
    
    string line;
    while (getline(infile, line))
    {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#')
            continue;
        
        // Parse line format: "material_number, young_modulus, poisson_ratio"
        istringstream iss(line);
        string material_str, young_str, poisson_str;
        
        if (getline(iss, material_str, ',') && 
            getline(iss, young_str, ',') && 
            getline(iss, poisson_str))
        {
            try {
                int material_num = stoi(material_str) - 1; // Convert to 0-based index
                float young_mod = stof(young_str);
                float poisson_rat = stof(poisson_str);
                
                // Check bounds
                if (material_num >= 0 && material_num < NUMBER_ELEMS)
                {
                    // Initialize all iterations for this element with the same values
                    for(int i = 0; i < NUMBER_ITERATIONS; i++) {
                        Young_mod_elem[material_num][i] = young_mod;
                        Poison_rat_elem[material_num][i] = poisson_rat;
                    }
                    
                    // Debug output (uncomment if needed)
                    // cout << "Material " << material_num + 1 << ": E = " 
                    //      << young_mod << ", nu = " << poisson_rat << endl;
                }
                else
                {
                    cout << "Warning: Material number " << material_num + 1 << " out of range" << endl;
                }
            }
            catch (...)
            {
                cout << "Error parsing line: " << line << endl;
            }
        }
    }
    
    infile.close();
    // cout << "Material properties read from material_file.txt" << endl;
}