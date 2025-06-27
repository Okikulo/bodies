#include <SFML/Graphics.hpp>
#include <iostream>
#include <signal.h>
#include <atomic>
#include "Simulation.h"
#include "Extra.h"
#include "Benchmark.hpp"

// Global variables for signal handling
std::atomic<bool> shouldExit(false);
Benchmark* globalBenchmark = nullptr;

void signalHandler(int signum) {
    std::cout << "\nReceived shutdown signal. Saving results..." << std::endl;
    if (globalBenchmark) {
        globalBenchmark->saveResults();
    }
    shouldExit = true;
}

std::string detectImplementation(const std::string& binaryPath) {
    // Extract just the filename from the full path
    size_t lastSlash = binaryPath.find_last_of("/\\");
    std::string filename = (lastSlash != std::string::npos) ? 
                          binaryPath.substr(lastSlash + 1) : binaryPath;
    
    // Check if filename contains implementation indicators
    if (filename.find("omp") != std::string::npos || 
        filename.find("OpenMP") != std::string::npos ||
        filename.find("openmp") != std::string::npos) {
        return "OpenMP";
    } else if (filename.find("serial") != std::string::npos ||
               filename.find("Serial") != std::string::npos) {
        return "Serial";
    } else {
        // Default fallback - you can change this based on your preference
        return "Unknown";
    }
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [numBodies] [dt] [softening]\n";
    std::cout << "  numBodies: Number of bodies in simulation (default: 1000, min: 2)\n";
    std::cout << "  dt:        Time step for simulation (default: 0.001, min: 0.0001)\n";
    std::cout << "  softening: Softening parameter (default: 2.0, min: 0.1)\n";
    std::cout << "Example: " << programName << " 500 0.005 1.5\n";
}

int main(int argc, char* argv[]) {
    // Automatically detect implementation from binary name
    std::string implementation = detectImplementation(argv[0]);
    
    // Parse command line arguments
    int numBodies = 1000;
    float softening = 2.0f;
    float dt = 0.001f;
    
    if (argc > 1) {
        // Show help if requested
        if (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help") {
            printUsage(argv[0]);
            return 0;
        }
        
        // Parse numBodies
        try {
            numBodies = std::stoi(argv[1]);
            if (numBodies <= 1) {
                std::cout << "Number of bodies must be at least 2. Using default: 1000" << std::endl;
                numBodies = 1000;
            }
        } catch (const std::exception& e) {
            std::cout << "Invalid number of bodies. Using default: 1000" << std::endl;
            numBodies = 1000;
        }
    }
    
    if (argc > 2) {
        // Parse dt
        try {
            dt = std::stof(argv[2]);
            if (dt < 0.0001f) {
                std::cout << "Time step too small (min: 0.0001). Using default: 0.001" << std::endl;
                dt = 0.001f;
            } else if (dt > 1.0f) {
                std::cout << "Time step too large (max: 1.0). Using default: 0.001" << std::endl;
                dt = 0.001f;
            }
        } catch (const std::exception& e) {
            std::cout << "Invalid time step. Using default: 0.001" << std::endl;
            dt = 0.001f;
        }
    }
    
    if (argc > 3) {
        // Parse softening
        try {
            softening = std::stof(argv[3]);
            if (softening < 0.1f) {
                std::cout << "Softening too small (min: 0.1). Using default: 2.0" << std::endl;
                softening = 2.0f;
            } else if (softening > 100.0f) {
                std::cout << "Softening too large (max: 100.0). Using default: 2.0" << std::endl;
                softening = 2.0f;
            }
        } catch (const std::exception& e) {
            std::cout << "Invalid softening parameter. Using default: 2.0" << std::endl;
            softening = 2.0f;
        }
    }
   
    // Constants
    const unsigned int WINDOW_WIDTH = 1920;
    const unsigned int WINDOW_HEIGHT = 1080;
    const float G = 1.0f;
    
    // Simulation parameters
    bool showTrails = false;
    float zoomLevel = 1.0f;
    
    // Create window and view
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), 
                           "N-Body Simulation - " + implementation);
    window.setFramerateLimit(0);
    sf::View view = window.getDefaultView();
    
    // Initialize simulation
    Simulation simulation(G, softening, dt, WINDOW_WIDTH, WINDOW_HEIGHT);
    simulation.initializeRandomBodies(numBodies, 100.0f, 8000.0f);
    
    // Initialize managers
    UIManager uiManager(WINDOW_WIDTH, WINDOW_HEIGHT);
    uiManager.loadFont("/usr/share/fonts/TTF/JetBrainsMono-SemiBoldItalic.ttf");
    
    TrailManager trailManager(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    InputHandler inputHandler(showTrails, numBodies, dt, softening, G, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // Initialize simple benchmark
    Benchmark benchmark(implementation, numBodies);
    globalBenchmark = &benchmark;  // Set global pointer for signal handler
    
    // Set up signal handler for graceful shutdown
    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);
    
    std::cout << "Detected implementation: " << implementation << " (from binary: " << argv[0] << ")" << std::endl;
    std::cout << "Running simulation with " << numBodies << " bodies" << std::endl;
    std::cout << "Close the window or press Ctrl+C to save benchmark results." << std::endl;
    
    // Main loop
    while (window.isOpen() && !shouldExit) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (inputHandler.handleEvent(event, window, simulation, trailManager, 
                                       uiManager, view, zoomLevel)) {
                // Window is closing, save benchmark before exit
                benchmark.saveResults();
                return 0;
            }
        }
        
        // Check for signal-based exit
        if (shouldExit) {
            window.close();
            break;
        }
        
        // Update simulation and managers
        simulation.update();
        uiManager.updateFPS();
        trailManager.update(simulation.getBodies());
        
        // Record FPS for benchmark
        benchmark.addFrame(uiManager.getFPS());
        
        // Update UI texts
        uiManager.updateTexts(numBodies, dt, softening, trailManager.isEnabled(), uiManager.getFPS());
        
        // Render
        window.setView(view);
        
        if (trailManager.isEnabled()) {
            trailManager.draw(window);
        } else {
            window.clear(sf::Color::Black);
            // Draw bodies directly
            const auto& bodies = simulation.getBodies();
            for (const auto& body : bodies) {
                sf::CircleShape circle(body.getRadius());
                circle.setFillColor(body.getColor());
                circle.setOrigin(body.getRadius(), body.getRadius());
                circle.setPosition(body.getPosition());
                window.draw(circle);
            }
        }

        window.setView(window.getDefaultView());
        
        // Draw UI
        uiManager.draw(window);
        window.display();
    }
    
    return 0;
}
