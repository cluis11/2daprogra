#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "GridSystem.h"
#include "Tower.h"
#include "Enemy.h"
#include "GeneticManager.h"
#include "Wave.h"

class Game {
public:
    Game(); //Constructor de SFML
    void run(); //Funcion que ejecuta el juego
    GridSystem& getGrid() { return m_grid; }

private:
    // Metodos principales del juego
    void processEvents(); //Captura eventos para ejecutar comportamientos
    void update(float deltaTime); //Ejecuta logica del objeto e interacciones
    void render(); //Encargado de dibujar los objetos
    void updateUI(); //Funcion encargada de actualizar la interfaz


    // Metodos de manejo de oleadas
    void startNewWave();
    void updateWaveState(float deltaTime);
    void spawnWaveEnemies();
    void endCurrentWave();
    void handleCooldownEnd();

    //Metodos de manejo de enemigos
    void spawnEnemy(const EnemyGenome& genome);
    void updateEnemies(float deltaTime);
    void logEnemyDeath(const std::unique_ptr<Enemy>& enemy);

    // Objetos del juego
    sf::RenderWindow m_window; //main screen
    GridSystem m_grid; //matriz
    std::vector<std::unique_ptr<Tower>> m_towers; //lista de torres
    std::vector<std::unique_ptr<Enemy>> m_enemies; //lista de enemigos
    std::unique_ptr<Wave> m_currentWave;
    GeneticManager m_geneticManager;

    // Estado del juego
    //Enum para patron state del comportamiento del juego
    //Se pueden agregar states como pantalla inicial, registro de actividades, pantalla de perdedor etc
    enum class GameState { Prep, Wave, Cooldown };
    GameState m_currentState = GameState::Prep;    //Se inicia el juego en estado prep, se puede cambiar a pantalla inicial



    //Temporizadores y contadores
    float m_stateTimer = 0.f; //se reinicia en cada state dek juego
    float m_spawnAccumulator = 0.f; // Nuevo miembro para controlar spawn
    int m_waveNumber = 1; //variable que controla en numero de wave actual
    int m_towersPlaced = 0; //variable que controla numero de torres, se tiene que agregr logica de economia
    int m_leaks = 0; //controla cuantos enemigos que han llegado a la puerta para condicion game over
    int m_totalEnemiesDefeated = 0;


    //UI
    //Fuentes
    sf::Font m_font;
    //Textos 
    sf::Text m_waveText;
    sf::Text m_timerText;
    sf::Text m_towersText;
    sf::Text m_leaksText;

    //solo por ahora, variable que rota el tipo de torre que coloca
    Tower::Type m_nextTowerType = Tower::Type::Archer;
};