#pragma once
#include "Entity.h"
#include <string>
#include <map>

class Tower : public Entity {
public:
    //enum para tipos de clases
    enum class Type { Archer, Mage, Artillery };

    Tower(Type type, int gridX, int gridY, GridSystem* grid);

    static int getCost(Type type) {
        static const std::map<Type, int> costs = {
            {Type::Archer, 50},
            {Type::Mage, 75},
            {Type::Artillery, 100}
        };
        return costs.at(type);
    }

    static int getUpgradeCost(int level) {
        static const std::map<int, int> costs = {
            {1, 100},
            {2, 300},
            {3, 500}
        };
        return costs.at(level);
    }


    void update(float deltaTime);
    static std::string typeToString(Type type);
    void attackEnemy();
    void upgrade(); 
    

    //Getters
    Type getType() const { return m_type; }
    int getRange() const { return m_range; }
    float getDamage() const { return m_damage; }
    float getAttackSpeed() const { return m_attackSpeed; }
    int getLevel() const { return m_level; }
    //Getters para obtener valores de tipo especifico
    static sf::Color getColorForType(Type type);
    static int getDefaultRange(Type type);
    static float getDefaultDamage(Type type);
    static float getDefaultAttackSpeed(Type type);



    private:
        //Atributos
        Type m_type;
        int m_range;
        float m_damage;
        float m_attackSpeed;
        float m_attackCooldown = 0.f;
        float m_attackTimer = 0.f;
        int m_level = 0;

        void upgradeArcher();
        void upgradeMage();
        void upgradeArtillery();
};