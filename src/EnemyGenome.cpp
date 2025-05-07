#include "EnemyGenome.h"
#include <random>

// Constructor
EnemyGenome::EnemyGenome(EnemyType type, Attributes attrs)
    : m_type(type), m_attributes(attrs), m_fitness(0.0f) {}

// Operación de cruce uniforme
EnemyGenome EnemyGenome::crossoverUniform(const EnemyGenome& parent1, const EnemyGenome& parent2) {
    // Solo cruzamos si son del mismo tipo
    if (parent1.m_type != parent2.m_type) {
        throw std::runtime_error("Cannot crossover different enemy types");
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 1);

    EnemyGenome::Attributes childAttrs;

    // Cruce uniforme para cada atributo
    childAttrs.health = dist(gen) ? parent1.m_attributes.health : parent2.m_attributes.health;
    childAttrs.speed = dist(gen) ? parent1.m_attributes.speed : parent2.m_attributes.speed;
    childAttrs.armor = dist(gen) ? parent1.m_attributes.armor : parent2.m_attributes.armor;
    childAttrs.magicResist = dist(gen) ? parent1.m_attributes.magicResist : parent2.m_attributes.magicResist;
    childAttrs.stepsTaken = 0; // Resetear para el nuevo enemigo

    return EnemyGenome(parent1.m_type, childAttrs);
}

// Operación de mutación
void EnemyGenome::mutate(float mutationChance) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    std::normal_distribution<float> mutationDist(1.0f, 0.1f); // Distribución normal para mutaciones

    auto mutateValue = [&](float& value) {
        if (dist(gen) < mutationChance) {
            value *= mutationDist(gen); // Multiplicar por factor aleatorio
        }
    };

    // Aplicar posible mutación a cada atributo
    mutateValue(m_attributes.health);
    mutateValue(m_attributes.speed);
    mutateValue(m_attributes.armor);
    mutateValue(m_attributes.magicResist);
}