#pragma once
#include "Entity.h"
#include <string>

class Tower : public Entity {
public:
    //enum para tipos de clases
    enum class Type { Archer, Mage, Artillery };

    Tower(Type type, int gridX, int gridY, GridSystem* grid);
    void update(float deltaTime);
    static std::string typeToString(Type type);
    void attackEnemy();
    

    //Getters
    Type getType() const { return m_type; }
    float getRange() const { return m_range; }
    float getDamage() const { return m_damage; }
    float getAttackSpeed() const { return m_attackSpeed; }
    //Getters para obtener valores de tipo especifico
    static sf::Color getColorForType(Type type);
    static float getDefaultRange(Type type);
    static float getDefaultDamage(Type type);
    static float getDefaultAttackSpeed(Type type);



    private:
        //Atributos
        Type m_type;
        float m_range;
        float m_damage;
        float m_attackSpeed;
        float m_attackCooldown = 0.f;
        float m_attackTimer = 0.f;
};