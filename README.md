# mechanobio_bone_ELA

Mechanobiological model for bone regeneration adapted from that of Perrier-Metz C. et al to handle abitrary specimen. See license for acknowledgement.


Changes made by Emil Lodahl Andersen:

Core Simulation Files
main.cpp
Line 6: Added #include <fstream> for file operations
Lines 22-23: Added simulation timer initialization with start_timer()
Lines 41-47: Removed manual initialization of Young's modulus (0.2 MPa) and Poisson ratio (0.167) for first 6 iterations
Line 69: Added Read_material_file() to load material properties from external file
Lines 62-82: Removed geometry-based lattice initialization (bone, marrow cavity, fixation plate with ellipsoid calculations) - replaced with simple empty initialization
Line 117: Modified Initialize_lattice() call - added lattice_point_element and Young_modulus parameters
Lines 124-130: Changed initial write from iteration 999 to -1; added Write_lattice_file_gnuplot() and Write_cell_count_summary() calls
Lines 135-138: Added Update_model() and ABAQUS execution (run.bat) before main loop starts
Line 146: Removed iteration>5 condition - differentiation now starts from iteration 0
Lines 179-185: Added periodic output every iteration: Write_lattice_file_gnuplot(), Write_cell_count_summary() (and commented Write_cell_count_results())
Lines 189-194: Modified Update_model: removed iteration>5 check, added iteration%3==0 for every 3 iterations, added Write_youngs_modulus_histogram() with 100 MPa bins
Lines 207-208: ABAQUS run: removed iteration>5 check, changed run.bat to run.bat (Windows syntax)
Lines 217-218: Added write_timing_results() to output simulation timing data
Line 228: Removed explicit deletion of Young_modulus and Poisson_ratio arrays

header.h
Added all new functions and parameters and improved user interface (variables)
Line 1: CELL_DIAMETER moved to first position and reduced from 0.1mm to 0.045mm
Lines 2-4: LATTICE_X/Y/Z changed from fixed values (361, 361, 401) to calculated based on geometry and CELL_DIAMETER (89, 89, 156)
Line 5: NUMBER_NODES reduced from 38785 to 745
Line 6: NUMBER_ELEMS reduced from 26215 to 2634
Line 7: NUMBER_ITERATIONS reduced from 360 to 43 days
Line 8: NODES_PER_ELEM changed from 10 to 4 (tetrahedral elements)
Line 9: ACTIVITY_MAX reduced from 15 to 0 days (no latency period)
Line 10: Added MAX_STIFFNESS constant (5000 MPa) for mechanoregulation
Line 16: Added Read_material_file() function declaration
Line 20: Modified Initialize_lattice() signature - added lattice_point_element and Young_modulus parameters
Lines 36-39: Added output functions: Write_lattice_file_gnuplot(), Write_cell_count_results(), Write_cell_count_summary(), Write_youngs_modulus_histogram()
Line 41: Modified Update_model() signature parameter order
Lines 44-45: Added timer functions: start_timer() and write_timing_results()


Geometry & Mesh Processing
Read_node_file.cpp
None

Read_element_file.cpp
None

Read_material_file.cpp
New script. 
Reads material properties for each element and stores Elastic modulus and Poisson’s ratio for each element

Calculate_lattice.cpp
None

Extremes_element.cpp
None

Lattice_point_in_element.cpp
None

Plane_intersection.cpp
None

Cellular Behavior Modules
Initialize_lattice.cpp
Line 13: Modified function signature - added lattice_point_element and Young_modulus parameters
Lines 15-22: Commented out unused variable declarations (elem, free_positions, points, r1,r2,r3, osteoblasts_to_seed, seeded_cells, i1,i2)
Lines 25-47: Complete rewrite - replaced geometry-based seeding (marrow cavity + periosteum ellipsoid calculations) with element-based seeding using Young's modulus
Lines 34-37: Added probabilistic seeding of mature osteoblasts (type 2) based on Young_modulus[elem][0] / MAX_STIFFNESS ratio
Line 42: Changed MSC seeding to 30% probability within elements (removed geometric constraints)
Lines 38, 44: Changed initial cell age from 1 to 6
Removed: All marrow cavity dimensions and periosteum dimensions 

Cell_proliferation.cpp
Lines 87-96: Changed from conditional proliferation rates (high early / low late based on ACTIVITY_MAX) to fixed lower rates only (12%, 11%, 4%, 6% for MSCs, fibroblasts, chondrocytes, osteoblasts)

Cell_mitosis.cpp
None

