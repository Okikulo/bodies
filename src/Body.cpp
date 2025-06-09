#include "Body.h"
#include <omp.h>

Body::Body(sf::Vector2f pos, sf::Vector2f vel, float m, float r, sf::Color c)
    : position(pos), velocity(vel), acceleration(sf::Vector2f(0.0f, 0.0f)), mass(m), radius(r), color(c) {}

sf::Vector2f Body::getPosition() const {
    return position;
}

sf::Vector2f Body::getVelocity() const {
    return velocity;
}

float Body::getMass() const {
    return mass;
}

float Body::getRadius() const {
    return radius;
}

sf::Color Body::getColor() const {
    return color;
}

void Body::applyForce(const sf::Vector2f& force) {
    // F = ma => a = F/m
    sf::Vector2f a;
    a.x = force.x / mass;
    a.y = force.y / mass;
    
    acceleration.x += a.x;
    acceleration.y += a.y;
}

void Body::update(float dt) {
    // Update velocity based on acceleration
    velocity.x += acceleration.x * dt;
    velocity.y += acceleration.y * dt;
    
    // Update position based on velocity
    position.x += velocity.x * dt;
    position.y += velocity.y * dt;
}

void Body::resetAcceleration() {
    acceleration.x = 0;
    acceleration.y = 0;
}
