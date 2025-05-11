#include "Enemy.h"
#include "GridSystem.h"
#include <iostream>

// Constructor de enemigo, varia atributos segun tipo
Enemy::Enemy(const EnemyGenome::Ptr& genome, int gridX, int gridY, GridSystem* grid)
    : Entity(gridX, gridY, getColorForType(genome->getType()), grid),
      m_type(genome->getType()), // Establece tipo desde genoma
      m_genome(genome), //referencia al genoma
      m_health(genome->getAttributes().health), // Salud inicial del genoma
      m_moveTimer(0),
      m_stepsTaken(0)
{}

void Enemy::update(float deltaTime) {
    updateMovement(deltaTime);
}

//hay que modificar esto con el calculo de daño por el tipo de daño recibido  magico o fisico.
void Enemy::takeDamage(float amount) {
    // falta agregar resistencias y tipo de daño o torre de la que proviene
    m_health -= amount;
    if (m_health < 0) m_health = 0;
}

//Implementacion de setGenome para asignar el genoma al enemigo
void Enemy::setGenome(const EnemyGenome::Ptr& genome) {
    m_genome = genome; // Actualiza referencia al genoma
    m_type = genome->getType(); // Actualiza tipo
    m_health = genome->getAttributes().health; // Restablece salud
}

// Configura muerte aleatoria con 5% de probabilidad
void Enemy::setRandomDeath() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::bernoulli_distribution dist(0.05); // 5% de probabilidad

    m_shouldDie = dist(gen); // Asigna true/false aleatoriamente
}

void Enemy::updateMovement(float deltaTime) {
    m_moveTimer += deltaTime;
    float speed = m_genome->getAttributes().speed; //velocidad del genoma

    if (m_moveTimer >= 1.0f / speed) {
        // Movimiento en eje X+
        int newX = m_gridX + 1;

        //Verifica si la nueva pos es valida
        if (newX < static_cast<int>(m_grid->getWidth()) &&
            m_grid->getCell(newX, m_gridY) == CellType::Empty) {
            // Limpia la celda anterior
            m_grid->setCell(m_gridX, m_gridY, CellType::Empty);

            // Actualiza la pos
            m_gridX = newX;
            m_grid->setCell(m_gridX, m_gridY, CellType::Enemy);
            // Actualizar la posicion visualmente
            m_shape.setPosition(m_grid->gridToWorld(m_gridX, m_gridY));
            m_stepsTaken++;
            }

        // Muerte aleatoria con probabilidad basada en salud
        if (m_health > 0) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::bernoulli_distribution dist(0.02); // 2% de chance por paso
            if (dist(gen)) {
                m_health = 0; // Mata al enemigo
            }
        }

        m_moveTimer = 0; // Reinicia temporizador
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
