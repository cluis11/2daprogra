#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "GridSystem.h"
#include "Tower.h"
#include "Enemy.h"

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float deltaTime);
    void render();
    void spawnEnemy();
    void updateUI();
    
    sf::RenderWindow m_window;
    GridSystem m_grid;
    
    std::vector<std::unique_ptr<Tower>> m_towers;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    
    enum class GameState { Prep, Wave, Cooldown };
    GameState m_currentState = GameState::Prep;
    
    float m_stateTimer = 0.f;
    int m_waveNumber = 1;
    int m_towersPlaced = 0;
    int m_leaks = 0;
    
    sf::Font m_font;
    sf::Text m_waveText;
    sf::Text m_timerText;
    sf::Text m_towersText;
    sf::Text m_leaksText;
};