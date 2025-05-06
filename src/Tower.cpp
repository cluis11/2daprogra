#include "Tower.h"
#include "Enemy.h"
#include <iostream>

//Constructor de torre, varia atributos segun tipo
Tower::Tower(Type type, int gridX, int gridY, GridSystem* grid)
    : Entity(gridX, gridY, getColorForType(type), grid), 
    //asignacion valores segun tipo
    m_type(type),
    m_range(getDefaultDamage(type)),
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
float Tower::getDefaultRange(Type type) {
    switch(type) {
        case Type::Archer: return 5.0f;
        case Type::Mage: return 3.5f;
        case Type::Artillery: return 2.5f;
        default: return 0.f;
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
        case Type::Archer: return 1.5f;
        case Type::Mage: return 1.0f;
        case Type::Artillery: return 0.5f;
        default: return 0.f;
    }
}


void Tower::attackEnemy() {
    auto enemies = m_grid->getEnemiesInRadius(m_gridX, m_gridY, static_cast<int>(m_range));
    if (!enemies.empty()) {
        // Por ahora ataca al primer enemigo de la lista
        enemies[0]->takeDamage(m_damage, typeToString(m_type));
        std::cout << "enemigo atacado";
    }
}



