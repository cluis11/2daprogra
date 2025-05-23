#pragma once
#include "Entity.h"
#include "GridSystem.h"
#include "PathFinding.h"
#include <map>
#include "EnemyGenome.h"



class Enemy : public Entity {
public:
    //enum para tipo de enemigo
    using Type = EnemyType;

    //struct de resistencias
    struct Resistances {
        float arrows;    // Resistencia a flechas (Archer)
        float magic;     // Resistencia a magia (Mage)
        float artillery; // Resistencia a artillería (Artillery)
    };

    Enemy(const EnemyGenome::Ptr& genome, int gridX, int gridY, GridSystem* grid, std::vector<sf::Vector2i> m_currentPath);
    ~Enemy();

    int getBounty() const { 
        static const std::map<Type, int> bounties = {
            {Type::Ogre, 20},
            {Type::DarkElf, 30},
            {Type::Harpy, 40},
            {Type::Mercenary, 50}
        };
        return bounties.at(m_type);
    }

    void update(float deltaTime) override;
    void takeDamage(float amount, const std::string& damageType); //para futura implementacion de ataque de torres
    void updateMovement(float deltaTime); //mueve al enemigo
    bool isAlive() const { return m_health > 0; }
    bool EndGame() const { return m_end; }

    //Asigna un genoma al enemigo y aplica sus atributos
    void setGenome(const EnemyGenome::Ptr& genome); // Asigna un nuevo genoma
    float getStepsTaken() const { return static_cast<float>(m_stepsTaken); } // Pasos dados

    //Getters
    Type getType() const { return m_type; }
    float getHealth() const { return m_health; }
    float getMaxHealth() const { return m_maxHealth; } //revisar si se usa
    float getSpeed() const { return m_speed; }
    const std::vector<sf::Vector2i>& getCurrentPath() const { return m_currentPath; }
    const Resistances& getResistances() const { return m_resistances; }
    //Getters para obtener valores de tipo especifico, puede que ya no se ocupen
    static sf::Color getColorForType(Type type);
    static float getDefaultHealth(Type type);
    static float getDefaultSpeed(Type type);

    static Resistances getDefaultResistances(Type type);
    const EnemyGenome::Ptr& getGenome() const { return m_genome; }
    //getters colision
    const sf::FloatRect& getGlobalBounds() const { return m_shape.getGlobalBounds(); }
    //Set para otorgar un nuevo Path
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

    int m_stepsTaken = 0;
    EnemyGenome::Ptr m_genome; // Puntero compartido al genoma

    std::vector<sf::Vector2i> m_currentPath; //Camino del enemigo
    bool m_end;
    PathFinding m_pathfinder;

    sf::Clock m_damageClock;  // Temporizador para el efecto
    bool m_isDamaged = false; // Estado de "recibiendo daño"
};