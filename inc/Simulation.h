#pragma once
#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include "Body.h"
#include <omp.h>
#include "Extra.h"

class Simulation {
private:
    std::vector<Body> bodies;
    float gravitationalConstant;
    float softening; // To prevent division by zero in force calculation
    float timeStep;
    float width;
    float height;

public:
    // Constructor
    Simulation(float g, float soften, float dt, float w, float h);

    // Initialize with random bodies
    void initializeRandomBodies(int n, float maxMassSmall, float MaxMassBig);

    // Calculate forces between all bodies and update their positions
    void update();

    // Getter for bodies vector
    const std::vector<Body>& getBodies() const;
};

#endif // SIMULATION_H
