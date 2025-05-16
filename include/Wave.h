#pragma once
#include "Enemy.h"
#include "GridSystem.h"
#include <vector>
#include <memory>
#include "EnemyGenome.h"
#include  <unordered_set>

class GeneticManager;

class Wave {
public:

    //struct para las configuraciones del wave
    struct Config {
        int totalWaves = 3; //total de waves
        float waveDuration = 60.f; //duracion de cada una
        float spawnInterval = 5.0f; //intervalo spawn
        int maxSpawnPoints = 6; // Puntos de spawn, lista en game
        int maxEnemies = 20;
    };
    //constructor
    //Recibe el numero de wave, y los puntos de spawn definidos en game, la congif y referencia al grid
    Wave(int waveNumber, const std::vector<sf::Vector2i>& spawnPoints, Config& config, GridSystem* grid, GeneticManager* geneticManager);

    //funciones
    void update(float deltaTime, std::vector<std::unique_ptr<Enemy>>& enemies);
    bool isCompleted() const;
    int getEnemiesSpawned() const { return m_enemiesSpawned; }
    int getTotalEnemies() const { return m_totalEnemies; } //puede que no se ocupe
    int getMaxEnemies() const { return m_config.maxEnemies; }
    int getWaveNumber() const { return m_waveNumber; } 
    const Config& getWaveConfig() const { return m_config; }
    // Obtiene los genomas para el proximo spawn
    std::vector<EnemyGenome::Ptr> getGenomesForNextSpawn();
    // Obtiene los puntos de spawn activos
    const std::vector<sf::Vector2i>& getSpawnPoints() const { return m_spawnPoints; }

    void generateInitialEnemies();

private:
    void spawnEnemy(const EnemyGenome::Ptr& genome, std::vector<std::unique_ptr<Enemy>>& enemies, const sf::Vector2i& point);

    Config m_config;
    const std::vector<sf::Vector2i>& m_spawnPoints;
    std::vector<sf::Vector2i> m_activeSpawnPoints; //probable que se quite
    GridSystem* m_grid;
    GeneticManager* m_geneticManager;

    std::unordered_set<int> m_usedGenomes; //Registro de IDs usados
    std::vector<EnemyGenome::Ptr> getUnusedGenomes(int count); // Nuevo método privado

    int m_waveNumber = 1;
    float m_timeElapsed = 0.f;
    float m_spawnTimer = 0.f;
    int m_enemiesSpawned = 0;
    int m_activeSpawnPoints = 0;
    int m_enemiesSpawned = 0; // Contador de enemigos spawneados
    int m_totalEnemies = 0; // Total de enemigos a spawnear en esta oleada
};