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

int main(int argc, char* argv[]) {
    // Automatically detect implementation from binary name
    std::string implementation = detectImplementation(argv[0]);
    
    // Parse command line arguments
    int numBodies = 250;
    
    if (argc > 1) {
        try {
            numBodies = std::stoi(argv[1]);
            if (numBodies <= 1) {
                std::cout << "Number of bodies must be at least 2. Using default: 100" << std::endl;
                numBodies = 100;
            }
        } catch (const std::exception& e) {
            std::cout << "Invalid number of bodies. Using default: 100" << std::endl;
            numBodies = 100;
        }
    }
   
    // Constants
    const unsigned int WINDOW_WIDTH = 1920;
    const unsigned int WINDOW_HEIGHT = 1080;
    const float G = 1.0f;
    
    // Simulation parameters
    float softening = 2.0f;
    float dt = 0.001f;
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
