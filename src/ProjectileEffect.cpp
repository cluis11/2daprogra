#include "ProjectileEffect.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdint>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

ProjectileEffect::ProjectileEffect(const sf::Vector2f& start, const sf::Vector2f& end,
                                 sf::Color color, float speed,
                                 float width, float length)
    : m_start(start), m_end(end), m_currentPosition(start),
      m_color(color), m_speed(speed),
      m_width(width), m_length(length),
      m_traveled(0) {

    // Calcular distancia total
    sf::Vector2f diff = end - start;
    m_distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    // Configurar el rectángulo del proyectil
    m_projectile.setSize(sf::Vector2f(m_length, m_width));
    m_projectile.setFillColor(m_color);
    m_projectile.setOrigin(sf::Vector2f(m_length / 2.0f, m_width / 2.0f));

    float angleRadians = std::atan2(diff.y, diff.x);
    m_projectile.setRotation(sf::radians(angleRadians));
}

bool ProjectileEffect::isComplete() const {
    return m_traveled >= m_distance;
}

void ProjectileEffect::update(float deltaTime) {
    if (isComplete()) return;

    // Mueve el proyectil
    float moveAmount = m_speed * deltaTime;
    m_traveled = std::min(m_traveled + moveAmount, m_distance);

    // Calcula nueva posición
    sf::Vector2f direction = m_end - m_start;
    m_currentPosition = m_start + (direction * (m_traveled / m_distance));

    // Actualiza posición del proyectil
    m_projectile.setPosition(m_currentPosition);
}

void ProjectileEffect::draw(sf::RenderTarget& target) const {
    if (m_traveled > 0 && !isComplete()) {
        target.draw(m_projectile);
    }
}