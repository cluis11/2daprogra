#pragma once
#include "Entity.h"
#include "GridSystem.h"



class Enemy : public Entity {
public:
    //enum para tipo de enemigo
    enum class Type { Ogre, DarkElf, Harpy, Mercenary };

    //struct de resistencias
    struct Resistances {
        float arrows;    // Resistencia a flechas (Archer)
        float magic;     // Resistencia a magia (Mage)
        float artillery; // Resistencia a artillería (Artillery)
    };

    Enemy(Type type, int gridX, int gridY, GridSystem* grid, std::vector<sf::Vector2i> m_currentPath);
    ~Enemy();

    void update(float deltaTime) override;
    void takeDamage(float amount, const std::string& damageType); //para futura implementacion de ataque de torres
    void updateMovement(float deltaTime); //mueve al enemigo
    bool isAlive() const { return m_health > 0; }

    //Getters
    Type getType() const { return m_type; }
    float getHealth() const { return m_health; }
    float getMaxHealth() const { return m_maxHealth; }
    float getSpeed() const { return m_speed; }
    const std::vector<sf::Vector2i>& getCurrentPath() const { return m_currentPath; }
    const Resistances& getResistances() const { return m_resistances; }
    //Getters para obtener valores de tipo especifico
    static sf::Color getColorForType(Type type);
    static float getDefaultHealth(Type type);
    static float getDefaultSpeed(Type type);
    static Resistances getDefaultResistances(Type type);
    //getters colision
    const sf::FloatRect& getGlobalBounds() const { return m_shape.getGlobalBounds(); }
    
    void setPath(std::vector<sf::Vector2i> path) { m_currentPath = path; };

private:
    //Atributos
    Type m_type;
    float m_maxHealth;
    float m_health;
    float m_speed;
    Resistances m_resistances;
    float m_moveTimer = 0; //timer para movimiento
    int m_prevGridX, m_prevGridY; //coordenadas anteriores para facilitar actualizaciones a matrices
    std::vector<sf::Vector2i> m_currentPath; //Camino del enemigo
};