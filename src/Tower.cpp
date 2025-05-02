#include "Tower.h"

//Constructor de torre, varia atributos segun tipo
Tower::Tower(Type type, int gridX, int gridY, GridSystem* grid)
    : Entity(gridX, gridY, getColorForType(type), grid), 
    //asignacion valores segun tipo
    m_type(type),
    m_range(getDefaultDamage(type)),
    m_damage(getDefaultDamage(type)),
    m_attackSpeed(getDefaultAttackSpeed(type)) {}

void Tower::update(float deltaTime) {
    if (m_attackCooldown > 0) {
        m_attackCooldown -= deltaTime;
    }
    // logica de ataque
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