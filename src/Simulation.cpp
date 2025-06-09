#include "Simulation.h"
#include <random>
#include <cmath>

Simulation::Simulation(float g, float soften, float dt, float w, float h)
    : gravitationalConstant(g), softening(soften), timeStep(dt), width(w), height(h) {}

void Simulation::initializeRandomBodies(int n, float maxMassSmall, float MaxMassBig) {
    bodies.clear();

    float massCentral = 50000.0f;
        
    // Add massive central body
    bodies.emplace_back(
        sf::Vector2f(width/2, height/2),       // Position at center
        sf::Vector2f(0, 0),                    // No initial velocity
        massCentral,                              // Very large mass
        10.0f,                                 // Larger radius
        sf::Color(255, 255, 255)                // Bright yellow/gold
    );

   
    // Random number generation
    std::random_device rd;
    std::mt19937 gen(rd());

    // Generate orbiting bodies (big)
    for (int i = 0; i <  n / 50; i++) {
        // Generate random angle and distance from center
        std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * M_PI);
        std::uniform_real_distribution<float> distDist(250.0f, std::min(width, height)/2 - 50.0f);
        std::uniform_real_distribution<float> massDist(1000.0f, MaxMassBig);
        std::uniform_real_distribution<float> radiusDist(4.0f, 9.0f);
        std::uniform_real_distribution<float> orbitTypeDist(0.7f, 1.0f);
        
        float angle = angleDist(gen);
        float distance = distDist(gen);
        float orbitFactor = orbitTypeDist(gen);
        
        // Calculate position based on angle and distance
        sf::Vector2f pos(
            width/2 + distance * cos(angle),
            height/2 + distance * sin(angle)
        );
        
        // Calculate orbital velocity for a circular orbit
        float baseOrbitalSpeed = sqrt(gravitationalConstant * massCentral / distance);
        
        float orbitalSpeed = baseOrbitalSpeed * orbitFactor;
        
        // Velocity perpendicular to radius vector (for circular orbit)
        sf::Vector2f vel(
            -orbitalSpeed * sin(angle),
            orbitalSpeed * cos(angle)
        );
        
        float mass = massDist(gen);
        float radius = radiusDist(gen);
        sf::Color color(255, 255, 255);
        
        bodies.emplace_back(pos, vel, mass, radius, color);
    }

    // Generate orbiting bodies (small)
    for (int i = 0; i < n-1; i++) {
        // Generate random angle and distance from center
        std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * M_PI);
        std::uniform_real_distribution<float> distDist(50.0f, std::min(width, height)/2 - 50.0f);
        std::uniform_real_distribution<float> massDist(20.0f, maxMassSmall);
        std::uniform_real_distribution<float> radiusDist(0.5f, 1.5f);
        std::uniform_int_distribution<int> colorDist(50, 255);
        std::uniform_real_distribution<float> orbitTypeDist(0.7f, 1.0f);
        
        float angle = angleDist(gen);
        float distance = distDist(gen);
        float orbitFactor = orbitTypeDist(gen);
        
        // Calculate position based on angle and distance
        sf::Vector2f pos(
            width/2 + distance * cos(angle),
            height/2 + distance * sin(angle)
        );
        
        // Calculate orbital velocity for a circular orbit
        // v = sqrt(G * M / r) perpendicular to radius vector
        float baseOrbitalSpeed = sqrt(gravitationalConstant * massCentral / distance);
        
        float orbitalSpeed = baseOrbitalSpeed * orbitFactor;

        // Velocity perpendicular to radius vector (for circular orbit)
        sf::Vector2f vel(
            -orbitalSpeed * sin(angle),
            orbitalSpeed * cos(angle)
        );
        
        float mass = massDist(gen);
        float radius = radiusDist(gen);
        sf::Color color(colorDist(gen), colorDist(gen), colorDist(gen));
        sf::Color noColor(255, 255 ,255);
        
        bodies.emplace_back(pos, vel, mass, radius, color);
    }
}

void Simulation::update() {
    // Reset all accelerations
    for (auto& body : bodies) {
        body.resetAcceleration();
    }
    
    // Calculate forces between all pairs of bodies
    for (size_t i = 0; i < bodies.size(); i++) {
        for (size_t j = i + 1; j < bodies.size(); j++) {
            sf::Vector2f pos_i = bodies[i].getPosition();
            sf::Vector2f pos_j = bodies[j].getPosition();
            
            // Calculate distance vector
            sf::Vector2f delta(pos_j.x - pos_i.x, pos_j.y - pos_i.y);
            
            // Calculate squared distance
            float distSquared = delta.x * delta.x + delta.y * delta.y;
            
            // Add softening to prevent division by zero
            distSquared += softening * softening;
            
            // Calculate gravitational force magnitude
            // F = G * m1 * m2 / r^2
            float forceMagnitude = gravitationalConstant * bodies[i].getMass() * bodies[j].getMass() / distSquared;
            
            // Calculate unit direction vector
            float distance = sqrt(distSquared);
            sf::Vector2f direction(delta.x / distance, delta.y / distance);
            
            // Calculate force vector
            sf::Vector2f force(direction.x * forceMagnitude, direction.y * forceMagnitude);
            
            // Apply forces (Newton's third law: equal and opposite)
            bodies[i].applyForce(force);
            bodies[j].applyForce(sf::Vector2f(-force.x, -force.y));
        }
    }
    
    // Update positions and velocities
    for (auto& body : bodies) {
        body.update(timeStep);
    }
}

const std::vector<Body>& Simulation::getBodies() const {
    return bodies;
}
