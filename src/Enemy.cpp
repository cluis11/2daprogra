#include "Enemy.h"
#include "GridSystem.h"
#include <iostream>

//Constructor de enemigo, varia atributos segun tipo
//registra al enemigo en el grid
Enemy::Enemy(const EnemyGenome::Ptr& genome, int gridX, int gridY, GridSystem* grid, std::vector<sf::Vector2i> path)
    : Entity(gridX, gridY, getColorForType(genome->getType()), grid),
    //asignacion valores segun tipo
    m_type(genome->getType()), // Establece tipo desde genoma
    m_genome(genome), //referencia al genoma
    m_health(genome->getAttributes().health),
    m_speed(genome->getAttributes().speed),
    m_resistances(getDefaultResistances(genome->getType())),
    m_currentPath(path),
    m_pathfinder(grid),
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

    // Efecto de parpadeo (rojo intermitente durante 0.3 segundos)
    if (m_isDamaged) {
        float elapsed = m_damageClock.getElapsedTime().asSeconds();
        if (elapsed < 0.3f) {
            // Alternar entre rojo y color original basado en tiempo (5 cambios por segundo)
            bool showRed = static_cast<int>(elapsed * 10) % 2 == 0;
            m_shape.setFillColor(showRed ? sf::Color::Red : getColorForType(m_type));
        } else {
            m_isDamaged = false;
            m_shape.setFillColor(getColorForType(m_type)); // Restaura el color original
        }
    }
}



void Enemy::takeDamage(float amount, const std::string& damageType) {
    float multiplier = 1.0f;
    if (damageType=="Archer"){ multiplier = m_resistances.arrows; }
    else if (damageType=="Archer"){ multiplier = m_resistances.magic; }
    else if (damageType=="Archer"){ multiplier = m_resistances.artillery; }
    
    m_health -= amount * std::max(0.0f, multiplier);
    if (m_health < 0) m_health = 0;

    // Activa el efecto visual
    m_isDamaged = true;
    m_damageClock.restart();
}

//hace que el enemigo se mueva
//hace que el enemigo se mueva
void Enemy::updateMovement(float deltaTime) {
    m_moveTimer += deltaTime;
    
    if (m_moveTimer >= m_speed) {
        if (!m_currentPath.empty()) {
            sf::Vector2i nextPos = m_currentPath.front();

            //Verificar que la celda de destino sea valida
            if (m_grid->getCell(nextPos.x, nextPos.y) == CellType::Empty ||
                m_grid->getCell(nextPos.x, nextPos.y) == CellType::ExitPoint ||
                m_grid->getCell(nextPos.x, nextPos.y) == CellType::Enemy) {
                
                //Actualizar posicion
                m_grid->unregisterEnemy(m_gridX, m_gridY);
                m_gridX = nextPos.x;
                m_gridY = nextPos.y;
                m_grid->registerEnemy(this, m_gridX, m_gridY);
                m_shape.setPosition(m_grid->gridToWorld(m_gridX, m_gridY));

                // Avanzar al siguiente paso del camino
                m_currentPath.erase(m_currentPath.begin());
            } else {
                //Camino bloqueado, limpiar para recalcular
                m_currentPath.clear();
                std::cout << "Choco contra una torre u obstaculo" << std::endl;
                auto path = m_pathfinder.findPath({ m_gridX, m_gridY });
                setPath(path);
            }
        } else {
            //Movimiento de emergencia
            int newX = m_gridX;
            int newY = m_gridY + 1;

            if (m_grid->getCell(newX, newY) == CellType::Empty) {
                m_grid->unregisterEnemy(m_gridX, m_gridY);
                m_gridX = newX;
                m_gridY = newY;
                m_grid->registerEnemy(this, m_gridX, m_gridY);
                m_shape.setPosition(m_grid->gridToWorld(m_gridX, m_gridY));
            }
        }
        // Reiniciar el temporizador de movimiento
        m_moveTimer = 0;
    }
}

//Implementacion de setGenome para asignar el genoma al enemigo
void Enemy::setGenome(const EnemyGenome::Ptr& genome) {
    m_genome = genome; // Actualiza referencia al genoma
    m_type = genome->getType(); // Actualiza tipo
    m_health = genome->getAttributes().health; // Restablece salud
    m_speed = genome->getAttributes().speed;
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
        case Type::Ogre: return 1.5f;
        case Type::DarkElf: return 1.f;
        case Type::Harpy: return 0.7f;
        case Type::Mercenary: return 1.2f;
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