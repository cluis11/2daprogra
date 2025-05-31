#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "GridSystem.h"
#include "Tower.h"
#include "Enemy.h"
#include "Wave.h"
#include "Economy.h"
#include "GeneticManager.h"
#include "PathFinding.h"
#include "ProjectileEffect.h"
#include "GameStats.h"
#include "AreaAttackEffect.h"

class Game {
public:
    Game(); //Constructor de SFML
    void run(); //Funcion que ejecuta el juego
    GridSystem& getGrid() { return m_grid; }
    Economy& getEconomy() { return m_economy; }
    GameStats& getGameStats() { return m_gameStats; }
    void showStats(); // Metodo para mostrar estadísticas
    void renderStats(); // Metodo para renderizar estadísticas
    sf::Color getWaveColor(int waveNumber) const;
    sf::Color getFitnessColor(float fitness) const;

private:
    void processEvents(); //Captura eventos para ejecutar comportamientos
    void update(float deltaTime); //Ejecuta logica del objeto e interacciones
    void render(); //Encargado de dibujar los objetos
    void recalculatePaths(); //Recalcula los caminos
    void updateUI(); //Funcion encargada de actualizar la interfaz
    void updateTowerInfo();
    void restartGame();
    std::vector<AreaAttackEffect> m_areaEffects;
    void addAreaEffect(const sf::Vector2f& center, float radius, sf::Color color, float duration = 0.5f);
    void updateAreaEffects(float deltaTime);
    void renderAreaEffects(sf::RenderTarget& target);

    void logEnemyDeath(const std::unique_ptr<Enemy>& enemy);




    sf::RenderWindow m_window; //main screen
    GridSystem m_grid; //matriz
    PathFinding m_pathfinder; //pathfinding
    GeneticManager m_geneticManager;
    GameStats m_gameStats;

    std::vector<std::unique_ptr<Tower>> m_towers; //lista de torres
    std::vector<std::unique_ptr<Enemy>> m_enemies; //lista de enemigos

    std::unique_ptr<Wave> m_currentWave;

    std::vector<ProjectileEffect> m_projectileEffects;  // Nuevo miembro
    void addProjectileEffect(const sf::Vector2f& start, const sf::Vector2f& end, sf::Color color) {
        // Valores por defecto para velocidad y radio
        m_projectileEffects.emplace_back(start, end, color, 400.0f, 4.0f);
    }

    //Enum para patron state del comportamiento del juego
    //Se pueden agregar states como pantalla inicial, registro de actividades, pantalla de perdedor etc
    enum class GameState { Prep, Wave, Cooldown, EndScreen, Victory, StatsScreen };

    //Se inicia el juego en estado prep, se puede cambiar a pantalla inicial
    GameState m_currentState = GameState::Prep;
    
    float m_stateTimer = 0.f; //se reinicia en cada state dek juego
    int m_waveNumber = 1; //variable que controla en numero de wave actual
    int m_towersPlaced = 0; //variable que controla numero de torres, se tiene que agregr logica de economia
    int m_leaks = 0; //controla cuantos enemigos que han llegado a la puerta para condicion game over

    float m_spawnAccumulator = 0.f; // Nuevo miembro para controlar spawn
    int m_totalEnemiesDefeated = 0;
    
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

    // Textos para estadísticas
    std::vector<sf::Text> m_statsTexts;
    sf::Text m_statsTitle;
    sf::RectangleShape m_statsBackground;
};