#include "Wave.h"
#include "GeneticManager.h" 
#include <iostream>
#include <random>

//constructor
Wave::Wave(int waveNumber, const std::vector<sf::Vector2i>& spawnPoints, Config& config, GridSystem* grid, GeneticManager* geneticManager)
    : m_waveNumber(waveNumber), m_spawnPoints(spawnPoints), m_config(config), m_grid(grid), m_geneticManager(geneticManager) {
    
    // Calcula spawn points activos (mínimo 1, máximo config.maxSpawnPoints)
    m_activeSpawnPoints = std::min(
        waveNumber * 33, // aumenta en múltiplos de 33 por ronda
        std::min(m_config.maxSpawnPoints, static_cast<int>(m_spawnPoints.size()))
    );
    m_enemiesDead = 0;

    //Almacena los puntos activos en el nuevo vector
    m_activeSpawnList.assign(m_spawnPoints.begin(), m_spawnPoints.begin() + m_activeSpawnPoints);
    
    m_spawnTimer=m_config.spawnInterval;

    if (m_waveNumber==1){ generateInitialEnemies(); }

    std::cout << "\n=== OLEADA " << m_waveNumber << " ===\n";
    std::cout << "Max enemigos: " << m_config.maxEnemies
              << " | Spawn cada: " << m_config.spawnInterval << "s"
              << " | Puntos de spawn: " << m_activeSpawnPoints
              << " | Genomas disponibles: " << m_geneticManager->getCurrentGenomes().size() << "\n";
}

void Wave::update(float deltaTime, std::vector<std::unique_ptr<Enemy>>& enemies) {
    m_timeElapsed += deltaTime;
    //m_spawnTimer += deltaTime;

    if (m_enemiesSpawned <= m_config.maxEnemies) {
        m_spawnTimer += deltaTime;
        auto genomes = getGenomesForNextSpawn(); // Ahora devuelve Ptr
        int i=0;
        for (const auto& genome : genomes) {
            const auto& point = m_spawnPoints[i % m_spawnPoints.size()];
            spawnEnemy(genome, enemies, point); // genome ya es Ptr
            i++;
        }
    } 

    if (m_waveNumber < m_config.totalWaves && m_enemiesDead == m_config.maxEnemies) {
        std::cout << "Se supone que solo entro al final de cada wave" << "\n";
        // 1. Evaluar la generacion actual (calcula fitness)
        m_geneticManager->evaluateGeneration(m_deathEnemyStats);

        EnemyGenome::resetMutationCount();

        int requiredPopulation = 20 + m_config.maxEnemies;

        // 3. Crear nueva generación BASADA EN LOS GENOMAS ACTUALES
        m_geneticManager->createNextGeneration(requiredPopulation);

        // 4. Limpiar enemigos existentes (pero no los genomas)
        enemies.clear();
        completed = true;
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

    if (m_enemiesSpawned < m_config.maxEnemies && m_spawnTimer >= m_config.spawnInterval) {
        m_spawnTimer = 0.f;
        int toSpawn = std::min(m_activeSpawnPoints,
                             m_config.maxEnemies - m_enemiesSpawned);

        int availableGenomes = static_cast<int>(m_geneticManager->getCurrentGenomes().size() - m_usedGenomes.size());
        toSpawn = std::min(toSpawn, availableGenomes);

        if (toSpawn > 0) {
            // Obtiene genomas unicos no usados aun
            auto unusedGenomes = getUnusedGenomes(toSpawn);
            genomes.insert(genomes.end(), unusedGenomes.begin(), unusedGenomes.end());

            m_enemiesSpawned += toSpawn;
        }
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
        auto it = m_grid->m_precomputedPaths.find(point);
        if (it != m_grid->m_precomputedPaths.end()) {
            enemies.emplace_back(std::make_unique<Enemy>(
                genome,
                point.x,
                point.y,
                m_grid,
                it->second
            ));

            // Log detallado del spawn
            const auto& attrs = genome->getAttributes();
            std::cout << "Spawning Enemy ID:" << genome->getId()
              << " Type:" << static_cast<int>(genome->getType())
              << " at (" << point.x << "," << point.y << ")"
              << " [H:" << attrs.health
              << " S:" << attrs.speed
              << " A:" << attrs.armor
              << " MR:" << attrs.magicResist << "]\n";
        }
    }
}

void Wave::enemyDead(const EnemyGenome::Ptr& genome, float steps){
    m_enemiesDead++;
    std::cout << genome->getFitness() << "\n\n";
    enemyData data = {genome, steps};
    m_deathEnemyStats.emplace_back(data);
    
}

// Función que verifica si todos los enemigos están muertos
bool Wave::isCompleted() const {
    if (completed) {
        bool allEnemiesDead = m_enemiesDead == m_enemiesSpawned; //m_config.maxEnemies;
        return allEnemiesDead; 
    }
    return false;
}
