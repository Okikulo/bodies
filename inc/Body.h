#pragma once
#ifndef BODY_H
#define BODY_H

#include <SFML/Graphics.hpp>

class Body {
private:
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    float mass;
    float radius;
    sf::Color color;

public:
    // Constructor
    Body(sf::Vector2f pos, sf::Vector2f vel, float m, float r, sf::Color c);

    // Getters
    sf::Vector2f getPosition() const;
    sf::Vector2f getVelocity() const;
    float getMass() const;
    float getRadius() const;
    sf::Color getColor() const;

    // Apply force to calculate new acceleration
    void applyForce(const sf::Vector2f& force);

    // Update position and velocity using current acceleration
    void update(float dt);

    // Reset acceleration to zero (called at beginning of each frame)
    void resetAcceleration();
};

#endif // BODY_H
