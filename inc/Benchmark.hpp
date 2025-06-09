#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <string>
#include <fstream>

class Benchmark {
private:
    std::string implementation;
    int numBodies;
    double totalFPS;
    int frameCount;
    
public:
    Benchmark(const std::string& impl, int bodies);
    
    // Add a frame's FPS measurement
    void addFrame(double fps);
    
    // Save the results to CSV file
    void saveResults(const std::string& filename = "benchmark_results.csv");
    
    // Get average FPS so far
    double getAverageFPS() const;
};

#endif // BENCHMARK_HPP
