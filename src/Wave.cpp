#include "Wave.h"
#include "GeneticManager.h" 
#include <iostream>
#include <random>

//constructor
Wave::Wave(int waveNumber, const std::vector<sf::Vector2i>& spawnPoints, Config& config, GridSystem* grid, GeneticManager* geneticManager)
    : m_waveNumber(waveNumber), m_spawnPoints(spawnPoints), m_config(config), m_grid(grid), m_geneticManager(geneticManager) {
    
    // Calcula spawn points activos (mínimo 1, máximo config.maxSpawnPoints)
    m_activeSpawnPoints = std::min(
        std::max(1, 3 + (waveNumber - 1)), // Ejemplo: 3, 4, 5, 6...
        std::min(m_config.maxSpawnPoints, static_cast<int>(m_spawnPoints.size()))
    );

    m_config.maxEnemies = 20*waveNumber;

    if (m_waveNumber==1){ generateInitialEnemies(); }

    std::cout << "\n=== OLEADA " << m_waveNumber << " ===\n";
    std::cout << "Max enemigos: " << m_totalEnemies
              << " | Spawn cada: " << m_config.spawnInterval << "s"
              << " | Puntos de spawn: " << m_activeSpawnPoints
              << " | Genomas disponibles: " << m_geneticManager->getCurrentGenomes().size() << "\n";
}

void Wave::update(float deltaTime, std::vector<std::unique_ptr<Enemy>>& enemies) {
    m_timeElapsed += deltaTime;
    m_spawnTimer += deltaTime;

    auto genomes = getGenomesForNextSpawn(); // Ahora devuelve Ptr
    int i=0;
    for (const auto& genome : genomes) {
        const auto& point = m_spawnPoints[i % m_spawnPoints.size()];
        spawnEnemy(genome, enemies, point); // genome ya es Ptr
        i++;
    }
}

void Wave::generateInitialEnemies() {
    // Verifica si ya hay genomas generados
    if (m_geneticManager->getCurrentGenomes().empty()) {
        std::cout << "Generando población inicial de " << m_config.maxEnemies << " enemigos\n";

        // 1. Crea 2 de cada tipo (8 enemigos base)
        for (int type = 0; type < 4; ++type) {
            m_geneticManager->generateEnemyGenome(static_cast<EnemyType>(type));
            m_geneticManager->generateEnemyGenome(static_cast<EnemyType>(type));
        }

        // 2. Crea el resto de forma aleatoria
        for (int i = 8; i < m_config.maxEnemies; ++i) {
            EnemyType randomType = static_cast<EnemyType>(rand() % 4);
            m_geneticManager->generateEnemyGenome(randomType);
        }
    } else {
        std::cout << "Población existente detectada ("
                 << m_geneticManager->getCurrentGenomes().size()
                 << " genomas). No se generan nuevos.\n";
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


void Wave::spawnEnemy(const EnemyGenome::Ptr& genome, std::vector<std::unique_ptr<Enemy>>& enemies, const sf::Vector2i& point) {
    if (m_grid->getCell(point.x, point.y) == CellType::Empty) {
        std::cout << "Se creo el enemigo y esta es la posicion: ( " << point.x << ", " << point.y << " )" << std::endl;
            auto it = m_grid->m_precomputedPaths.find(point);
            if (it != m_grid->m_precomputedPaths.end()) {
                std::cout << "si se encontro el camino" << std::endl;
                enemies.emplace_back(std::make_unique<Enemy>(
                    genome,
                    point.x,
                    point.y,
                    m_grid,
                    it->second
                ));
            }
        }

}

//revisar bien esta logica
bool Wave::isCompleted() const {
    //bool timeExpired = m_timeElapsed >= m_config.waveDuration;
    bool allEnemiesSpawned = m_enemiesSpawned >= m_totalEnemies;
    bool allGenomesUsed = m_usedGenomes.size() >= m_geneticManager->getCurrentGenomes().size();

    return (allEnemiesSpawned && allGenomesUsed); //|| timeExpired
}