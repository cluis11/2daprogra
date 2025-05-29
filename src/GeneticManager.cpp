#include "GeneticManager.h"
#include "Enemy.h"
#include "Wave.h"
#include <random>
#include <algorithm>
#include <iostream>
#include <unordered_set>

GeneticManager::GeneticManager() {
    // Inicialización vacía, la población se generará bajo demanda
}

EnemyGenome::Ptr GeneticManager::generateEnemyGenome(EnemyType type) {
    auto attrs = getRandomAttributesForType(type);
    auto genome = std::make_shared<EnemyGenome>(type, attrs);
    m_currentGenomes.push_back(genome);

    std::cout << "Generado nuevo genoma - ID: " << genome->getId()
              << ", Tipo: " << static_cast<int>(type)
              << ", Total genomas: " << m_currentGenomes.size() << "\n";

    return genome;
}

EnemyGenome::Attributes GeneticManager::getRandomAttributesForType(EnemyType type) const {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Configuracion de atributos por tipo
    struct TypeConfig {
        float healthMin, healthMax;
        float speedMin, speedMax;
        float armorMax, magicResistMax;
    };

    static const std::unordered_map<EnemyType, TypeConfig> typeConfigs = {
        {EnemyType::Ogre,      {100.0f, 200.0f, 0.5f, 1.2f, 0.5f, 0.3f}},
        {EnemyType::DarkElf,   {60.0f, 120.0f, 1.0f, 2.0f, 0.3f, 0.5f}},
        {EnemyType::Harpy,     {80.0f, 150.0f, 1.5f, 2.5f, 0.2f, 0.2f}},
        {EnemyType::Mercenary, {120.0f, 180.0f, 0.8f, 1.5f, 0.6f, 0.4f}}
    };

    const auto& config = typeConfigs.at(type);

    // Distribuciones para cada atributo
    std::uniform_real_distribution<float> healthDist(config.healthMin, config.healthMax);
    std::uniform_real_distribution<float> speedDist(config.speedMin, config.speedMax);
    std::uniform_real_distribution<float> armorDist(0.0f, config.armorMax);
    std::uniform_real_distribution<float> magicResistDist(0.0f, config.magicResistMax);

    EnemyGenome::Attributes attrs;
    attrs.health = healthDist(gen);
    attrs.speed = speedDist(gen);
    attrs.armor = armorDist(gen);
    attrs.magicResist = magicResistDist(gen);
    attrs.stepsTaken = 0;

    return attrs;
}

void GeneticManager::evaluateGeneration(std::vector<Wave::enemyData> enemies) {
    std::cout << "\n=== EVALUANDO GENERACIÓN ===\n";

    // Reset fitness
    for (auto& genome : m_currentGenomes) {
        genome->setFitness(0.0f);
    }

    // Asignar fitness basado en pasos recorridos
    for (const auto& enemy : enemies) {
        auto genome = enemy.m_genome;
        genome->setFitness(genome->getFitness() + enemy.steps);
    }

    // Ordenar por fitness
    std::sort(m_currentGenomes.begin(), m_currentGenomes.end(),
        [](const auto& a, const auto& b) { return a->getFitness() > b->getFitness(); });

    // Mostrar resultados
    for (const auto& genome : m_currentGenomes) {
        const auto& attrs = genome->getAttributes();
        std::cout << "ID:" << genome->getId()
                  << " T:" << static_cast<int>(genome->getType())
                  << " F:" << genome->getFitness()
                  << " H:" << attrs.health
                  << " S:" << attrs.speed
                  << " A:" << attrs.armor
                  << " MR:" << attrs.magicResist << "\n";
    }
}


