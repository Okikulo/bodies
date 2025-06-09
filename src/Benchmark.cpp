#include "Benchmark.hpp"
#include <iostream>
#include <iomanip>

Benchmark::Benchmark(const std::string& impl, int bodies) 
    : implementation(impl), numBodies(bodies), totalFPS(0.0), frameCount(0) {
}

void Benchmark::addFrame(double fps) {
    // Skip the first 100 frames to avoid initialization overhead
    if (frameCount >= 100) {
        totalFPS += fps;
    }
    frameCount++;
}

double Benchmark::getAverageFPS() const {
    if (frameCount <= 100) return 0.0;
    return totalFPS / (frameCount - 100);
}

void Benchmark::saveResults(const std::string& filename) {
    if (frameCount <= 100) {
        std::cout << "Not enough frames recorded for benchmark." << std::endl;
        return;
    }
    
    double avgFPS = getAverageFPS();
    
    // Check if file exists to determine if we need to write header
    std::ifstream checkFile(filename);
    bool fileExists = checkFile.good();
    checkFile.close();
    
    // Open file in append mode
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << filename << " for writing." << std::endl;
        return;
    }
    
    // Write header if file is new
    if (!fileExists) {
        file << "Implementation,NumBodies,AverageFPS\n";
    }
    
    // Write data
    file << implementation << "," << numBodies << "," 
         << std::fixed << std::setprecision(2) << avgFPS << "\n";
    
    file.close();
    
    std::cout << "Benchmark saved: " << implementation << " with " << numBodies 
              << " bodies, Average FPS: " << std::fixed << std::setprecision(2) 
              << avgFPS << std::endl;
}
