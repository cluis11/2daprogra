#include "Wave.h"
#include "GeneticManager.h" 
#include <iostream>
#include <random>

//constructor
Wave::Wave(int waveNumber, const std::vector<sf::Vector2i>& spawnPoints, const Config& config, GridSystem* grid, GeneticManager* geneticManager)
    : m_waveNumber(waveNumber), m_spawnPoints(spawnPoints), m_config(config), m_grid(grid), m_geneticManager(geneticManager) {
    
    // Calcula spawn points activos (mínimo 1, máximo config.maxSpawnPoints)
    m_activeSpawnPoints = std::min(
        std::max(1, 3 + (waveNumber - 1)), // Ejemplo: 3, 4, 5, 6...
        std::min(m_config.maxSpawnPoints, static_cast<int>(m_spawnPoints.size()))
    );

    std::cout << "\n=== OLEADA " << m_waveNumber << " ===\n";
    std::cout << "Max enemigos: " << m_totalEnemies
              << " | Spawn cada: " << m_config.spawnInterval << "s"
              << " | Puntos de spawn: " << m_activeSpawnPoints
              << " | Genomas disponibles: " << m_geneticManager->getCurrentGenomes().size() << "\n";
}

void Wave::update(float deltaTime, std::vector<std::unique_ptr<Enemy>>& enemies) {
    m_timeElapsed += deltaTime;
    m_spawnTimer += deltaTime;

    if (m_spawnTimer >= m_config.spawnInterval && m_timeElapsed <= m_config.waveDuration) {
        spawnEnemy(enemies);
        m_spawnTimer = 0.f;
    }
}

std::vector<EnemyGenome::Ptr> Wave::getGenomesForNextSpawn() {
    std::vector<EnemyGenome::Ptr> genomes;

    if (m_enemiesSpawned < m_totalEnemies && m_spawnTimer >= m_config.spawnInterval) {
        m_spawnTimer = 0.f;
        int toSpawn = std::min(m_activeSpawnPoints,
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

Enemy::Type Wave::getEnemyType() const {
    std::uniform_int_distribution<int> dist(0, 99);
    int roll = dist(m_grid->getRNG());

    if (m_waveNumber >= 3 && roll < 15) return Enemy::Type::Mercenary;
    else if (m_waveNumber >= 2 && roll < 30) return Enemy::Type::Harpy;
    else if (roll < 40) return Enemy::Type::Ogre;
    else return Enemy::Type::DarkElf;
}

void Wave::spawnEnemy(std::vector<std::unique_ptr<Enemy>>& enemies) {
    for (int i = 0; i < m_activeSpawnPoints && m_enemiesSpawned < m_totalEnemies; ++i) {
        const auto& point = m_spawnPoints[i % m_spawnPoints.size()];
        if (m_grid->getCell(point.x, point.y) == CellType::Empty) {
            std::cout << "Se creo el enemigo y esta es la posicion: ( " << point.x << ", " << point.y << " )" << std::endl;
                auto it = m_grid->m_precomputedPaths.find(point);
                if (it != m_grid->m_precomputedPaths.end()) {
                    std::cout << "si se encontro el camino" << std::endl;
                    enemies.emplace_back(std::make_unique<Enemy>(
                        getEnemyType(),
                        point.x,
                        point.y,
                        m_grid,
                        it->second
                    ));
                    m_enemiesSpawned++;
            } else { continue; }
        }
    }
}

//revisar bien esta logica
bool Wave::isCompleted() const {
    bool timeExpired = m_timeElapsed >= m_config.waveDuration;
    bool allEnemiesSpawned = m_enemiesSpawned >= m_totalEnemies;
    bool allGenomesUsed = m_usedGenomes.size() >= m_geneticManager->getCurrentGenomes().size();

    return timeExpired || (allEnemiesSpawned && allGenomesUsed);
}