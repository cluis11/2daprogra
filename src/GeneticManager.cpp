#include "GeneticManager.h"
#include "Enemy.h"
#include <random>
#include <algorithm>
#include <iostream>

GeneticManager::GeneticManager() {
    // Inicializacion de la primera generacion (vacía, se generara bajo demanda)
}

/*
 * Genera un nuevo genoma de enemigo con atributos aleatorios
 * Recibe Tipo de enemigo a generar
 * Retorna: Puntero compartido al nuevo genoma creado
 *
 * El genoma se agrega automaticamente a la generacion actual
 */
EnemyGenome::Ptr GeneticManager::generateEnemyGenome(EnemyType type) {
    // Obtiene atributos aleatorios para el tipo especificado
    auto attrs = getRandomAttributesForType(type);
    // Crea un nuevo genoma como shared_ptr
    auto genome = std::make_shared<EnemyGenome>(type, attrs);
    // Agrega el genoma a la generacion actual
    m_currentGenomes.push_back(genome);
    return genome;
}
/*
 * Genera atributos aleatorios para un tipo de enemigo especifico
 * Recibe type: Tipo de enemigo (Ogre, DarkElf, Harpy, Mercenary)
 * Retorna: Estructura Attributes con valores aleatorios dentro de rangos predefinidos
 *
 * Cada tipo tiene rangos diferentes para sus atributos:
 * - Ogros: alta salud, baja velocidad
 * - Elfos Oscuros: equilibrados
 * - Arpias: alta velocidad, baja defensa
 * - Mercenarios: buena salud y defensa
 */
EnemyGenome::Attributes GeneticManager::getRandomAttributesForType(EnemyType type) const {
    //generador de numeros aleatorios
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Rangos base para los atributos segun tipo
    float healthMin, healthMax, speedMin, speedMax, armorMax, magicResistMax;
    // Configura rangos segun tipo de enemigo
    switch(type) {
        case EnemyType::Ogre:
            healthMin = 100.0f; healthMax = 200.0f;  // Mucha salud
            speedMin = 0.5f; speedMax = 1.2f;       // Lento
            armorMax = 0.5f; magicResistMax = 0.3f; // Resistencia moderada
            break;
        case EnemyType::DarkElf:
            healthMin = 60.0f; healthMax = 120.0f;  // Salud media
            speedMin = 1.0f; speedMax = 2.0f;       // Velocidad media
            armorMax = 0.3f; magicResistMax = 0.5f; // Resistencia magica alta
            break;
        case EnemyType::Harpy:
            healthMin = 80.0f; healthMax = 150.0f;  // Salud decente
            speedMin = 1.5f; speedMax = 2.5f;       // Muy rapido
            armorMax = 0.2f; magicResistMax = 0.2f; // Poca defensa
            break;
        case EnemyType::Mercenary:
            healthMin = 120.0f; healthMax = 180.0f; // Buena salud
            speedMin = 0.8f; speedMax = 1.5f;       // Velocidad decente
            armorMax = 0.6f; magicResistMax = 0.4f; // Buena defensa fisica
            break;
        default: // Valores por defecto para tipos no especificados
            healthMin = 50.0f; healthMax = 150.0f;
            speedMin = 0.5f; speedMax = 2.0f;
            armorMax = 0.5f; magicResistMax = 0.5f;
    }

    // Distribuciones para cada atributo
    std::uniform_real_distribution<float> healthDist(healthMin, healthMax);
    std::uniform_real_distribution<float> speedDist(speedMin, speedMax);
    std::uniform_real_distribution<float> armorDist(0.0f, armorMax);
    std::uniform_real_distribution<float> magicResistDist(0.0f, magicResistMax);

    // Crea y retorna estructura de atributos con valores aleatorios
    EnemyGenome::Attributes attrs;
    attrs.health = healthDist(gen);
    attrs.speed = speedDist(gen);
    attrs.armor = armorDist(gen);
    attrs.magicResist = magicResistDist(gen);
    attrs.stepsTaken = 0; // resetea pasos a 0

    return attrs;
}
/*
 * Evalua la generacion actual basada en el desempeño de los enemigos
 * Recibe enemies: Lista de enemigos de la oleada que acaba de terminar
 *
 * Calcula el fitness de cada genoma basado en cuantos pasos avanzaron los enemigos
 * Ordena los genomas por fitness, los mejores primero
 * Muestra resultados en consola
 */

void GeneticManager::evaluateGeneration(const std::vector<std::unique_ptr<Enemy>>& enemies) {
    std::cout << "\n=== EVALUANDO GENERACION ===\n";
    std::cout << std::fixed << std::setprecision(2);

    // Reset fitness solo para genomas actuales
    for (auto& genome : m_currentGenomes) {
        genome->setFitness(0.0f);
    }

    // Calcula fitness basado en pasos recorridos por cada enemigo
    for (const auto& enemy : enemies) {
        auto genome = enemy->getGenome();
        float fitness = static_cast<float>(enemy->getStepsTaken());
        genome->setFitness(genome->getFitness() + fitness);
    }

    // Ordena genomas por fitness (de mayor a menor)
    std::sort(m_currentGenomes.begin(), m_currentGenomes.end(),
        [](const EnemyGenome::Ptr& a, const EnemyGenome::Ptr& b) {
            return a->getFitness() > b->getFitness();
        });

    // Mostrar resultados de evaluacion
    std::cout << "\n--- RESULTADOS DE GENERACION ---\n";
    for (const auto& genome : m_currentGenomes) {
        const auto& attrs = genome->getAttributes();
        std::cout << "Genoma ID:" << genome->getId()
                  << " Tipo:" << static_cast<int>(genome->getType())
                  << " Fitness:" << genome->getFitness()
                  << " (H:" << attrs.health
                  << " S:" << attrs.speed
                  << " A:" << attrs.armor
                  << " MR:" << attrs.magicResist << ")\n";
    }
}
/*
 * Crea una nueva generacion de genomas usando seleccion natural
 *
 * Proceso:
 * 1. Conserva los mejores genomas (elite) sin cambios
 * 2. Completa la poblacion con descendencia de los mejores
 * 3. Aplica mutaciones aleatorias a la descendencia
 * 4. Muestra estadisticas del proceso
 */
