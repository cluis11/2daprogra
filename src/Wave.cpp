#include "Wave.h"

//constructor
Wave::Wave(int waveNumber, const std::vector<sf::Vector2i>& spawnPoints,
           const Config& config, GridSystem* grid,
           const std::vector<EnemyGenome>& genomes)
    : m_waveNumber(waveNumber),
      m_spawnPoints(spawnPoints),
      m_config(config),
      m_grid(grid),
      m_genomes(genomes)
{
    // Calcula spawn points activos (mínimo 1, máximo config.maxSpawnPoints)
    m_activeSpawnPoints = std::min(
        std::max(1, 3 + (waveNumber - 1)), // Ejemplo: 3, 4, 5, 6...
        std::min(m_config.maxSpawnPoints, static_cast<int>(m_spawnPoints.size()))
    );

    // Calcula total de enemigos (60s / 10s * spawn points activos)
    m_totalEnemies = static_cast<int>(m_config.waveDuration / m_config.spawnInterval) * m_activeSpawnPoints;
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
        int enemiesToSpawn = std::min(m_activeSpawnPoints,
                                    m_totalEnemies - m_enemiesSpawned);

        for (int i = 0; i < enemiesToSpawn; ++i) {
            const auto& point = m_spawnPoints[i % static_cast<int>(m_spawnPoints.size())];
            if (m_grid->getCell(point.x, point.y) == CellType::Empty) {
                std::uniform_int_distribution<int> dist(0, static_cast<int>(m_genomes.size()) - 1);
                genomesToSpawn.push_back(m_genomes[dist(m_grid->getRNG())]);
                m_enemiesSpawned++;
            }
        }
        m_spawnTimer = 0.f;
    }

    return genomesToSpawn;
}

bool Wave::isCompleted() const {
    return m_timeElapsed >= m_config.waveDuration;
}