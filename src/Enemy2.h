/*#pragma once
#include "Entity.h"

enum class EnemyType { Ogre, DarkElf, Harpy, Mercenary };

class Enemy : public Entity {
public:
    Enemy(EnemyType type, sf::Vector2f position)
        : Entity(position, getColorForType(type)), m_type(type) {
        initAttributes();
    }
    
    void update(float deltaTime) override {
        (void)deltaTime;
        // Movement logic would go here
    }
    
    void takeDamage(float amount) {
        m_health -= amount;
    }
    
    bool isAlive() const { return m_health > 0; }

private:
    void initAttributes() {
        switch(m_type) {
            case EnemyType::Ogre:
                m_health = 100.f;
                m_speed = 50.f;
                break;
            case EnemyType::DarkElf:
                m_health = 60.f;
                m_speed = 120.f;
                break;
            case EnemyType::Harpy:
                m_health = 80.f;
                m_speed = 90.f;
                break;
            case EnemyType::Mercenary:
                m_health = 120.f;
                m_speed = 70.f;
                break;
        }
    }
    
    static sf::Color getColorForType(EnemyType type) {
        switch(type) {
            case EnemyType::Ogre: return sf::Color(128, 0, 0); // Dark red
            case EnemyType::DarkElf: return sf::Color::Magenta;
            case EnemyType::Harpy: return sf::Color::Cyan;
            case EnemyType::Mercenary: return sf::Color::Yellow;
            default: return sf::Color::White;
        }
    }
    
    EnemyType m_type;
    float m_health;
    float m_speed;
};*/