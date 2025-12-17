#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <string>
#include "header.h"

using namespace std;

void Read_stimulus(char stimulus_read[NUMBER_ELEMS],int elements_read[NUMBER_ELEMS][NODES_PER_ELEM])
{
    int elem;
    float min_E_read;
    float HP_read;
    
    // Initialize the stimulus array to zeros
    for(int i = 0; i < NUMBER_ELEMS; i++) {
        stimulus_read[i] = 0;
    }
   
    ifstream infile("simple.dat");
	string temp;
 	string line;
   
    infile>>temp;
    
    while (temp!="FOOT-")
    {
    	infile>>temp;
    }
    getline (infile,line);
    getline (infile,line);
    getline (infile,line);
    // Ready to read first element data line
    // cout<<"Ready to read data"<<endl;

    float a,b,c,d,e,f;
    // int j;
    
    for (elem=0;elem<NUMBER_ELEMS;elem++)
    {
        min_E_read=0;       // min principal strain
        HP_read=0;          // hydrostatic stress
        a=b=c=d=e=f=0;
        
        // for (j=0;j<4;j++)
        // {
        //     infile>>temp;   //node
        //     infile>>a;      // e1 (e11)
        //     infile>>b;      // e2 (e22)
        //     infile>>c;      // e3 (e33)
        //     infile>>d;      // s1 (s11)
        //     infile>>e;      // s2 (s22)
        //     infile>>f;      // s3 (s33)

        //     min_E_read += a;
        //     HP_read += -(d+e+f)/3;
            
        //     infile>>temp;   //next element      
        //     // system("PAUSE");
        // }
        
        int element_num, pt;
        infile>>element_num;   // element number 
        infile>>pt;            // PT (point) - always 1
        infile>>a;             // EP1 (e1)
        infile>>b;             // EP2 (e2) 
        infile>>c;             // EP3 (e3)
        infile>>d;             // SP1 (s11)
        infile>>e;             // SP2 (s22)
        infile>>f;             // SP3 (s33)

        min_E_read += a;
        HP_read += -(d+e+f)/3;
            
        // min_E_read=min_E_read/4;
        // HP_read=HP_read/4;
        
        
        // Define stimulus - Priority order based on mechanobiology diagram

        // if (abs(HP_read)<0.0016 && abs(min_E_read)<0.0004) { // 0.04%; alternative for HP: 0.0016 MPa instead of 0.15 MPa.
        // 	stimulus_read[elem]=1;  // bone resorption
		// }
        if (abs(HP_read)<0.004 && abs(min_E_read)<0.001) { // 0.004 MPa; 0.1%; Model D in Postigo et al. 2014
        	stimulus_read[elem]=1;  // bone resorption
            
		
        }

        else if (abs(HP_read)<0.15 && abs(min_E_read)<0.05) {
        	stimulus_read[elem]=2; // intramembranous ossification
		}

        else if (HP_read>0.15 && abs(min_E_read)<0.15) {
        	stimulus_read[elem]=4; // endochondral ossification
		}

        else {
        	stimulus_read[elem]=5; // fibrocartilage or connective tissue	
		}

        // cout<<"Element "<<elem<<" min_E: "<<min_E_read<<endl;
        // cout<<"Element "<<elem<<" HP: "<<HP_read<<endl;
        // cout<<"Element "<<elem<<" Stimulus: "<<(int)stimulus_read[elem]<<endl;

    }

    // cout<<"Last element (650) min_E: "<<min_E_read<<endl;
    // cout<<"Last element (650) HP: "<<HP_read<<endl;
    // cout<<"Last element (650) Stimulus: "<<(int)stimulus_read[NUMBER_ELEMS-1]<<endl;
    // // Show some statistics
    // int count[6] = {0}; // count for stimulus types 0-5
    // for(int i = 0; i < NUMBER_ELEMS; i++) {
    //     if(stimulus_read[i] >= 0 && stimulus_read[i] <= 5) {
    //         count[static_cast<int>(stimulus_read[i])]++;
    //     }
    // }
    // cout<<"Stimulus distribution:"<<endl;
    // cout<<"Type 1 (low strain/stress): "<<count[1]<<endl;
    // cout<<"Type 2 (intramembranous): "<<count[2]<<endl;
    // cout<<"Type 4 (endochondral): "<<count[4]<<endl;
    // cout<<"Type 5 (fibrocartilage): "<<count[5]<<endl;
    // system("PAUSE");

}
