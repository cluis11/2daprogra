#include "Enemy.h"
#include "GridSystem.h"
#include <iostream>

//Constructor de enemigo, varia atributos segun tipo
//registra al enemigo en el grid
Enemy::Enemy(Type type, int gridX, int gridY, GridSystem* grid)
    : Entity(gridX, gridY, getColorForType(type), grid),
    //asignacion valores segun tipo
    m_type(type),
    m_maxHealth(getDefaultHealth(type)),
    m_health(m_maxHealth),
    m_speed(getDefaultSpeed(type)),
    m_resistances(getDefaultResistances(type)),
    m_prevGridX(gridX),
    m_prevGridY(gridY)  {
        m_grid->registerEnemy(this, gridX, gridY);
    }

//cuando se destruye limpia las matrices
Enemy::~Enemy() {
    m_grid->unregisterEnemy(m_gridX, m_gridY);
}

void Enemy::update(float deltaTime) {
    updateMovement(deltaTime);
}

void Enemy::takeDamage(float amount, const std::string& damageType) {
    float multiplier = 1.0f;
    if (damageType=="Archer"){ multiplier = m_resistances.arrows; }
    else if (damageType=="Archer"){ multiplier = m_resistances.magic; }
    else if (damageType=="Archer"){ multiplier = m_resistances.artillery; }
    
    m_health -= amount * std::max(0.0f, multiplier);
    if (m_health < 0) m_health = 0;
}

//hace que el enemigo se mueva
//por ahora solo lo mueve hacia abajo, con pathfinder se elije donde
void Enemy::updateMovement(float deltaTime) {
    m_moveTimer += deltaTime;
    //cuando el timer sea el valor de la velocidad del enemigo
    if (m_moveTimer >= m_speed) {
        int newX = m_gridX;
        //test mover hacia abajo
        int newY = m_gridY + 1;
        //valida si la celda a moverse es valida
        if (m_grid->getCell(newX, newY) == CellType::Empty) {
            // Hace la pos actual Empty en la matriz
            m_grid->unregisterEnemy(m_gridX, m_gridY);
            m_gridX = newX;
            m_gridY = newY;
            m_grid->registerEnemy(this, m_gridX, m_gridY);
            //mueve el enemigo en la pantalla
            m_shape.setPosition(m_grid->gridToWorld(m_gridX, m_gridY));
        }
        
        m_moveTimer = 0; // hace reset al contador de tiempo
    }
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
        case Type::Ogre: return 1.2f;
        case Type::DarkElf: return 0.7f;
        case Type::Harpy: return 0.5f;
        case Type::Mercenary: return 1.f;
        default: return 0.f;
    }
}

// Función para obtener resistencias según tipo
Enemy::Resistances Enemy::getDefaultResistances(Type type) {
    switch(type) {
        case Type::Ogre:
            return {0.5f, 1.5f, 1.5f}; // Resistente a flechas, débil a magia/artillería
        case Type::DarkElf:
            return {1.5f, 0.5f, 1.5f}; // Resistente a magia, débil a flechas/artillería
        case Type::Harpy:
            return {1.0f, 1.0f, 0.0f}; // Normal contra flechas/magia, inmune a artillería
        case Type::Mercenary:
            return {0.5f, 1.5f, 0.5f}; // Resistente a flechas/artillería, débil a magia
        default:
            return {1.0f, 1.0f, 1.0f};
    }
}