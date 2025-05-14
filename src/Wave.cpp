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
      m_geneticManager(geneticManager),
      m_waveNumber(waveNumber),
      m_totalEnemies(config.maxEnemies),
      m_usedGenomes() // Inicializa el conjunto vacío
{
    int pointsToActivate = std::min(m_config.maxSpawnPoints, static_cast<int>(spawnPoints.size()));
    m_activeSpawnPoints.assign(spawnPoints.begin(), spawnPoints.begin() + pointsToActivate);

    std::cout << "\n=== OLEADA " << m_waveNumber << " ===\n";
    std::cout << "Max enemigos: " << m_totalEnemies
              << " | Spawn cada: " << m_config.spawnInterval << "s"
              << " | Puntos de spawn: " << m_activeSpawnPoints.size()
              << " | Genomas disponibles: " << m_geneticManager->getCurrentGenomes().size() << "\n";
}

void Wave::update(float deltaTime) {
    m_timeElapsed += deltaTime;
    m_spawnTimer += deltaTime;
}
std::vector<EnemyGenome::Ptr> Wave::getGenomesForNextSpawn() {
    std::vector<EnemyGenome::Ptr> genomes;

    if (m_enemiesSpawned < m_totalEnemies && m_spawnTimer >= m_config.spawnInterval) {
        m_spawnTimer = 0.f;
        int toSpawn = std::min(m_config.enemiesPerSpawn,
                             m_totalEnemies - m_enemiesSpawned);

        // Obtiene genomas unicos no usados aun
        auto unusedGenomes = getUnusedGenomes(toSpawn);
        genomes.insert(genomes.end(), unusedGenomes.begin(), unusedGenomes.end());

        m_enemiesSpawned += toSpawn;
    }

    return genomes;
}

std::vector<EnemyGenome::Ptr> Wave::getUnusedGenomes(int count) {
    std::vector<EnemyGenome::Ptr> result;

    if (count <= 0) return result;

    auto allGenomes = m_geneticManager->getCurrentGenomes();

    // Filtra genomas no usados
    std::vector<EnemyGenome::Ptr> availableGenomes;
    for (const auto& genome : allGenomes) {
        if (m_usedGenomes.find(genome->getId()) == m_usedGenomes.end()) {
            availableGenomes.push_back(genome);
        }
    }

    static std::random_device rd;
    static std::mt19937 g(rd());
    std::shuffle(availableGenomes.begin(), availableGenomes.end(), g);


    int actualCount = std::min(count, static_cast<int>(availableGenomes.size()));
    for (int i = 0; i < actualCount; ++i) {
        m_usedGenomes.insert(availableGenomes[i]->getId());
        result.push_back(availableGenomes[i]);
    }

    return result;
}

bool Wave::isCompleted() const {
    bool timeExpired = m_timeElapsed >= m_config.waveDuration;
    bool allEnemiesSpawned = m_enemiesSpawned >= m_totalEnemies;
    bool allGenomesUsed = m_usedGenomes.size() >= m_geneticManager->getCurrentGenomes().size();

    return timeExpired || (allEnemiesSpawned && allGenomesUsed);
}

