#pragma once
#include "EnemyTypes.h"

class EnemyGenome {
public:
    struct Attributes {
        float health;
        float speed;
        float armor;
        float magicResist;
        int stepsTaken;
    };

    EnemyGenome(EnemyType type, Attributes attrs);

    static EnemyGenome crossoverUniform(const EnemyGenome& parent1, const EnemyGenome& parent2);
    void mutate(float mutationChance);

    EnemyType getType() const { return m_type; }
    const Attributes& getAttributes() const { return m_attributes; }
    float getFitness() const { return m_fitness; }
    void setFitness(float fitness) { m_fitness = fitness; }


private:
    EnemyType m_type;
    Attributes m_attributes;
    float m_fitness = 0.0f;
};