void GeneticManager::createNextGeneration() {
    //generador de numeros aleatorios
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::cout << std::fixed << std::setprecision(2);// Configura formato de salida para numeros, que sean de 2 decimales
    std::cout << "\n=== CREANDO NUEVA GENERACION ===\n";

    m_nextGenomes.clear();// Prepara la nueva generacion
    // Calcula cuantos genomas de elite conservar (30% por defecto)
    size_t eliteCount = static_cast<size_t>(m_currentGenomes.size() * SELECTION_RATE);
    eliteCount = std::max(eliteCount, static_cast<size_t>(1)); // Minimo 1

    // Conservar elite
    std::cout << "\n--- GENOMAS ELITE ---\n";
    for (size_t i = 0; i < eliteCount; ++i) {
        const auto& genome = m_currentGenomes[i];
        const auto& attrs = genome->getAttributes();

        // Muestra info del genoma elite
        std::cout << "ID:" << genome->getId()
                  << " Tipo:" << static_cast<int>(genome->getType())
                  << " Fit:" << genome->getFitness()
                  << " H:" << attrs.health
                  << " S:" << attrs.speed
                  << " A:" << attrs.armor
                  << " MR:" << attrs.magicResist << "\n";

        // Copia el genoma elite a la nueva generacion
        m_nextGenomes.push_back(std::make_shared<EnemyGenome>(*genome));
    }

    // Crea descendencia hasta completar la poblacion
    std::cout << "\n--- DESCENDENCIA ---\n";
    while (m_nextGenomes.size() < m_currentGenomes.size()) {
        // Selecciona un genoma aleatorio de la elite como referencia para el tipo
        std::uniform_int_distribution<size_t> typeDist(0, eliteCount-1);
        EnemyType targetType = m_currentGenomes[typeDist(gen)]->getType();

        // Selecciona primer padre
        auto parent1 = selectParent(m_currentGenomes, eliteCount, targetType);

        // Selecciona segundo padre (excluyendo al primero)
        auto parent2 = selectParent(m_currentGenomes, eliteCount, targetType, parent1->getId());

        // Si por algun motivo son iguales (no debería pasar), regenerar el segundo, Garantiza que los padres sean diferentes
        while (parent2->getId() == parent1->getId()) {
            parent2 = selectParent(m_currentGenomes, eliteCount, targetType, parent1->getId());
        }

        // Crea hijo mediante cruce genetico
        auto child = EnemyGenome::crossoverUniform(parent1, parent2);
        child->mutate(MUTATION_RATE); // Aplica posibles mutaciones al hijo
        m_nextGenomes.push_back(child);// Agrega el hijo a la nueva generacion
    }

    // Muestra resumen de la nueva generacion
    std::cout << "\n--- RESUMEN ---\n"
              << "Total: " << m_currentGenomes.size() << "\n"
              << "Elite: " << eliteCount << "\n"
              << "Nuevos: " << (m_currentGenomes.size() - eliteCount) << "\n";
}

/*
 * Selecciona un padre para reproduccion con exclusion opcional
 *  Recibe: -genomes: Lista de genomas disponibles
 *          -eliteCount: Cantidad de genomas elite
 *          -requiredType: Tipo de enemigo requerido
 *          -excludedId: ID a excluir
 * Retorna: Puntero al genoma seleccionado
 *
 * Prioriza seleccionar de la elite (80% probabilidad)
 * Si no hay candidatos validos, genera un nuevo genoma
 */
EnemyGenome::Ptr GeneticManager::selectParent(
    const std::vector<EnemyGenome::Ptr>& genomes,
    size_t eliteCount,
    EnemyType requiredType,
    int excludedId)  // Nuevo parámetro para excluir un ID específico
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Busca candidatos validos (mismo tipo y ID diferente al excluido)
    std::vector<EnemyGenome::Ptr> validCandidates;
    for (const auto& genome : genomes) {
        if (genome->getType() == requiredType && genome->getId() != excludedId) {
            validCandidates.push_back(genome);
        }
    }

    // Si no hay candidatos válidos (excepto el excluido), genera uno nuevo
    if (validCandidates.empty()) {
        return generateEnemyGenome(requiredType);
    }

    // Separa los candidatos elite
    std::vector<EnemyGenome::Ptr> eliteCandidates;
    for (size_t i = 0; i < std::min(eliteCount, validCandidates.size()); ++i) {
        eliteCandidates.push_back(validCandidates[i]);
    }

    // 80% de probabilidad de seleccionar de la elite
    std::bernoulli_distribution eliteDist(0.8);
    if (!eliteCandidates.empty() && eliteDist(gen)) {
        std::uniform_int_distribution<size_t> dist(0, eliteCandidates.size()-1);
        return eliteCandidates[dist(gen)];
    }

    // Si no se selecciona elite, elige de toda la poblacion valida
    std::uniform_int_distribution<size_t> dist(0, validCandidates.size()-1);
    return validCandidates[dist(gen)];
}