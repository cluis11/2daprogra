
#pragma once
#include <SFML/Graphics.hpp>


class ProjectileEffect {
public:
    ProjectileEffect(const sf::Vector2f& start, const sf::Vector2f& end,
                   sf::Color color, float speed = 600.0f,
                   float width = 2.0f, float length = 8.0f);
    bool isComplete() const;
    void update(float deltaTime);
    void draw(sf::RenderTarget& target) const;

private:
    sf::RectangleShape m_projectile;
    sf::Vector2f m_start;
    sf::Vector2f m_end;
    sf::Vector2f m_currentPosition;
    sf::Color m_color;
    float m_speed;
    float m_width;
    float m_length;
    float m_distance;
    float m_traveled;
};