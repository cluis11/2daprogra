#pragma once
#include "Enemy.h"
#include "GridSystem.h"
#include <vector>
#include <memory>
#include "EnemyGenome.h"

class Game;
class GridSystem;

class Wave {
public:
    //struct para las configuraciones del wave
    struct Config {
        int totalWaves = 3; //total de waves
        float waveDuration = 60.f; //duracion de cada una
        float spawnInterval = 10.f; //intervalo spawn
        int maxSpawnPoints = 6; // Puntos de spawn, lista en game
    };

    //constructor
    //Recibe el numero de wave, y los puntos de spawn definidos en game, la congif y referencia al grid
    Wave(int waveNumber, const std::vector<sf::Vector2i>& spawnPoints,
         const Config& config, GridSystem* grid,
         const std::vector<EnemyGenome>& genomes);
    //funciones
    void update(float deltaTime);
    bool isCompleted() const;
    int getEnemiesSpawned() const { return m_enemiesSpawned; }
    int getTotalEnemies() const { return m_totalEnemies; }

    // Nueva funcion para obtener genomas a spawnear
    std::vector<EnemyGenome> getGenomesForNextSpawn();

private:
    const Config m_config;
    const std::vector<sf::Vector2i>& m_spawnPoints;
    GridSystem* m_grid;
    const std::vector<EnemyGenome>& m_genomes;

    int m_waveNumber;
    float m_timeElapsed = 0.f;
    float m_spawnTimer = 0.f;
    int m_enemiesSpawned = 0;
    int m_totalEnemies = 0;
    int m_activeSpawnPoints = 0;

};