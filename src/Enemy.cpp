#include "Enemy.h"
#include "GridSystem.h"
#include <iostream>

//Constructor de enemigo, varia atributos segun tipo
Enemy::Enemy(Type type, int gridX, int gridY, GridSystem* grid)
    : Entity(gridX, gridY, getColorForType(type), grid),
    //asignacion valores segun tipo
    m_type(type),
    m_maxHealth(getDefaultHealth(type)),
    m_health(m_maxHealth),
    m_speed(getDefaultSpeed(type)) {}

void Enemy::update(float deltaTime) {
    updateMovement(deltaTime);
}

void Enemy::takeDamage(float amount) {
    //falta agregar resistencias y tipo de daño o torre de la que proviene
    m_health -= amount;
    if (m_health < 0) m_health = 0;
}

//hace que el enemigo se mueva
//por ahora solo lo mueve hacia abajo, con pathfinder se elije donde
void Enemy::updateMovement(float deltaTime) {
    m_moveTimer += deltaTime;
    //cuando el timer sea el valor de la velocidad del enemigo
    if (m_moveTimer >= m_speed) {
        //test mover hacia abajo
        int newY = m_gridY + 1;
        std::cout << "[updatemovement]" << std::endl;
        //valida si la celda a moverse es valida
        if (m_grid->getCell(m_gridX, newY) == CellType::Empty) {
            // Hace la pos actual Empty en la matriz
            m_grid->setCell(m_gridX, m_gridY, CellType::Empty);
            
            // Mueve al enemigo en la matriz
            m_gridY = newY;
            m_grid->setCell(m_gridX, m_gridY, CellType::Enemy);
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