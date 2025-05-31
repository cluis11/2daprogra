// GameStats.h
#pragma once
#include <vector>
#include <map>
#include <string>

struct TowerStats {
    int kills = 0;
    int archerKills = 0;
    int archerCount = 0;
    int mageKills = 0;
    int mageCount = 0;
    int artilleryKills = 0;
    int artilleryCount = 0;
    
    // Torres por nivel
    int archerLevel1 = 0;
    int archerLevel2 = 0;
    int archerLevel3 = 0;
    int mageLevel1 = 0;
    int mageLevel2 = 0;
    int mageLevel3 = 0;
    int artilleryLevel1 = 0;
    int artilleryLevel2 = 0;
    int artilleryLevel3 = 0;
};

struct WaveStats {
    int waveNumber;
    int totalEnemies;
    int killedEnemies;
    float averageFitness;
    int mutationsCount;
    //int ogreCount;
    //int elfCount;
    //int harpyCount;
    //int MercenaryCount;
    TowerStats towerStats;
};

class GameStats {
public:
    void recordWaveStart(int generation, int totalEnemies);
    void recordEnemyDeath(const std::string& killerTowerType);
    void recordFitness(float fitness);
    void recordMutation();
    void recordTower(const std::string& towerType, int level);
    
    const std::vector<WaveStats>& getStats() const { return m_stats; }
    WaveStats& getCurrentGeneration();

    bool hasData() const { return !m_stats.empty(); }
    const std::vector<WaveStats>& getAllStats() const { return m_stats; }
    const WaveStats* getWaveStats(int waveNumber) const {
        for (const auto& wave : m_stats) {
            if (wave.waveNumber == waveNumber) {
                return &wave;
            }
        }
        return nullptr;
    }
    
private:
    std::vector<WaveStats> m_stats;
    int m_generation;
};