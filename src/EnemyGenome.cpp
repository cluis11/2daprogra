#include "EnemyGenome.h"
#include <random>
#include <iostream>
#include <iomanip>

//para asignar IDs unicos a cada genoma
int EnemyGenome::nextId = 0;

EnemyGenome::EnemyGenome(EnemyType type, Attributes attrs)
    : m_type(type),
      m_attributes(attrs),
      m_fitness(0.0f),
      m_id(nextId++)
{
    // Resetea el contador si llega a un número muy grande
    if (nextId > 1000000) nextId = 0;

    std::cout << "Nuevo genoma creado - ID: " << m_id
              << " Tipo: " << static_cast<int>(m_type) << "\n";
}

EnemyGenome::Ptr EnemyGenome::crossoverUniform(const Ptr& parent1, const Ptr& parent2) {
    if (parent1->getType() != parent2->getType()) {
        throw std::runtime_error("Error: No se pueden cruzar tipos de enemigos diferentes");
    }

    static thread_local std::mt19937 gen(std::random_device{}());
    std::bernoulli_distribution dist(0.5);

    Attributes childAttrs;
    childAttrs.health = dist(gen) ? parent1->getAttributes().health : parent2->getAttributes().health;
    childAttrs.speed = dist(gen) ? parent1->getAttributes().speed : parent2->getAttributes().speed;
    childAttrs.armor = dist(gen) ? parent1->getAttributes().armor : parent2->getAttributes().armor;
    childAttrs.magicResist = dist(gen) ? parent1->getAttributes().magicResist : parent2->getAttributes().magicResist;
    childAttrs.stepsTaken = 0;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "[Cruce] Nuevo ID:" << nextId
              << "\n  Padre1 ID:" << parent1->getId()
              << " (H:" << parent1->getAttributes().health
              << ", S:" << parent1->getAttributes().speed
              << ", A:" << parent1->getAttributes().armor
              << ", MR:" << parent1->getAttributes().magicResist << ")"
              << "\n  Padre2 ID:" << parent2->getId()
              << " (H:" << parent2->getAttributes().health
              << ", S:" << parent2->getAttributes().speed
              << ", A:" << parent2->getAttributes().armor
              << ", MR:" << parent2->getAttributes().magicResist << ")"
              << "\n  Resultado: H:" << childAttrs.health
              << " S:" << childAttrs.speed
              << " A:" << childAttrs.armor
              << " MR:" << childAttrs.magicResist << "\n";

    return std::make_shared<EnemyGenome>(parent1->getType(), childAttrs);
}

void EnemyGenome::mutate(float mutationChance) {
    static thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);
    std::normal_distribution<float> mutationDist(0.0f, 0.2f);

    auto mutateAttr = [&](float& attr, const char* name) {
        if (chanceDist(gen) < mutationChance) {
            float original = attr;
            float mutation = mutationDist(gen);
            attr = std::max(0.1f, attr * (1.0f + mutation));

            std::cout << "[Mut] ID:" << m_id
                      << " " << name << ": " << original
                      << " -> " << attr
                      << " (" << (mutation > 0 ? "+" : "")
                      << std::abs(mutation)*100 << "%)\n";
        }
    };

    mutateAttr(m_attributes.health, "Health");
    mutateAttr(m_attributes.speed, "Speed");
    mutateAttr(m_attributes.armor, "Armor");
    mutateAttr(m_attributes.magicResist, "MR");
}