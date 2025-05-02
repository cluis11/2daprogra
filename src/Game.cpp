#include "Game.h"
#include <sstream>
#include <stdexcept>

Game::Game() 
    : m_window(sf::VideoMode( sf::Vector2u(800u, 800u) ), "Genetic Kingdom"),
      m_grid(50, 50, 16.f),
      m_font(),
      m_waveText(m_font, "Wave: 0/3",    20),
      m_timerText(m_font, "Time: 0s",     20),
      m_towersText(m_font, "Towers: 0/10",20),
      m_leaksText(m_font, "Leaks: 0/5",   20)
{
    
    m_window.setFramerateLimit(60);
    
    if (!m_font.openFromFile("assets/fonts/arial.ttf")) {
        throw std::runtime_error("Failed to load font file 'arial.ttf'");
    }
    
    // Setup UI texts
    m_waveText.setPosition(sf::Vector2f(10.f, 10.f));
    m_timerText.setPosition(sf::Vector2f(350.f, 10.f));
    m_towersText.setPosition(sf::Vector2f(650.f, 10.f));
    m_leaksText.setPosition(sf::Vector2f(10.f, 750.f));

    updateUI();
}

void Game::run() {
    sf::Clock clock;
    while (m_window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::processEvents() {
    // SFML 3’s pollEvent() returns a std::optional<sf::Event>
    while (const auto event = m_window.pollEvent()) {
        // Use the new is<T>() check instead of comparing .type
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }
        if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>();
            mb
         && m_currentState == GameState::Prep
         && m_towersPlaced  < 10)
        {
            // Convertir posición de mouse a grid
            auto gridPos = m_grid.worldToGrid(
                static_cast<float>(mb->position.x),
                static_cast<float>(mb->position.y)
            );

            if (m_grid.getCell(gridPos.x, gridPos.y) == CellType::Empty) {
                m_towers.emplace_back(
                    std::make_unique<Tower>(gridPos.x, gridPos.y, &m_grid)
                );
                m_grid.setCell(gridPos.x, gridPos.y, CellType::Tower);
                m_towersPlaced++;
                updateUI();
            }
        }
    }  
}


void Game::update(float deltaTime) {
    // Update game state
    m_stateTimer += deltaTime;

    // State transitions
    if (m_currentState == GameState::Prep && m_stateTimer >= 30.f) {
        m_currentState = GameState::Wave;
        m_stateTimer = 0.f;
    }
    else if (m_currentState == GameState::Wave) {
        if (m_stateTimer >= 60.f) {
            m_currentState = GameState::Cooldown;
            m_stateTimer = 0.f;
            m_waveNumber++;
        }
        
        // Spawn enemies every 10 seconds
        if (static_cast<int>(m_stateTimer) % 10 == 0) {
            spawnEnemy();
        }
    }
    else if (m_currentState == GameState::Cooldown && m_stateTimer >= 10.f) {
        if (m_waveNumber > 3) {
            // Game win logic
            m_window.close();
        }
        m_currentState = GameState::Prep;
        m_stateTimer = 0.f;
    }

    updateUI();
}

void Game::render() {
    m_window.clear();
    
    // Draw grid
    m_grid.render(m_window);
    
    // Draw entities
    for (const auto& tower : m_towers) tower->render(m_window);
    for (const auto& enemy : m_enemies) enemy->render(m_window);
    
    // Draw UI
    m_window.draw(m_waveText);
    m_window.draw(m_timerText);
    m_window.draw(m_towersText);
    m_window.draw(m_leaksText);
    
    m_window.display();
}

void Game::spawnEnemy() {
    auto entry = m_grid.getEntryPoint();
    m_enemies.emplace_back(
        std::make_unique<Enemy>(entry.x, entry.y, &m_grid)
    );
}

void Game::updateUI() {
    std::string stateStr;
    switch (m_currentState) {
        case GameState::Prep:     stateStr = "PREP";  break;
        case GameState::Wave:     stateStr = "WAVE";  break;
        case GameState::Cooldown: stateStr = "END";   break;
    }

    m_waveText.setString("Wave:  " + std::to_string(m_waveNumber)   + "/3");
    m_timerText.setString(stateStr + ": " + std::to_string(int(m_stateTimer)) + "s");
    m_towersText.setString("Towers: " + std::to_string(m_towersPlaced) + "/10");
    m_leaksText.setString("Leaks: "  + std::to_string(m_leaks)         + "/5");
}