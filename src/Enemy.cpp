#include "Enemy.h"
#include "GridSystem.h"
#include <iostream>

// Constructor de enemigo, varia atributos segun tipo
Enemy::Enemy(EnemyType type, int gridX, int gridY, GridSystem* grid)
    : Entity(gridX, gridY, getColorForType(type), grid),
      m_type(type),
      m_maxHealth(getDefaultHealth(type)),
      m_health(m_maxHealth),
      m_speed(getDefaultSpeed(type)),
      m_moveTimer(0),
      m_stepsTaken(0),
      m_genome(EnemyGenome(type, {m_maxHealth, m_speed, 0.0f, 0.0f, 0})) // Inicializa m_genome
{}

void Enemy::update(float deltaTime) {
    updateMovement(deltaTime);
}

void Enemy::takeDamage(float amount) {
    // falta agregar resistencias y tipo de daño o torre de la que proviene
    m_health -= amount;
    if (m_health < 0) m_health = 0;
}

//Implementación de setGenome para asignar el genoma al enemigo
void Enemy::setGenome(const EnemyGenome& genome) {
    m_genome = genome; //Guarda el genoma completo
    // Aplicar atributos del genoma al enemigo
    m_maxHealth = genome.getAttributes().health;
    m_health = m_maxHealth;
    m_speed = genome.getAttributes().speed;
}

void Enemy::setRandomDeath() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::bernoulli_distribution dist(0.1); // 10% de chance

    m_shouldDie = dist(gen);
}

void Enemy::updateMovement(float deltaTime) {
    m_moveTimer += deltaTime;

    if (m_moveTimer >= m_speed) {
        setRandomDeath(); // Verificar muerte en cada paso

        if (m_shouldDie) {
            m_health = 0; // Marcar para morir
            return;
        }

        int newY = m_gridY + 1;
        if (m_grid->getCell(m_gridX, newY) == CellType::Empty) {
            m_grid->setCell(m_gridX, m_gridY, CellType::Empty);
            m_gridY = newY;
            m_grid->setCell(m_gridX, m_gridY, CellType::Enemy);
            m_shape.setPosition(m_grid->gridToWorld(m_gridX, m_gridY));
            m_stepsTaken++;
        }
        m_moveTimer = 0;
    }
}

// Funciones para obtener valores de cada tipo específico

sf::Color Enemy::getColorForType(EnemyType type) {
    switch(type) {
        case EnemyType::Ogre:      return sf::Color(128, 0, 128);    // Morado Oscuro
        case EnemyType::DarkElf:   return sf::Color(0, 255, 255);    // Cyan
        case EnemyType::Harpy:     return sf::Color(255, 215, 0);    // Amarillo Oro
        case EnemyType::Mercenary: return sf::Color(112, 128, 144);  // Gris Pizarra
        default:                   return sf::Color::White;
    }
}

float Enemy::getDefaultHealth(EnemyType type) {
    switch(type) {
        case EnemyType::Ogre:      return 120.f;
        case EnemyType::DarkElf:   return 70.f;
        case EnemyType::Harpy:     return 90.f;
        case EnemyType::Mercenary: return 150.f;
        default:                   return 0.f;
    }
}

float Enemy::getDefaultSpeed(EnemyType type) {
    switch(type) {
        case EnemyType::Ogre:      return 1.2f;
        case EnemyType::DarkElf:   return 0.7f;
        case EnemyType::Harpy:     return 0.5f;
        case EnemyType::Mercenary: return 1.f;
        default:                   return 0.f;
    }
}