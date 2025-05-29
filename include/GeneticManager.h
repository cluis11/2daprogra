#pragma once
#include "EnemyGenome.h"
#include <vector>
#include <memory>
#include "Wave.h" 

class Enemy;

class GeneticManager {
public:
    GeneticManager();

    // Genera un nuevo genoma para un enemigo
    EnemyGenome::Ptr generateEnemyGenome(EnemyType type);

    // Evalua la generación actual basado en el fitness de los enemigos
    void evaluateGeneration(std::vector<Wave::enemyData> enemies);

    // Crea una nueva generación usando los mejores genomas
    void createNextGeneration(int requiredPopulation);

    // Obtiene los genomas de la generacion actual
    const std::vector<EnemyGenome::Ptr>& getCurrentGenomes() const { return m_currentGenomes; }

    // Reinicia el administrador genético (nuevo metodo)
    void resetGeneration(bool force = false) {
        if (force || !m_nextGenomes.empty()) {
            m_currentGenomes = std::move(m_nextGenomes);
            m_nextGenomes.clear();
        }
        EnemyGenome::resetIdCounter(); // Resetear IDs si es necesario
    }

private:
    std::vector<EnemyGenome::Ptr> m_currentGenomes; // Genomas de la generacion actual
    std::vector<EnemyGenome::Ptr> m_nextGenomes; // Siguiente generacion

    // Parametros geneticos
    const float MUTATION_RATE = 0.05f;
    const float SELECTION_RATE = 0.3f;

    // Genera atributos aleatorios para un tipo de enemigo
    EnemyGenome::Attributes getRandomAttributesForType(EnemyType type) const;

    // Selecciona padre para reproduccion
    EnemyGenome::Ptr selectParent(
        const std::vector<EnemyGenome::Ptr>& genomes,
        size_t eliteCount,
        EnemyType requiredType,
        int excludedId = -1);  // Parametro opcional para excluir un ID, que los 2 padres no sea el mismo.

    // Selecciona padre de cualquier tipo (para casos especiales)
    EnemyGenome::Ptr selectParentFromAllTypes(
        const std::vector<EnemyGenome::Ptr>& genomes,
        int excludedId = -1);

    // Valida la generación actual
    void validateGeneration();
};