#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

// Forward declarations
class Body;
class Simulation;

class FPS {
private:
    unsigned int mFrame;
    unsigned int mFps;
    sf::Clock mClock;
public:
    FPS() : mFrame(0), mFps(0) {}
    void update(); 
    unsigned int getFPS() const;
};

class UIManager {
private:
    sf::Font font;
    sf::Text bodyCountText;
    sf::Text timeStepText;
    sf::Text softeningText;
    sf::Text trailText;
    sf::Text controlsText;
    sf::Text fpsText;
    bool hideTui;
    FPS fps;

public:
    UIManager(unsigned int windowWidth, unsigned int windowHeight);
    bool loadFont(const std::string& fontPath);
    void updateTexts(int numBodies, float dt, float softening, bool showTrails, unsigned int currentFPS);
    void draw(sf::RenderWindow& window);
    void toggleUI() { hideTui = !hideTui; }
    bool isUIHidden() const { return hideTui; }
    void updateFPS() { fps.update(); }
    unsigned int getFPS() const { return fps.getFPS(); }
};

class TrailManager {
private:
    sf::RenderTexture trailTexture;
    sf::Sprite trailSprite;
    bool showTrails;

public:
    TrailManager(unsigned int windowWidth, unsigned int windowHeight);
    void clear();
    void update(const std::vector<Body>& bodies);
    void draw(sf::RenderWindow& window);
    void toggle();
    bool isEnabled() const { return showTrails; }
    sf::RenderTexture& getTrailTexture() { return trailTexture; }
};

class InputHandler {
private:
    bool& showTrails;
    int& numBodies;
    float& dt;
    float& softening;
    const float G;
    const unsigned int windowWidth, windowHeight;

public:
    InputHandler(bool& trails, int& bodies, float& timeStep, float& soft, 
                float gravConst, unsigned int winWidth, unsigned int winHeight);
    
    bool handleEvent(const sf::Event& event, sf::RenderWindow& window, 
                    Simulation& simulation, TrailManager& trailManager,
                    UIManager& uiManager, sf::View& view, float& zoomLevel);
};

