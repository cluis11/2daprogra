#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "GridSystem.h"
#include "Tower.h"
#include "Enemy.h"
#include "Wave.h"
#include "Economy.h"
#include "PathFinding.h"

class Game {
public:
    Game(); //Constructor de SFML
    void run(); //Funcion que ejecuta el juego
    GridSystem& getGrid() { return m_grid; }
    Economy& getEconomy() { return m_economy; }

private:
    void processEvents(); //Captura eventos para ejecutar comportamientos
    void update(float deltaTime); //Ejecuta logica del objeto e interacciones
    void render(); //Encargado de dibujar los objetos
    void spawnEnemy(); //Funcion encargada de crear enemigos durante las waves
    void recalculatePaths(); //Recalcula los caminos
    void updateUI(); //Funcion encargada de actualizar la interfaz
    void updateTowerInfo();
    void restartGame();

    sf::RenderWindow m_window; //main screen
    GridSystem m_grid; //matriz
    PathFinding m_pathfinder; //pathfinding
    
    std::vector<std::unique_ptr<Tower>> m_towers; //lista de torres
    std::vector<std::unique_ptr<Enemy>> m_enemies; //lista de enemigos

    std::unique_ptr<Wave> m_currentWave;
    
    //Enum para patron state del comportamiento del juego
    //Se pueden agregar states como pantalla inicial, registro de actividades, pantalla de perdedor etc
    enum class GameState { Prep, Wave, Cooldown, EndScreen }; 

    //Se inicia el juego en estado prep, se puede cambiar a pantalla inicial
    GameState m_currentState = GameState::Prep;
    
    float m_stateTimer = 0.f; //se reinicia en cada state dek juego
    int m_waveNumber = 1; //variable que controla en numero de wave actual
    int m_towersPlaced = 0; //variable que controla numero de torres, se tiene que agregr logica de economia
    int m_leaks = 0; //controla cuantos enemigos que han llegado a la puerta para condicion game over
    
    //economia
    Economy m_economy;

    //Fuentes
    sf::Font m_font;
    //Textos 
    sf::Text m_waveText;
    sf::Text m_timerText;
    sf::Text m_towersText;
    sf::Text m_leaksText;
    sf::Text m_coinsText;

    //elementos de ui de seleccion de torres
    sf::RectangleShape m_uiPanel;
    std::vector<sf::RectangleShape> m_towerButtons;
    std::vector<sf::Text> m_towerButtonTexts;
    Tower::Type m_selectedTowerType;
    bool m_isPlacingTower;
    sf::RectangleShape m_towerGhost;
    //funciones seleccion de torres
    void initUI();
    void handleTowerSelection(const sf::Vector2i& mousePos);
    void handleTowerUpgrade();
    void updateTowerGhost(const sf::Vector2i& mousePos);

    sf::RectangleShape m_towerInfoPanel;
    sf::Text m_towerInfoText;
    sf::Text m_towerLevelText;
    sf::RectangleShape m_upgradeButton;
    sf::Text m_upgradeButtonText;
    Tower* m_selectedTower = nullptr;

    sf::RectangleShape m_restartButton;   
    sf::RectangleShape m_exitButton;
    sf::RectangleShape m_statsButton;

    sf::Text m_restartText;
    sf::Text m_exitText;
    sf::Text m_statsText;
    sf::Text m_titleEnd;
};