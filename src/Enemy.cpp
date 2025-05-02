#include "Enemy.h"

//Constructor de enemigo, varia atributos segun tipo
Enemy::Enemy(Type type, int gridX, int gridY, GridSystem* grid)
    : Entity(gridX, gridY, getColorForType(type), grid),
    //asignacion valores segun tipo
    m_type(type),
    m_maxHealth(getDefaultHealth(type)),
    m_health(m_maxHealth),
    m_speed(getDefaultSpeed(type)) {}

void Enemy::update(float deltaTime) {
    (void)deltaTime; //lplace holder para evitar warning
    //Agregar logica de movimiento
}

void Enemy::takeDamage(float amount) {
    //falta agregar resistencias y tipo de daño o torre de la que proviene
    m_health -= amount;
    if (m_health < 0) m_health = 0;
}

//Funciones para obtener valores de cada tipo espeficico en el constructor

sf::Color Enemy::getColorForType(Type type) {
    switch(type) {
        case Type::Ogre: return sf::Color(128, 0, 128);     // Morado Oscuro
        case Type::DarkElf: return sf::Color(0, 255, 255);  // Cyan
        case Type::Harpy: return sf::Color(255, 215, 0);    // Amarillo Oro
        case Type::Mercenary: return sf::Color(112, 128, 144); // Gris Pizarra
        default: return sf::Color::White;
    }
}

float Enemy::getDefaultHealth(Type type) {
    switch(type) {
        case Type::Ogre: return 120.f;
        case Type::DarkElf: return 70.f;
        case Type::Harpy: return 90.f;
        case Type::Mercenary: return 150.f;
        default: return 0.f;
    }
}

float Enemy::getDefaultSpeed(Type type) {
    switch(type) {
        case Type::Ogre: return 45.f;
        case Type::DarkElf: return 130.f;
        case Type::Harpy: return 95.f;
        case Type::Mercenary: return 60.f;
        default: return 0.f;
    }
}