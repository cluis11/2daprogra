#include "Wave.h"
#include <iostream>

// En Wave.cpp
Wave::Wave(int waveNumber, const std::vector<sf::Vector2i>& spawnPoints,
         const Config& config, GridSystem* grid,
         const std::vector<EnemyGenome>& genomes)
    : m_config(config), m_spawnPoints(spawnPoints), m_grid(grid),
      m_genomes(genomes), m_waveNumber(waveNumber)
{
    // Activa puntos de spawn
    int pointsToActivate = std::min(config.maxSpawnPoints, static_cast<int>(spawnPoints.size()));
    m_activeSpawnPoints.assign(spawnPoints.begin(), spawnPoints.begin() + pointsToActivate);

    m_totalEnemies = config.maxEnemies;
}

void Wave::update(float deltaTime) {
    m_timeElapsed += deltaTime;
    m_spawnTimer += deltaTime;
}


std::vector<EnemyGenome> Wave::getGenomesForNextSpawn() {
    std::vector<EnemyGenome> genomesToSpawn;

    if (m_genomes.empty() ||
        m_timeElapsed > m_config.waveDuration ||
        m_enemiesSpawned >= m_totalEnemies) {
        return genomesToSpawn;
        }

    if (m_spawnTimer >= m_config.spawnInterval) {
        // Selecciona punto de spawn rotativo
        int spawnIndex = m_enemiesSpawned % m_activeSpawnPoints.size();
        const auto& spawnPos = m_activeSpawnPoints[spawnIndex];

        if (m_grid->getCell(spawnPos.x, spawnPos.y) == CellType::Empty) {
            // Selecciona genoma aleatorio
            std::uniform_int_distribution<size_t> dist(0, m_genomes.size() - 1);
            genomesToSpawn.push_back(m_genomes[dist(m_grid->getRNG())]);
            m_enemiesSpawned++;

            std::cout << "[Wave " << m_waveNumber << "] Spawned enemy "
                      << m_enemiesSpawned << "/" << m_totalEnemies
                      << " at (" << spawnPos.x << "," << spawnPos.y << ")\n";
        }

        m_spawnTimer = 0.f;
    }

    return genomesToSpawn;
}


bool Wave::isCompleted() const {
    return m_timeElapsed >= m_config.waveDuration || m_enemiesSpawned >= m_totalEnemies;
}