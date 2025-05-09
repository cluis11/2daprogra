#pragma once
#include "Entity.h"
#include "EnemyGenome.h"
#include "EnemyTypes.h"

class Enemy : public Entity {
public:

    // Se usa el mismo enum que EnemyGenome para mantener consistencia en los tipos
    using Type = EnemyType;

    // Constructor que recibe tipo, posicion inicial y referencia al grid
    Enemy(Type type, int gridX, int gridY, GridSystem* grid);

    //Actualizacion del enemigo cada frame (heredado de Entity)
    void update(float deltaTime) override;
    void takeDamage(float amount); //para futura implementacion de ataque de torres
    void updateMovement(float deltaTime); //mueve al enemigo


    //Asigna un genoma al enemigo y aplica sus atributos
    void setGenome(const EnemyGenome& genome);
    int getStepsTaken() const { return m_stepsTaken; } //Para calculo de fitness

    //Getters
    Type getType() const { return m_type; }
    float getHealth() const { return m_health; }
    float getMaxHealth() const { return m_maxHealth; }
    float getSpeed() const { return m_speed; }
    bool isAlive() const { return m_health > 0; }
    //Getters para obtener valores de tipo especifico
    static sf::Color getColorForType(Type type);
    static float getDefaultHealth(Type type);
    static float getDefaultSpeed(Type type);

    void setRandomDeath(); //Metodo para determinar muerte aleatoria
private:
    bool m_shouldDie = false; //Flag para muerte aleatoria

    //Atributos
    Type m_type;
    float m_maxHealth;
    float m_health;
    float m_speed;
    //faltan resistencias
    float m_moveTimer = 0; //timer para movimiento

    int m_stepsTaken = 0; //Nuevo contador de pasos para calculo de fitness
    EnemyGenome m_genome; //Nuevo miembro
};