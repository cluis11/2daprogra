#pragma once
#include "EnemyTypes.h"
#include <string>
#include <sstream>
#include <memory>

class EnemyGenome {
public:
    //Contiene todos los atributos geneticos
    struct Attributes {
        float health;
        float speed;
        float armor;
        float magicResist;
        int stepsTaken;

    };

    using Ptr = std::shared_ptr<EnemyGenome>; //puntero compartido

    // Constructor que crea un genoma con tipo y atributos especificos
    EnemyGenome(EnemyType type, Attributes attrs);

    // Realiza cruce uniforme entre dos genomas padres
    static Ptr crossoverUniform(const Ptr& parent1, const Ptr& parent2);
    void mutate(float mutationChance); // Aplica mutaciones aleatorias al genoma

    // Getters
    EnemyType getType() const { return m_type; }
    const Attributes& getAttributes() const { return m_attributes; } // Todos los atributos
    float getFitness() const { return m_fitness; }
    int getId() const { return m_id; } // ID unico
    static void resetIdCounter() { nextId = 0; }

    // Setters
    void setFitness(float fitness) { m_fitness = fitness; }
    void setStepsTaken(int steps) { m_attributes.stepsTaken = steps; }




private:
    EnemyType m_type;
    Attributes m_attributes; // Atributos geneticos
    float m_fitness = 0.0f; // Valor de aptitud para seleccion natural
    int m_id;  // ID unico para cada genoma
    static int nextId;  // Contador para IDs

};