Cell_differentiation.cpp
Line 38-40: Added apoptosis tracking variables (total_chondro_apop, total_fibro_apop, total_osteo_apop)
Line 109: Changed differentiation rate from conditional (30% early / 6% late) to fixed 6%
Line 209, 276, 339, 414: Added apoptosis counter accumulation for each tissue type per stimulus condition
Line 439: Updated output path from "differentiation.txt" to "output\\differentiation.txt"
Line 443-447: Added new apoptosis output file (apoptosis.txt) writing osteoblast, chondrocyte, and fibroblast apoptosis counts
Lines 21-24, 26-28: Removed unused variable declarations (points, i1-i3, j1-j3, k1-k3, inmature_osteoblasts)

Cell_migration.cpp
Lines 36-39: Commented out marrow cavity geometry variables (a, b, h) and changed dimensions 
Line 43: Removed marrow cavity ellipsoid condition check - now only migrates cells within finite elements
Lines 54-56: Updated from deprecated random_shuffle() to modern std::shuffle() with std::random_device and std::default_random_engine
Line 65: Removed marrow cavity migration zone check (1/3 height constraint) - commented out
Line 93: Updated output path from "migration_distribution.txt" to "output\\migration_distribution.txt"

Jump_migration.cpp
None

Neighbour_presence.cpp 
None

Cell_age.cpp


FEA Integration & I/O
Read_stimulus.cpp
Lines 17-20: Added stimulus array initialization to zeros before reading data
Line 22: Changed input filename from "without_scaffold.dat" to "simple.dat"
Lines 32-34: Removed "first element" read and debug comment
Lines 40, 48-66: Commented out old 4-node per element averaging loop
Lines 68-80: Replaced with single integration point reading (element number, PT, EP1-3, SP1-3 format)
Lines 82-84: Removed division by 4 (no averaging needed for single point data)
Lines 87-92: Added commented alternative threshold values (0.0016 MPa, 0.04%)
Line 93: Changed bone resorption threshold to 0.004 MPa and 0.1% with reference to Postigo et al. 2014 Model D
Lines 87-107: Restructured from nested if-else to priority-based else-if chain
Lines 113-132: Added extensive commented debugging code for statistics and validation

Update_model.cpp
Line 30: Added number_inmature_osteoblasts variable declaration
Line 33: Changed comment from "first element in callus" to "first element"
Lines 89-97: Added case 3 handling for immature osteoblasts (woven bone) with detailed cell type comments
Lines 109-111: Updated Young's modulus calculation - added immature osteoblasts (1000 MPa), changed mature osteoblasts from 17000 MPa to MAX_STIFFNESS, added detailed comments
Lines 114-116: Updated Poisson ratio calculation - added immature osteoblasts term (0.3) with cell type comments
Lines 121-130: Added special handling for initial setup when t_prop == -1
Lines 148-155: Changed averaging for cases <10 iterations - now uses average of existing values instead of fixed defaults (0.2/0.167)
Lines 166-254: Complete restructure - removed if (t_prop>=0) conditional wrapper, always writes file now
Line 168: Changed system commands from rm to del (Windows syntax)
Line 169: Changed from cp [Model.txt](http://_vscodecontentref_/0) without_scaffold.inp to copy [Model.txt](http://_vscodecontentref_/1) simple.inp (Windows + different filename)
Line 171: Changed filename from "without_scaffold.inp" to "simple.inp"
Lines 177-178: Changed material naming from "material_" to "MAT_"
Lines 183-234: Completely restructured FEM file output - new format with different step setup, simplified BC (only BC-1 ENCASTRE), removed BC-2 displacement constraints, changed loads from force (-823 N) + moment (9777 N·mm) to single torsional moment (24.0 N·mm about z-axis)
Line 244: Removed ELSET specification in *EL PRINT command

nrand.cpp
None

timer.cpp
The timer functions track simulation runtime using C++ <chrono> library.

extract_abaqus_data.py
New script
This script parses ABAQUS finite element model files (.inp) and extracts data for a mechanobiology simulation. It:
Extracts nodes from all model instances and renumbers them sequentially
Extracts elements (C3D4 tetrahedrons) and adjusts node connectivity across instances
Creates material assignments for each element based on elastic properties (Young's modulus, Poisson ratio)
Generates a model template with per-element section assignments

Output
Write_raw_lattice_file.cpp
None

Write_cell_count.cpp
New script
This function counts cell types across the 3D lattice and appends the results to a time-series summary file. It:
Counts cells in each category: empty spaces (0), MSCs (1), osteoblasts (2), chondrocytes (4), fibroblasts (5), and other
Appends one row per iteration to cell_count_summary.txt in space-separated format
Writes a header on the first iteration (iteration -1)

Write_youngs_modulus_histogram.cpp
New script
This function creates a histogram of Young's modulus values across all finite elements at a specific iteration. It:
Bins the data into fixed intervals from 0 to MAX_STIFFNESS using the specified bin spacing
Counts elements in each bin based on their current Young's modulus
Appends results to youngs_modulus_histogram.txt, with each row representing one iteration
Writes a header on first use with bin centers and metadata
