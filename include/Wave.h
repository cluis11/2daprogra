#pragma once
#include "Enemy.h"
#include "GridSystem.h"
#include <vector>
#include <memory>
#include "EnemyGenome.h"

// Forward declarations
class Game;
class GridSystem;
class GeneticManager;

class Wave {
public:
    //struct para las configuraciones del wave
    struct Config {
        int totalWaves = 3; //total de waves
        float waveDuration = 60.f; // Duracion maxima de la oleada en segundos
        float spawnInterval = 5.0f; //intervalo de spawn de enemigos
        int maxSpawnPoints = 6; // Puntos de spawn, lista en game
        int enemiesPerSpawn = 1; // Enemigos a spawnear por interval
        int maxEnemies = 20; //Maxima cantidad de enemigos que aparecen en la primer oleada
    };

    //Recibe el numero de wave, y los puntos de spawn definidos en game, la config, referencia al grid y el genetic manager
    Wave(int waveNumber,
         const std::vector<sf::Vector2i>& spawnPoints,
         const Config& config,
         GridSystem* grid,
         GeneticManager* geneticManager);

    //funciones
    void update(float deltaTime); // Actualiza el estado de la oleada
    bool isCompleted() const; // Verifica si la oleada ha terminado
    int getEnemiesSpawned() const { return m_enemiesSpawned; }
    int getTotalEnemies() const { return m_totalEnemies; }
    int getMaxEnemies() const { return m_config.maxEnemies; }

    // Obtiene los genomas para el proximo spawn
    std::vector<EnemyGenome::Ptr> getGenomesForNextSpawn();
    // Obtiene los puntos de spawn activos
    const std::vector<sf::Vector2i>& getSpawnPoints() const { return m_spawnPoints; }


private:
    const Config m_config;
    std::vector<sf::Vector2i> m_spawnPoints;
    std::vector<sf::Vector2i> m_activeSpawnPoints;
    GridSystem* m_grid;
    GeneticManager* m_geneticManager;

    int m_waveNumber;
    float m_timeElapsed = 0.f; // Tiempo transcurrido en la oleada
    float m_spawnTimer = 0.f; // Temporizador para controlar spawns
    int m_enemiesSpawned = 0; // Contador de enemigos spawneados
    int m_totalEnemies = 0; // Total de enemigos a spawnear en esta oleada
};