#pragma once
#include "Enemy.h"
#include "Tower.h"
#include "Wave.h"
#include <vector>
#include <map>
#include <SFML/Graphics.hpp>

class StatisticsManager {
public:
    struct EnemyStats {
        int generation;
        float fitness;
        EnemyType type;
        float damageTaken;
        int steps;
        bool survived;
    };

    struct WaveStats {
        int waveNumber;
        int totalEnemies;
        int enemiesDefeated;
        float avgFitness;
        std::vector<EnemyStats> enemyDetails;
    };

    struct TowerStats {
        Tower::Type type;
        int level;
        int kills;
        float totalDamage;
    };

    StatisticsManager();

    // Registro de eventos
    void recordEnemySpawn(const Enemy& enemy);
    void recordEnemyDeath(const Enemy& enemy, const std::string& killerType);
    void recordTowerAction(const Tower& tower, float damage);
    void recordWaveStart(const Wave& wave);
    void recordWaveEnd();
    void recordMutation(int mutationsCount);

    // UI
    void render(sf::RenderWindow& window) const;
    void updateUI();

private:
    std::vector<WaveStats> m_waveHistory;
    std::map<int, std::vector<TowerStats>> m_towerStats; // Wave -> Stats
    std::map<int, int> m_mutationStats; // Generación -> Mutaciones
    
    // Datos oleada actual
    WaveStats m_currentWave;
    std::vector<TowerStats> m_currentTowers;

    // UI
    sf::Font m_font;
    sf::RectangleShape m_statsPanel;
    sf::Text m_generalStatsText;
    sf::Text m_detailedStatsText;
    bool m_showDetails = false;

    void updateGeneralStats();
    void updateDetailedStats();
};