#include "GeneticManager.h"
#include "Enemy.h"
#include <algorithm>
#include <random>
#include <iostream>

GeneticManager::GeneticManager() {
    // Inicializacion básica
}

// Crea la primera generacion de genomas con atributos aleatorios
void GeneticManager::initializeFirstGeneration() {
    m_currentGenomes.clear();

    //Genera 5 individuos por cada tipo de enemigo (4 tipos)
    for (int i = 0; i < 4; ++i) { // 4 tipos de enemigos
        EnemyType type = static_cast<EnemyType>(i);
        for (int j = 0; j < 5; ++j) { // 5 individuos por tipo todo : cambiar a 10 en el juego final

            //Genera atributos aleatorios segun  tipo
            auto attrs = getRandomAttributesForType(type);
            m_currentGenomes.emplace_back(type, attrs);

            // depurar stats iniciales
            std::cout << "Genoma inicial - Raza: " << static_cast<int>(type)
                      << " Salud: " << attrs.health
                      << " Velocidad: " << attrs.speed
                      << " Armadura: " << attrs.armor
                      << " Resistencia magica: " << attrs.magicResist << std::endl;
        }
    }
}

// Evalua la generación actual basandose en el desempeño de los enemigos
void GeneticManager::evaluateGeneration(const std::vector<std::unique_ptr<Enemy>>& enemies) {
    std::cout << "\n=== EVALUANDO GENERACION ===" << std::endl;

    // Resetear fitness primero
    for (auto& genome : m_currentGenomes) {
        genome.setFitness(0.0f);
    }

    // Calcular fitness basado en pasos de enemigos muertos
    for (const auto& enemy : enemies) {
        for (auto& genome : m_currentGenomes) {
            if (genome.getType() == enemy->getType()) {
                // Solo suma pasos si el enemigo murio (isAlive() == false)
                if (!enemy->isAlive()) {
                    genome.setFitness(genome.getFitness() + enemy->getStepsTaken());
                }
                break; // Rompe el loop interno al encontrar el genoma
            }
        }
    }

    // Mostrar resumen por raza
    std::vector<float> raceFitness(4, 0.0f);
    std::vector<int> raceCount(4, 0);

    // Acumula fitness y cuenta individuos por tipo
    for (const auto& genome : m_currentGenomes) {
        int typeIdx = static_cast<int>(genome.getType());
        raceFitness[typeIdx] += genome.getFitness();
        raceCount[typeIdx]++;

        std::cout << "Genoma " << typeIdx << " Fitness: " << genome.getFitness()
                  << " (H:" << genome.getAttributes().health
                  << ", S:" << genome.getAttributes().speed << ")" << std::endl;
    }

    // Mostrar promedios por raza
    for (int i = 0; i < 4; ++i) {
        if (raceCount[i] > 0) {
            std::cout << "Raza " << i << " - Fitness promedio: "
                      << (raceFitness[i] / raceCount[i]) << std::endl;
        }
    }
}
// Crea una nueva generacion mediante seleccion, cruce y mutacion
void GeneticManager::createNextGeneration() {
    std::cout << "\n=== Creando nueva generacion ===" << std::endl;
    m_nextGenomes.clear();

    // 1. Primero agrupamos los genomas por su tipo (raza)
    std::vector<std::vector<EnemyGenome>> genomesByRace(4);
    for (const auto& genome : m_currentGenomes) {
        genomesByRace[static_cast<int>(genome.getType())].push_back(genome);
    }

    // 2. Procesamos cada raza por separado
    for (auto& raceGenomes : genomesByRace) {
        if (raceGenomes.empty()) continue;

        // Ordenar por fitness
        std::sort(raceGenomes.begin(), raceGenomes.end(),
            [](const EnemyGenome& a, const EnemyGenome& b) {
                return a.getFitness() > b.getFitness();
            });

        // Mostrar el mejor de cada raza
        std::cout << "Mejor de raza " << static_cast<int>(raceGenomes[0].getType())
                  << ": Fitness = " << raceGenomes[0].getFitness()
                  << " Stats: H=" << raceGenomes[0].getAttributes().health
                  << " S=" << raceGenomes[0].getAttributes().speed << std::endl;

        // Selección (30% de la raza)
        size_t eliteCount = std::max(1, static_cast<int>(raceGenomes.size() * SELECTION_RATE));

        // Conservar elite
        for (size_t i = 0; i < eliteCount; ++i) {
            m_nextGenomes.push_back(raceGenomes[i]);
        }

        // Reproducir
        std::random_device rd;
        std::mt19937 gen(rd());

        while (true) {
            // Contamos cuántos genomas de este tipo ya tenemos
            int currentCount = 0;
            for (const auto& genome : m_nextGenomes) {
                if (genome.getType() == raceGenomes[0].getType()) currentCount++;
            }
            // Si ya completamos la cantidad necesaria, pasamos a la siguiente raza
            if (currentCount >= raceGenomes.size()) break;

            // Seleccionamos dos padres aleatorios del elite de ESTA RAZA
            std::uniform_int_distribution<size_t> dist(0, eliteCount - 1);

            const auto& parent1 = raceGenomes[dist(gen)];
            const auto& parent2 = raceGenomes[dist(gen)];
            // Cruzamos
            EnemyGenome child = EnemyGenome::crossoverUniform(parent1, parent2);

            // Mutamos
            child.mutate(MUTATION_RATE);

            std::cout << "Nuevo genoma - Raza: " << static_cast<int>(child.getType())
                      << " H=" << child.getAttributes().health
                      << " S=" << child.getAttributes().speed << std::endl;

            m_nextGenomes.push_back(child);
        }
    }
    // La nueva generación se convierte en la actual
    m_currentGenomes = std::move(m_nextGenomes);
}

// Genera atributos aleatorios para un tipo específico de enemigo
EnemyGenome::Attributes GeneticManager::getRandomAttributesForType(EnemyType type) const {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Rangos base para los atributos según tipo
    float healthMin, healthMax, speedMin, speedMax;

    switch(type) {
        case EnemyType::Ogre: // Ogros: alta salud, baja velocidad
            healthMin = 100.0f; healthMax = 200.0f;
        speedMin = 0.5f; speedMax = 1.2f;
        break;
        case EnemyType::DarkElf: // Elfos: salud media, alta velocidad
            healthMin = 60.0f; healthMax = 120.0f;
        speedMin = 1.0f; speedMax = 2.0f;
        break;
        case EnemyType::Harpy: // Arpías: salud media-alta, muy alta velocidad
            healthMin = 80.0f; healthMax = 150.0f;
        speedMin = 1.5f; speedMax = 2.5f;
        break;
        case EnemyType::Mercenary: // Mercenarios: alta salud, velocidad media
            healthMin = 120.0f; healthMax = 180.0f;
        speedMin = 0.8f; speedMax = 1.5f;
        break;
        default:
            healthMin = 50.0f; healthMax = 150.0f;
        speedMin = 0.5f; speedMax = 2.0f;
    }

    // Distribuciones para cada atributo
    std::uniform_real_distribution<float> healthDist(healthMin, healthMax);
    std::uniform_real_distribution<float> speedDist(speedMin, speedMax);
    std::uniform_real_distribution<float> resistDist(0.0f, 0.5f); // Resistencias bajas

    EnemyGenome::Attributes attrs;
    attrs.health = healthDist(gen);
    attrs.speed = speedDist(gen);
    attrs.armor = resistDist(gen);
    attrs.magicResist = resistDist(gen);
    attrs.stepsTaken = 0; // Inicializar pasos

    return attrs;
}