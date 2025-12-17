#include <chrono>
#include <fstream>
#include <iostream>
#include "header.h"

using namespace std;

static auto simulation_start_time = std::chrono::high_resolution_clock::now();

void start_timer() {
    simulation_start_time = std::chrono::high_resolution_clock::now();
    cout << "Simulation timer started" << endl;
}

void write_timing_results() {
    auto simulation_end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(simulation_end - simulation_start_time);
    
    // Write timing results to file
    std::ofstream timing_file("output/simulation_timing.txt");
    if (timing_file.is_open()) {
        timing_file << "Simulation Timing Results\n";
        timing_file << "========================\n";
        timing_file << "Total runtime: " << duration.count() << " milliseconds\n";
        timing_file << "Total runtime: " << duration.count() / 1000.0 << " seconds\n";
        timing_file << "Total runtime: " << duration.count() / 60000.0 << " minutes\n";
        timing_file << "Number of iterations completed: " << NUMBER_ITERATIONS << "\n";
        timing_file << "Average time per iteration: " << (duration.count() / (double)NUMBER_ITERATIONS) << " milliseconds\n";
        timing_file.close();
        cout << "Timing results written to output/simulation_timing.txt" << endl;
    } else {
        cout << "Error: Could not create timing output file" << endl;
    }
    
    // Display timing results in console as well
    cout << "Total simulation runtime: " << duration.count() / 1000.0 << " seconds (" << NUMBER_ITERATIONS << " iterations)" << endl;
}