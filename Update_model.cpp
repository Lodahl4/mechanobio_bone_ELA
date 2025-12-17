/************************************************************

    Algorithm to update material properties
         in each of the elements
 
************************************************************/

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include "header.h"

using namespace std;


void Update_model(char lattice[LATTICE_X][LATTICE_Y][LATTICE_Z], float Young_modulus_prop[NUMBER_ELEMS][NUMBER_ITERATIONS], float Poison_ratio_prop[NUMBER_ELEMS][NUMBER_ITERATIONS], 
	int t_prop,int element_local_min[NUMBER_ELEMS][3], int element_local_max[NUMBER_ELEMS][3],int lattice_point_element[LATTICE_X][LATTICE_Y][LATTICE_Z])
{    
    float* Young_mod_elem = new float[NUMBER_ELEMS];
	float* Poison_rat_elem = new float[NUMBER_ELEMS];
     
	int elem;
    int i,j,k;
    int imin,jmin,kmin;
    int imax,jmax,kmax;
    float total;
    float number_MSC,number_chondrocytes,number_fibroblasts,number_mature_osteoblasts,number_inmature_osteoblasts;
    int count;
    int count2;
    int first_element = 1; // first element 
     
    for(elem=0;elem<NUMBER_ELEMS;elem++)
    {
        Young_mod_elem[elem]=Poison_rat_elem[elem]=0.0;
    }
     
    for (elem=0;elem<NUMBER_ELEMS;elem++)
    {
            
        imin=element_local_min[elem][0];
        jmin=element_local_min[elem][1];
        kmin=element_local_min[elem][2];
        
        imax=element_local_max[elem][0];
        jmax=element_local_max[elem][1];
        kmax=element_local_max[elem][2];
         
        total=0.0;
        
        for (i=imin;i<imax;i++)
        {
            for (j=jmin;j<jmax;j++)
            {
                for (k=kmin;k<kmax;k++)
                {
                   if (lattice_point_element[i][j][k]==elem+1)
                   {
                        total=total+1.0;
                   }
                }
            }
        }
                        
        if (total==0.0)
        {
            Young_mod_elem[elem]=0.2;
            Poison_rat_elem[elem]=0.167;
        }
        else
        {
         
            number_MSC=number_chondrocytes=number_inmature_osteoblasts=number_fibroblasts=number_mature_osteoblasts=0;
    
            for (i=imin;i<imax;i++)
            {
                for (j=jmin;j<jmax;j++)
                {
                    for (k=kmin;k<kmax;k++)
                    {
                        if (lattice_point_element[i][j][k]==elem+1)
                        {
                            switch (lattice[i][j][k])
                            {
                                case 1:
                                    number_MSC=number_MSC+1.0;                                  // Mesenchymal Stem Cells
                                    break;
                                case 2:
                                    number_mature_osteoblasts=number_mature_osteoblasts+1.0;    // Mature Osteoblasts (bone-forming)
                                    break;
                                case 3:
                                    number_inmature_osteoblasts=number_inmature_osteoblasts+1.0;// Immature Osteoblasts (woven bone)
                                    break;
                                case 4:
                                    number_chondrocytes=number_chondrocytes+1.0;                // Chondrocytes (cartilage)
                                    break;
                                case 5:
                                    number_fibroblasts=number_fibroblasts+1.0;                  // Fibroblasts (fibrous tissue)
                                    break;    
                            }   
                        }
                    }
                }
            } //end for
            
        // Young's Modulus (MPa)
        Young_modulus_prop[elem][t_prop] = 
        (number_chondrocytes * 10.0 / total) +                  // Cartilage: 10 MPa
        (number_fibroblasts * 2.0 / total) +                    // Fibrous tissue: 2 MPa
        (number_inmature_osteoblasts * 1000.0 / total) +        // Immature Osteoblasts: 1000 MPa
        (number_mature_osteoblasts * MAX_STIFFNESS / total) +   // Bone: MAX_STIFFNESS MPa
        ((total-number_fibroblasts-number_inmature_osteoblasts-number_chondrocytes-number_mature_osteoblasts) * 0.2 / total);   // MSC/empty: 0.167                      // MSC/empty: 0.2 MPa
            
        // Poisson's Ratio
        Poison_ratio_prop[elem][t_prop] = 
        (number_chondrocytes * 0.3 / total) +                   // Cartilage: 0.3
        (number_fibroblasts * 0.167 / total) +                  // Fibrous tissue: 0.167
        (number_inmature_osteoblasts * 0.3 / total) +           // Immature Osteoblasts: 0.3
        (number_mature_osteoblasts * 0.3 / total) +             // Bone: 0.3
        ((total-number_fibroblasts-number_inmature_osteoblasts-number_chondrocytes-number_mature_osteoblasts)*0.167/total);   // MSC/empty: 0.167                 // MSC/empty: 0.167  
        
        // Handle initial setup (t_prop = -1) separately
        if (t_prop == -1)
        {
            // For initial setup, use the initial material properties directly
            Young_mod_elem[elem] = Young_modulus_prop[elem][-1];
            Poison_rat_elem[elem] = Poison_ratio_prop[elem][-1];
        }
        else
        {
            i=t_prop;         
            count=0;

            // Average over last 10 iterations            
            while (i>=0 && count<10)    
            {
                Young_mod_elem[elem]=Young_mod_elem[elem]+Young_modulus_prop[elem][i];
                Poison_rat_elem[elem]=Poison_rat_elem[elem]+Poison_ratio_prop[elem][i];
                count=count+1;
                i=i-1;
            }
             
            // Handle cases with fewer than 10 iterations: added to consider change of material properties
            if (count<=9)
            {
                count2=10-count;
                // Fill with average of existing counts instead of default values
                float avg_young = Young_mod_elem[elem] / count;
                float avg_poison = Poison_rat_elem[elem] / count;
                Young_mod_elem[elem]=Young_mod_elem[elem]+avg_young*count2;
                Poison_rat_elem[elem]=Poison_rat_elem[elem]+avg_poison*count2;
            }
            //*******************************************************************
            // Final averaging 
            Young_mod_elem[elem]=Young_mod_elem[elem]/10;
            Poison_rat_elem[elem]=Poison_rat_elem[elem]/10;
        }
        //cout<<"Element "<<elem+1<<": Young's Modulus = "<<Young_mod_elem[elem]<<" MPa, Poisson's Ratio = "<<Poison_rat_elem[elem]<<endl;
        } // end else total==0.0  
         
    }// end for element
     

	 //************* Write to file ************

	system("del simple.inp");  // rm in LINUX; del in WINDOWS
	system("copy Model.txt simple.inp"); // cp in LINUX; copy in WINDOWS
	ofstream myfile;
	myfile.open("simple.inp",ios::app);
	for (elem=0;elem<NUMBER_ELEMS;elem++)
	{
		ostringstream s1;
		s1<<"MAT_"<<elem+first_element;
		string materialname=s1.str(); 
		myfile<<"\n*Material, name="<<materialname<<endl;
		myfile<<"*Elastic"<<endl;
		myfile<<Young_mod_elem[elem]<<","<<Poison_rat_elem[elem];
	}
		 
	//*** Write to file rest of FEM information
	myfile<<"\n** ----------------------------------------------------------------"<<endl;
	myfile<<"** "<<endl;
	myfile<<"** STEP: Step-1"<<endl;
	myfile<<"** "<<endl;
	myfile<<"*Step, name=Step-1, nlgeom=NO"<<endl;
	myfile<<"*Static"<<endl;
	myfile<<"1., 1., 1e-05, 1."<<endl;
	myfile<<"** "<<endl;
	myfile<<"** BOUNDARY CONDITIONS"<<endl;
	myfile<<"** "<<endl;
	myfile<<"** Name: BC-1 Type: Symmetry/Antisymmetry/Encastre"<<endl;
	myfile<<"*Boundary"<<endl;
	myfile<<"m_Set-1, ENCASTRE"<<endl;
	// myfile<<"** Name: BC-2 Type: Displacement/Rotation"<<endl;
	// myfile<<"*Boundary"<<endl;
	// myfile<<"RP, 6, 6"<<endl;
	myfile<<"** "<<endl;
	myfile<<"**--------------  Loads  ------------------------------------------"<<endl;
	myfile<<"**"<<endl;
	myfile<<"**"<<endl;
	// myfile<<"** Name: Load-1   Type: Concentrated force"<<endl;
	// myfile<<"*Cload, op=NEW"<<endl;
	// myfile<<"RP, 3, -16.8"<<endl;    // in N
	// myfile<<"** Name: Load-2   Type: Concentrated force"<<endl;     // Moment created as (force x proximal-distal-distance) at RP 
	// myfile<<"*Cload, op=NEW"<<endl;      
	// myfile<<"RP, 2, 4.7"<<endl;      // in N (13.8 x BW mm)
	myfile<<"** Name: Load-3   Type: Moment (Torsional load around z-axis)"<<endl;
	myfile<<"*Cload, op=NEW"<<endl; 
	myfile<<"RP, 6, 24.0"<<endl;      // Moment about z-axis in N·mm
	myfile<<"** "<<endl;
	myfile<<"**"<<endl;
	myfile<<"**---------------- OUTPUT REQUESTS -------------------------------"<<endl;
	myfile<<"** "<<endl;
	myfile<<"*Restart, write, frequency=0"<<endl;
	myfile<<"** "<<endl;
	myfile<<"** FIELD OUTPUT: F-Output-1"<<endl;
	myfile<<"**"<<endl; 
	myfile<<"*Output, field"<<endl;
	myfile<<"*Element Output, directions=YES"<<endl;
	myfile<<"E, S"<<endl;
	myfile<<"**"<<endl;
	myfile<<"**"<<endl;
	myfile<<"*EL PRINT, FREQUENCY=100"<<endl;
	myfile<<"EP, SP"<<endl;                     // Alternative: EP1, PRESS (for Minimal Principal Strain and Hydrostatic Pressure)
	myfile<<"**"<<endl;
	myfile<<"**"<<endl;
	myfile<<"** HISTORY OUTPUT: H-Output-1"<<endl;
	myfile<<"**"<<endl;
	myfile<<"**"<<endl;
	myfile<<"*Output, history"<<endl;
	myfile<<"*Energy Output"<<endl;
	myfile<<"ALLAE,"<<endl;
	myfile<<"*End Step"<<endl;
    
    myfile.close();        
	
	delete[] Young_mod_elem;
	delete[] Poison_rat_elem;
    
}
