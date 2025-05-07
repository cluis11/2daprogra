#pragma once
#include <vector>
#include <memory>
#include "EnemyGenome.h"
#include "EnemyTypes.h"

class Enemy;

class GeneticManager {
public:
    GeneticManager();
    
    void initializeFirstGeneration();
    void evaluateGeneration(const std::vector<std::unique_ptr<Enemy>>& enemies);
    void createNextGeneration();
    int countGenomesOfType(const std::vector<EnemyGenome>& genomes, EnemyType type) const;
    
    const std::vector<EnemyGenome>& getCurrentGenomes() const { return m_currentGenomes; }

private:
    std::vector<EnemyGenome> m_currentGenomes;
    std::vector<EnemyGenome> m_nextGenomes;
    
    const float MUTATION_RATE = 0.05f; //probabilidad de 5% de mutar
    const float SELECTION_RATE = 0.3f; //Se elige el 30% con mejor finess
    
    EnemyGenome::Attributes getRandomAttributesForType(EnemyType type) const;

};