EnemyGenome::Ptr GeneticManager::selectParent(
    const std::vector<EnemyGenome::Ptr>& genomes,
    size_t eliteCount,
    EnemyType requiredType,
    int excludedId)
{
    static std::mt19937 gen(std::random_device{}());

    // 1. Buscar candidatos válidos del tipo requerido (excluyendo excludedId)
    std::vector<EnemyGenome::Ptr> validCandidates;
    for (const auto& genome : genomes) {
        if (genome->getType() == requiredType && genome->getId() != excludedId) {
            validCandidates.push_back(genome);
        }
    }

    // 2. Si no hay del tipo requerido, buscar cualquier tipo (excepto excludedId)
    if (validCandidates.empty()) {
        for (const auto& genome : genomes) {
            if (genome->getId() != excludedId) {
                validCandidates.push_back(genome);
            }
        }
    }

    // 3. Si aun no hay, permitir incluso el excludedId (último recurso)
    if (validCandidates.empty()) {
        validCandidates = genomes;
    }

    // 4. Si realmente no hay genomas
    if (validCandidates.empty()) {
        std::cerr << "ERROR CRÍTICO: No se encontraron genomas para selección\n";
        std::cerr << "Genomas disponibles en el sistema: " << genomes.size() << "\n";
        for (const auto& g : genomes) {
            std::cerr << "ID:" << g->getId() << " T:" << static_cast<int>(g->getType())
                     << " F:" << g->getFitness() << "\n";
        }
        throw std::runtime_error("Fallo crítico en selección de padres");
    }

    // 5. Clasificar en elite y no-elite
    std::vector<EnemyGenome::Ptr> eliteCandidates;
    std::vector<EnemyGenome::Ptr> nonEliteCandidates;

    for (const auto& genome : validCandidates) {
        bool isElite = false;
        for (size_t i = 0; i < eliteCount && i < genomes.size(); ++i) {
            if (genomes[i]->getId() == genome->getId()) {
                isElite = true;
                break;
            }
        }

        if (isElite) {
            eliteCandidates.push_back(genome);
        } else {
            nonEliteCandidates.push_back(genome);
        }
    }

    // 6. Seleccion con preferencia por elite (80% de probabilidad)
    std::bernoulli_distribution eliteDist(0.8);

    if (!eliteCandidates.empty() && eliteDist(gen)) {
        return eliteCandidates[std::uniform_int_distribution<size_t>(0, eliteCandidates.size()-1)(gen)];
    }

    if (!nonEliteCandidates.empty()) {
        return nonEliteCandidates[std::uniform_int_distribution<size_t>(0, nonEliteCandidates.size()-1)(gen)];
    }

    return validCandidates[std::uniform_int_distribution<size_t>(0, validCandidates.size()-1)(gen)];
}

void GeneticManager::createNextGeneration(int requiredPopulation) {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    m_nextGenomes.clear();
    std::cout << "\n=== CREANDO NUEVA GENERACIÓN (" << requiredPopulation << " enemigos) ===\n";

    // 1. Ordenar por fitness
    std::sort(m_currentGenomes.begin(), m_currentGenomes.end(),
        [](const auto& a, const auto& b) { return a->getFitness() > b->getFitness(); });

    // 2. Conservar SOLO el 10% como elite
    size_t eliteCount = std::min(
        static_cast<size_t>(requiredPopulation * 0.1f), // Solo 10% elite
        m_currentGenomes.size()
    );

    // 3. Llenar nueva generacion con la elite
    for (size_t i = 0; i < eliteCount; ++i) {
        auto eliteCopy = std::make_shared<EnemyGenome>(*m_currentGenomes[i]);
        m_nextGenomes.push_back(eliteCopy);
        std::cout << "Elite conservada - ID:" << eliteCopy->getId()
                 << " Tipo:" << static_cast<int>(eliteCopy->getType())
                 << " Fitness:" << eliteCopy->getFitness() << "\n";
    }

    // 4. Organizar poblacion por tipos
    std::array<std::vector<EnemyGenome::Ptr>, 4> genomesByType;
    for (const auto& genome : m_currentGenomes) {
        genomesByType[static_cast<int>(genome->getType())].push_back(genome);
    }

    // 5. Generar descendencia balanceada
    while (m_nextGenomes.size() < requiredPopulation) {
        // Selección rotativa de tipos (0-1-2-3)
        int targetType = m_nextGenomes.size() % 4;

        EnemyGenome::Ptr parent1, parent2;
        bool usingPreferredParents = false;

        // Estrategia mejorada de seleccion de padres:
        // 1. Primero intentar encontrar 2 padres del tipo objetivo en toda la poblacion
        if (genomesByType[targetType].size() >= 2) {
            std::uniform_int_distribution<size_t> dist(0, genomesByType[targetType].size()-1);
            size_t idx1 = dist(gen);
            size_t idx2;

            do {
                idx2 = dist(gen);
            } while (idx2 == idx1 && genomesByType[targetType].size() > 1);

            parent1 = genomesByType[targetType][idx1];
            parent2 = genomesByType[targetType][idx2];
            usingPreferredParents = true;
        }
        // 2. Si no hay suficientes, buscar un padre del tipo y otro cualquiera
        else if (!genomesByType[targetType].empty()) {
            parent1 = genomesByType[targetType][0];
            parent2 = selectParentFromAllTypes(m_currentGenomes, parent1->getId());
        }
        // 3. Como ultimo recurso, usar cualquier padre
        else {
            parent1 = selectParentFromAllTypes(m_currentGenomes, -1);
            parent2 = selectParentFromAllTypes(m_currentGenomes, parent1->getId());
        }

        // Crear hijo con validacion
        auto child = EnemyGenome::crossoverUniform(parent1, parent2);
        child->mutate(MUTATION_RATE);
        m_nextGenomes.push_back(child);

        std::cout << "Nuevo hijo - ID:" << child->getId()
                 << " Tipo:" << targetType
                 << " | Padres: " << parent1->getId() << "(T" << static_cast<int>(parent1->getType())
                 << (usingPreferredParents ? "*" : "") << "), "
                 << parent2->getId() << "(T" << static_cast<int>(parent2->getType())
                 << (usingPreferredParents ? "*" : "") << ")\n";
    }

    // 6. Mezclar y actualizar generacion
    std::shuffle(m_nextGenomes.begin(), m_nextGenomes.end(), gen);
    m_currentGenomes = m_nextGenomes;

    // 7. Validacion final
    validateGeneration();
}

