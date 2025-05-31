#include "GameStats.h"
#include <fstream>
#include <iostream>

void GameStats::recordWaveStart(int generation, int totalEnemies) {
    
    m_generation = generation;
    
    // Si no existe, crear nueva
    WaveStats ws;
    ws.waveNumber = m_generation;
    ws.totalEnemies = totalEnemies;
    ws.killedEnemies = 0;
    ws.averageFitness = 0;
    ws.mutationsCount = 0;
    m_stats.push_back(ws);
}

void GameStats::recordEnemyDeath(const std::string& killerTowerType) {
    auto& w = getCurrentGeneration();
    w.killedEnemies++;
    w.towerStats.kills++;
    if (killerTowerType=="archer"){ w.towerStats.archerKills++; }
    else if(killerTowerType=="mage"){ w.towerStats.mageKills++; }
    else if(killerTowerType=="artillery"){ w.towerStats.artilleryKills++; }
}

void GameStats::recordFitness(float fitness){
    auto& w = getCurrentGeneration();
    w.averageFitness=fitness;
}

void GameStats::recordMutation() {
    auto& w = getCurrentGeneration();
    w.mutationsCount++;
}

void GameStats::recordTower(const std::string& towerType, int level) {
    auto& w = getCurrentGeneration();
    
    if (towerType == "archer") {
        w.towerStats.archerCount++;
        if (level == 1) w.towerStats.archerLevel1++;
        else if (level == 2) w.towerStats.archerLevel2++;
        else if (level == 3) w.towerStats.archerLevel3++;
    }
    else if (towerType == "mage") {
        w.towerStats.mageCount++;
        if (level == 1) w.towerStats.mageLevel1++;
        else if (level == 2) w.towerStats.mageLevel2++;
        else if (level == 3) w.towerStats.mageLevel3++;
    }
    else if (towerType == "artillery") {
        w.towerStats.artilleryCount++;
        if (level == 1) w.towerStats.artilleryLevel1++;
        else if (level == 2) w.towerStats.artilleryLevel2++;
        else if (level == 3) w.towerStats.artilleryLevel3++;
    }
}

WaveStats& GameStats::getCurrentGeneration() {
    for (auto& gen : m_stats) {
        if (gen.waveNumber == m_generation) {
            return gen;
        }
    }
    throw std::runtime_error("No se encontró la generación actual.");
}
