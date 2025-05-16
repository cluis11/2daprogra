#include "EnemyGenome.h"
#include <random>
#include <iostream>
#include <iomanip>

//para asignar IDs unicos a cada genoma
int EnemyGenome::nextId = 0;

// Constructor - Inicializa un genoma con tipo y atributos especificos
EnemyGenome::EnemyGenome(EnemyType type, Attributes attrs)
    : m_type(type),         // Inicializa el tipo de enemigo
      m_attributes(attrs),  // Copia los atributos proporcionados
      m_fitness(0.0f),      // Fitness inicial en 0
      m_id(nextId++)      // Asigna ID y incrementa contador
    {
        // Resetea el contador si llega a un número muy grande
        if (nextId > 1000000) nextId = 0;

        std::cout << "Nuevo genoma creado - ID: " << m_id
                << " Tipo: " << static_cast<int>(m_type) << "\n";
    }

/* Realiza cruce uniforme entre dos genomas padres
* Funcionamiento:
* 1. Verifica compatibilidad de tipos
* 2. Para cada atributo, elige aleatoriamente (50%) de cual padre heredar
* 3. Muestra en consola los detalles del cruce
* 4. Retorna nuevo genoma con atributos mezclados
*/
EnemyGenome::Ptr EnemyGenome::crossoverUniform(const Ptr& parent1, const Ptr& parent2) {
    // ambos padres deben ser del mismo tipo
    if (parent1->getType() != parent2->getType()) {
        throw std::runtime_error("Error: No se pueden cruzar tipos de enemigos diferentes");
    }

    static thread_local std::mt19937 gen(std::random_device{}());
    std::bernoulli_distribution dist(0.5); // Distribucion para elegir entre padres (50/50)

    // Atributos del hijo
    Attributes childAttrs;
    // Seleccion aleatoria para cada atributo:
    childAttrs.health = dist(gen) ? parent1->getAttributes().health : parent2->getAttributes().health;
    childAttrs.speed = dist(gen) ? parent1->getAttributes().speed : parent2->getAttributes().speed;
    childAttrs.armor = dist(gen) ? parent1->getAttributes().armor : parent2->getAttributes().armor;
    childAttrs.magicResist = dist(gen) ? parent1->getAttributes().magicResist : parent2->getAttributes().magicResist;
    childAttrs.stepsTaken = 0;// Reinicia contador de pasos del hijo


    // Configurar formato de salida
    std::cout << std::fixed << std::setprecision(2);

    // Log detallado del cruce genetico
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

    // Crea y retorna nuevo genoma hijo
    return std::make_shared<EnemyGenome>(parent1->getType(), childAttrs);
}


/*
 * Aplica mutacionesd de forma aleatorias a los atributos del genoma

 * Funcionamiento:
 * 1. Para cada atributo, decide si muta basado en mutationChance(que va de 0 a 1)
 * 2. Las mutaciones son cambios porcentuales (+/- 20% normalmente)
 * 3. Los atributos nunca caen debajo de 0.1
 * 4. Muestra en consola los cambios realizados
 */
void EnemyGenome::mutate(float mutationChance) {
    // Generador de numeros aleatorios por hilo (thread-safe)
    static thread_local std::mt19937 gen(std::random_device{}());

    // Distribucion para decidir si hay mutacion
    std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);

    // Distribucion normal para magnitud de mutacion (media 0, desv 0.2)
    std::normal_distribution<float> mutationDist(0.0f, 0.2f);

    // Configurar formato de salida
    std::cout << std::fixed << std::setprecision(2);

    // Funcion lambda para mutar un atributo individual
    auto mutateAttr = [&](float& attr, const char* name) {
        // Decide si este atributo en particular muta
        if (chanceDist(gen) < mutationChance) {
            float original = attr; // Guarda valor original
            float mutation = mutationDist(gen); // Valor de mutacion (-0.2 a +0.2 aprox)
            attr = std::max(0.1f, attr * (1.0f + mutation));  // Aplica mutacion (multiplicativa) con valor minimo 0.1


            // Muestra informacion de la mutacion
            std::cout.precision(2); // Solo 2 decimales para porcentaje
            std::cout << "[Mut] ID:" << m_id
                      << " " << name << ": " << original
                      << " -> " << attr
                      << " (" << (mutation > 0 ? "+" : "")
                      << std::abs(mutation)*100 << "%)\n"; // Valor absoluto para porcentaje
            std::cout.precision(6); // Restaurar precision por defecto
        }
    };

    // Aplica posible mutacion a cada atributo
    mutateAttr(m_attributes.health, "Health");
    mutateAttr(m_attributes.speed, "Speed");
    mutateAttr(m_attributes.armor, "Armor");
    mutateAttr(m_attributes.magicResist, "MR");
}