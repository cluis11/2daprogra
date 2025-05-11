#include "Wave.h"
#include "GeneticManager.h"  // Añadir este include
#include <random>
#include <iostream>

Wave::Wave(int waveNumber,
           const std::vector<sf::Vector2i>& spawnPoints,
           const Config& config,
           GridSystem* grid,
           GeneticManager* geneticManager)
    : m_config(config),
      m_spawnPoints(spawnPoints),
      m_grid(grid),
      m_geneticManager(geneticManager),  // Inicializar geneticManager
      m_waveNumber(waveNumber)
{
    int pointsToActivate = std::min(config.maxSpawnPoints, static_cast<int>(spawnPoints.size()));
    m_activeSpawnPoints.assign(spawnPoints.begin(), spawnPoints.begin() + pointsToActivate);
    m_totalEnemies = config.maxEnemies;

    std::cout << "\n=== OLEADA " << m_waveNumber << " ===\n";
    std::cout << "Max enemigos: " << m_totalEnemies
              << " | Spawn cada: " << m_config.spawnInterval << "s"
              << " | Puntos de spawn: " << m_activeSpawnPoints.size() << "\n";
}

void Wave::update(float deltaTime) {
    m_timeElapsed += deltaTime;
    m_spawnTimer += deltaTime;
}

std::vector<EnemyGenome::Ptr> Wave::getGenomesForNextSpawn() {
    std::vector<EnemyGenome::Ptr> genomesToSpawn;

    // No spawnear si la oleada ha terminado o ya se generaron todos los enemigos
    if (m_timeElapsed > m_config.waveDuration || m_enemiesSpawned >= m_totalEnemies) {
        return genomesToSpawn;
    }
    // Verificar si debe generar un nuevo enemigo
    if (m_spawnTimer >= m_config.spawnInterval) {
        // Rota entre tipos de enemigos
        EnemyType type = static_cast<EnemyType>(m_enemiesSpawned % 4);

        // Genera nuevo genoma usando puntero compartido
        auto newGenome = m_geneticManager->generateEnemyGenome(type);
        genomesToSpawn.push_back(newGenome);

        m_enemiesSpawned++;
        m_spawnTimer = 0.f; // Reinicia temporizador de spawn
    }

    return genomesToSpawn;
}

bool Wave::isCompleted() const {
    return m_timeElapsed >= m_config.waveDuration || m_enemiesSpawned >= m_totalEnemies;
}