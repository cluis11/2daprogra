#include "Tower.h"
#include "Enemy.h"
#include <iostream>

//Constructor de torre, varia atributos segun tipo
Tower::Tower(Type type, int gridX, int gridY, GridSystem* grid)
    : Entity(gridX, gridY, getColorForType(type), grid), 
    //asignacion valores segun tipo
    m_type(type),
    m_range(getDefaultRange(type)),
    m_damage(getDefaultDamage(type)),
    m_attackSpeed(getDefaultAttackSpeed(type)) {}

std::string Tower::typeToString(Type type) {
    static const std::string names[] = {"archer", "mage", "artillery"};
    return names[static_cast<int>(type)];
}

void Tower::update(float deltaTime) {
    m_attackTimer += deltaTime;
    if (m_attackTimer >= m_attackSpeed) {
        attackEnemy();
        m_attackTimer = 0.f;
    }
}

void Tower::upgrade() {
    if (m_level >= 3) return;
    
    m_level++;
    
    switch(m_type) {
        case Type::Archer: upgradeArcher(); break;
        case Type::Mage: upgradeMage(); break;
        case Type::Artillery: upgradeArtillery(); break;
    }
}


void Tower::upgradeArcher() {
    switch(m_level) {
        case 1:  // Nivel 1
            m_damage += 10;
            break;
        case 2:  // Nivel 2
            m_damage += 5;
            m_attackSpeed *= 0.9f;  // 10% más rápido
            break;
        case 3:  // Nivel 3 (máximo)
            m_damage += 10;
            m_attackSpeed *= 0.9f; 
            m_range += 1;
            break;
    }
}

void Tower::upgradeMage() {
    switch(m_level) {
        case 1:
            m_damage += 10;
            m_attackSpeed *= 0.9f; 
            break;
        case 2:
            m_damage += 15;
            m_attackSpeed *= 0.9f; 
            break;
        case 3:
            m_damage += 20;
            m_range += 1;
            break;
    }
}

void Tower::upgradeArtillery() {
    switch(m_level) {
        case 1:
            m_damage += 15;
            break;
        case 2:
            m_damage += 25;
            m_range += 1;
            break;
        case 3:
            m_damage += 40;
            break;
    }
}

//Funciones para obtener valores de cada tipo espeficico en el constructor

sf::Color Tower::getColorForType(Type type) {
    switch(type) {
        case Type::Archer: return sf::Color(34, 139, 34);   // Verde Bosque
        case Type::Mage: return sf::Color(70, 130, 180);    // Azul Acero
        case Type::Artillery: return sf::Color(183, 65, 14);// Rojo Óxido
        default: return sf::Color::White;
    }
}

//Asigna el alcance de ataque de la torre
int Tower::getDefaultRange(Type type) {
    switch(type) {
        case Type::Archer: return 5;
        case Type::Mage: return 4;
        case Type::Artillery: return 3;
        default: return 0;
    }
}

//Asigna la cantidad de daño de la torre
float Tower::getDefaultDamage(Type type) {
    switch(type) {
        case Type::Archer: return 15.f;
        case Type::Mage: return 25.f;
        case Type::Artillery: return 40.f;
        default: return 0.f;
    }
}

//Asigna la velocidad de ataque
float Tower::getDefaultAttackSpeed(Type type) {
    switch(type) {
        case Type::Archer: return 0.7f;
        case Type::Mage: return 1.0f;
        case Type::Artillery: return 1.5f;
        default: return 0.f;
    }
}


void Tower::attackEnemy() {
    auto enemies = m_grid->getEnemiesInRadius(m_gridX, m_gridY, m_range);
    if (!enemies.empty()) {
        // Ordenar enemigos por vida (ascendente: el de menor vida primero)
        std::sort(enemies.begin(), enemies.end(), [](Enemy* a, Enemy* b) {
            return a->getHealth() < b->getHealth();
        });
        // Atacar al primero (menor vida)
        enemies[0]->takeDamage(m_damage, typeToString(m_type));
    }
}



