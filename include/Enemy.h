#pragma once
#include "Entity.h"
#include "EnemyGenome.h"
#include <memory>

class Enemy : public Entity {
public:
    // Se usa el mismo enum que EnemyGenome para mantener consistencia en los tipos
    using Type = EnemyType;

    // Constructor que recibe un genoma compartido, posicion en grid y referencia al sistema de grid
    Enemy(const EnemyGenome::Ptr& genome, int gridX, int gridY, GridSystem* grid);

    //Actualizacion del enemigo cada frame (heredado de Entity)
    void update(float deltaTime) override;
    void takeDamage(float amount);
    void updateMovement(float deltaTime);

    //Asigna un genoma al enemigo y aplica sus atributos
    void setGenome(const EnemyGenome::Ptr& genome); // Asigna un nuevo genoma
    float getStepsTaken() const { return static_cast<float>(m_stepsTaken); } // Pasos dados

    //Getters
    Type getType() const { return m_type; }
    float getHealth() const { return m_health; }
    float getArmor() const { return m_genome->getAttributes().armor; }
    float getMagicResist() const { return m_genome->getAttributes().magicResist; }

    //Getters para obtener valores de tipo especifico
    bool isAlive() const { return m_health > 0; }
    const EnemyGenome::Ptr& getGenome() const { return m_genome; }
    float getSpeed() const { return m_genome->getAttributes().speed; }

    static sf::Color getColorForType(Type type); // Color visual segun tipo
    void setRandomDeath(); //Metodo para determinar muerte aleatoria

private:
    bool m_shouldDie = false; // Flag para muerte aleatoria
    Type m_type; // Tipo de enemigo
    float m_health;
    float m_moveTimer;
    int m_stepsTaken;
    EnemyGenome::Ptr m_genome; // Puntero compartido al genoma
};