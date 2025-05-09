#pragma once
#include <vector>
#include <memory>
#include "EnemyGenome.h"
#include "EnemyTypes.h"

class Enemy;

class GeneticManager {
public:
    GeneticManager();
    
    void initializeFirstGeneration(); //Inicializa la primera generación con genomas completamente aleatorios
    void evaluateGeneration(const std::vector<std::unique_ptr<Enemy>>& enemies); // Evalua la generación actual basado en el fitness de los enemigos
    void createNextGeneration();//Crea una nueva generacion usando los mejores genomas de la generacion anterior con seleccion, cruce y mutacion

    // Cuenta cuantos genomas hay de un tipo especifico
    int countGenomesOfType(const std::vector<EnemyGenome>& genomes, EnemyType type) const;

    // Obtiene los genomas de la generacion actual
    const std::vector<EnemyGenome>& getCurrentGenomes() const { return m_currentGenomes; }

private:
    std::vector<EnemyGenome> m_currentGenomes;
    std::vector<EnemyGenome> m_nextGenomes;
    
    const float MUTATION_RATE = 0.05f; //probabilidad de 5% de mutar
    const float SELECTION_RATE = 0.3f; //Se elige el 30% con mejor finess

    // Genera atributos aleatorios para un tipo de enemigo
    EnemyGenome::Attributes getRandomAttributesForType(EnemyType type) const;

};