EnemyGenome::Ptr GeneticManager::selectParentFromAllTypes(
    const std::vector<EnemyGenome::Ptr>& genomes,
    int excludedId)
{
    static std::mt19937 gen(std::random_device{}());

    std::vector<EnemyGenome::Ptr> candidates;
    for (const auto& genome : genomes) {
        if (genome->getId() != excludedId) {
            candidates.push_back(genome);
        }
    }

    if (candidates.empty()) {
        candidates = genomes; // Permitir cualquier como último recurso
    }

    if (candidates.empty()) {
        throw std::runtime_error("No hay genomas disponibles para selección");
    }

    // Selección por torneo (más eficiente que ruleta)
    const size_t tournamentSize = std::min(static_cast<size_t>(3), candidates.size());
    std::vector<EnemyGenome::Ptr> tournament;

    std::uniform_int_distribution<size_t> dist(0, candidates.size()-1);
    for (size_t i = 0; i < tournamentSize; ++i) {
        size_t randomIndex = dist(gen);
        tournament.push_back(candidates[randomIndex]);
    }

    // Seleccionar el mejor del torneo
    return *std::max_element(tournament.begin(), tournament.end(),
        [](const auto& a, const auto& b) { return a->getFitness() < b->getFitness(); });
}

void GeneticManager::validateGeneration() {
    std::unordered_set<int> ids;
    std::array<int, 4> typeCounts = {0};
    float totalFitness = 0.0f;

    for (const auto& genome : m_currentGenomes) {
        // Verificar IDs unicos
        if (ids.count(genome->getId())) {
            std::cerr << "ERROR: ID duplicado " << genome->getId() << "\n";
        }
        ids.insert(genome->getId());

        // Contar tipos
        typeCounts[static_cast<int>(genome->getType())]++;

        // Calcular fitness total
        totalFitness += genome->getFitness();

        // Validar atributos
        const auto& attrs = genome->getAttributes();
        if (attrs.health <= 0 || attrs.speed <= 0) {
            std::cerr << "ADVERTENCIA: Genoma " << genome->getId()
                     << " tiene atributos inválidos\n";
        }
    }

    // Reporte final
    std::cout << "\n=== VALIDACION FINAL ===\n";
    std::cout << "Distribucion de tipos:\n";
    for (int i = 0; i < 4; ++i) {
        std::cout << "Tipo " << i << ": " << typeCounts[i] << " ("
                 << (100.0f * typeCounts[i] / m_currentGenomes.size()) << "%)\n";
    }
    std::cout << "Fitness promedio: " << (totalFitness / m_currentGenomes.size()) << "\n";
    std::cout << "Total genomas: " << m_currentGenomes.size() << "\n";
}