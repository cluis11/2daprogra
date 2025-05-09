#include "Wave.h"
#include <iostream>

//constructor
Wave::Wave(int waveNumber, const std::vector<sf::Vector2i>& spawnPoints, const Config& config, GridSystem* grid)
    : m_waveNumber(waveNumber), m_spawnPoints(spawnPoints), m_config(config), m_grid(grid) {
    
    // Calcula spawn points activos (mínimo 1, máximo config.maxSpawnPoints)
    m_activeSpawnPoints = std::min(
        std::max(1, 3 + (waveNumber - 1)), // Ejemplo: 3, 4, 5, 6...
        std::min(m_config.maxSpawnPoints, static_cast<int>(m_spawnPoints.size()))
    );

    // Calcula total de enemigos (60s / 10s * spawn points activos)
    m_totalEnemies = static_cast<int>(m_config.waveDuration / m_config.spawnInterval) * m_activeSpawnPoints;
}

void Wave::update(float deltaTime, std::vector<std::unique_ptr<Enemy>>& enemies) {
    m_timeElapsed += deltaTime;
    m_spawnTimer += deltaTime;

    if (m_spawnTimer >= m_config.spawnInterval && m_timeElapsed <= m_config.waveDuration) {
        spawnEnemy(enemies);
        m_spawnTimer = 0.f;
    }
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

bool Wave::isCompleted() const {
    return m_timeElapsed >= m_config.waveDuration;
}