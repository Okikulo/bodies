#include "Extra.h"
#include "Simulation.h"
#include "Body.h"  // Add this include
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

// FPS implementation (existing)
void FPS::update() {
    if (mClock.getElapsedTime().asSeconds() >= 1.f) {
        mFps = mFrame;
        mFrame = 0;
        mClock.restart();
    }
    ++mFrame;
}

unsigned int FPS::getFPS() const {
    return mFps;
}

// UIManager implementation
UIManager::UIManager(unsigned int windowWidth, unsigned int windowHeight) : hideTui(false) {
    // Initialize text objects with positions
    bodyCountText.setCharacterSize(12);
    bodyCountText.setFillColor(sf::Color::White);
    bodyCountText.setPosition(10, 10);

    timeStepText.setCharacterSize(12);
    timeStepText.setFillColor(sf::Color::White);
    timeStepText.setPosition(10, 30);

    softeningText.setCharacterSize(12);
    softeningText.setFillColor(sf::Color::White);
    softeningText.setPosition(10, 50);

    trailText.setCharacterSize(12);
    trailText.setFillColor(sf::Color::White);
    trailText.setPosition(10, 70);

    controlsText.setCharacterSize(12);
    controlsText.setFillColor(sf::Color::White);
    controlsText.setPosition(10, windowHeight - 156);
    controlsText.setString("Space to hide interface\nR to reset with random bodies\nT to toggle trails\nF to increase time step\nS to decrease time step\n+ to add 100 more bodies\n- to remove 100 bodies\nH to increase softening\nK to decrease softening\nESC to exit");

    fpsText.setCharacterSize(12);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(windowWidth - 80, 10);
}

bool UIManager::loadFont(const std::string& fontPath) {
    if (!font.loadFromFile(fontPath)) {
        std::cout << "Warning: Could not load font. UI text won't be displayed." << std::endl;
        return false;
    }
    
    // Set font for all text objects
    bodyCountText.setFont(font);
    timeStepText.setFont(font);
    softeningText.setFont(font);
    trailText.setFont(font);
    controlsText.setFont(font);
    fpsText.setFont(font);
    return true;
}

void UIManager::updateTexts(int numBodies, float dt, float softening, bool showTrails, unsigned int currentFPS) {
    bodyCountText.setString("Bodies: " + std::to_string(numBodies));
    
    std::stringstream ts;
    ts << std::fixed << std::setprecision(6) << dt;
    timeStepText.setString("Time Step: " + ts.str() + "s");

    std::stringstream sf;
    sf << std::fixed << std::setprecision(2) << softening;
    softeningText.setString("Softening: " + sf.str());
    
    trailText.setString(showTrails ? "Trails: ON" : "");
    fpsText.setString("FPS: " + std::to_string(currentFPS));
}

void UIManager::draw(sf::RenderWindow& window) {
    if (!hideTui && font.getInfo().family != "") {
        window.draw(bodyCountText);
        window.draw(timeStepText);
        window.draw(softeningText);
        window.draw(trailText);
        window.draw(controlsText);
        window.draw(fpsText);
    }
}

// InputHandler implementation
InputHandler::InputHandler(bool& trails, int& bodies, float& timeStep, float& soft,
                          float gravConst, unsigned int winWidth, unsigned int winHeight)
    : showTrails(trails), numBodies(bodies), dt(timeStep), softening(soft),
      G(gravConst), windowWidth(winWidth), windowHeight(winHeight) {}

bool InputHandler::handleEvent(const sf::Event& event, sf::RenderWindow& window,
                              Simulation& simulation, TrailManager& trailManager,
                              UIManager& uiManager, sf::View& view, float& zoomLevel) {
    
    if (event.type == sf::Event::Closed) {
        window.close();
        return true;
    }
    
    if (event.type == sf::Event::KeyPressed) {
        auto resetSimulation = [&]() {
            simulation = Simulation(G, softening, dt, windowWidth, windowHeight);
            simulation.initializeRandomBodies(numBodies, 100.0f, 8000.0f);
            trailManager.clear();
        };
        
        switch (event.key.code) {
            case sf::Keyboard::Escape:
                window.close();
                return true;
                
            case sf::Keyboard::Space:
                uiManager.toggleUI();
                break;

            case sf::Keyboard::F:
                dt *= 10;
                if (dt >= 0.001f) softening += 1.0f;
                resetSimulation();
                break;

            case sf::Keyboard::S:
                dt /= 10;
                if (softening > 1.0f) softening -= 1.0f;
                resetSimulation();
                break;

            case sf::Keyboard::H:
                softening += 1.0f;
                resetSimulation();
                break;

            case sf::Keyboard::K:
                if (softening > 1.0f) softening -= 1.0f;
                resetSimulation();
                break;

            case sf::Keyboard::R:
                resetSimulation();
                break;

            case sf::Keyboard::Add:
            case sf::Keyboard::Equal:
                numBodies += 100;
                resetSimulation();
                break;
                
            case sf::Keyboard::Subtract:
            case sf::Keyboard::Dash:
                numBodies = std::max(11, numBodies - 100);
                resetSimulation();
                break;
                
            case sf::Keyboard::T:
                trailManager.toggle();
                showTrails = trailManager.isEnabled();
                break;
        }
    }
    else if (event.type == sf::Event::MouseWheelScrolled) {
        // Handle zoom
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f beforeCoord = window.mapPixelToCoords(pixelPos);

        float zoomChange = (event.mouseWheelScroll.delta > 0) ? 0.9f : 1.1f;
        float proposedZoomLevel = zoomLevel * zoomChange;
        float clampedZoomLevel = std::clamp(proposedZoomLevel, 0.2f, 1.0f);
        float zoomFactor = clampedZoomLevel / zoomLevel;
        
        view.zoom(zoomFactor);
        zoomLevel = clampedZoomLevel;

        sf::Vector2f afterCoord = window.mapPixelToCoords(pixelPos);
        sf::Vector2f offset = beforeCoord - afterCoord;
        view.move(offset);
    }
    
    return false;
}

// TrailManager implementation
TrailManager::TrailManager(unsigned int windowWidth, unsigned int windowHeight) : showTrails(false) {
    trailTexture.create(windowWidth, windowHeight);
    trailTexture.clear(sf::Color::Black);
    trailTexture.display();
    trailSprite.setTexture(trailTexture.getTexture());
}

void TrailManager::clear() {
    trailTexture.clear(sf::Color::Black);
    trailTexture.display();
}

void TrailManager::update(const std::vector<Body>& bodies) {
    if (!showTrails) return;
    
    // Draw faded version of previous frame
    sf::RectangleShape fadeRect(sf::Vector2f(trailTexture.getSize().x, trailTexture.getSize().y));
    fadeRect.setFillColor(sf::Color(10, 10, 40, 10));
    trailTexture.draw(fadeRect);
    
    // Draw all bodies to trail texture
    for (const auto& body : bodies) {
        sf::CircleShape circle(body.getRadius());
        circle.setFillColor(body.getColor());
        circle.setOrigin(body.getRadius(), body.getRadius());
        circle.setPosition(body.getPosition());
        trailTexture.draw(circle);
    }
    
    trailTexture.display();
}

void TrailManager::draw(sf::RenderWindow& window) {
    if (showTrails) {
        window.draw(trailSprite);
    }
}

void TrailManager::toggle() {
    showTrails = !showTrails;
    if (!showTrails) clear();
}
