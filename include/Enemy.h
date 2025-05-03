#pragma once
#include "Entity.h"

class Enemy : public Entity {
public:
    //enum para tipo de enemigo
    enum class Type { Ogre, DarkElf, Harpy, Mercenary };

    Enemy(Type type, int gridX, int gridY, GridSystem* grid);
    void update(float deltaTime) override;
    void takeDamage(float amount); //para futura implementacion de ataque de torres
    void updateMovement(float deltaTime); //mueve al enemigo

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

private:
    //Atributos
    Type m_type;
    float m_maxHealth;
    float m_health;
    float m_speed;
    //faltan resistencias
    float m_moveTimer = 0; //timer para movimiento
};