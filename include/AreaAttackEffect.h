#pragma once
#include <SFML/Graphics.hpp>

class AreaAttackEffect {
public:
    AreaAttackEffect(const sf::Vector2f& center, float maxRadius);

    bool isComplete() const;
    void update(float deltaTime);
    void draw(sf::RenderTarget& target) const;

private:
    sf::CircleShape m_circle;
    float m_maxRadius;
    float m_currentRadius = 0.f;
    float m_duration = 0.5f; // Duración en segundos
    float m_currentTime = 0.f;
};