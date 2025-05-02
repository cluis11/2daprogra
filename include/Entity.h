#pragma once
#include <SFML/Graphics.hpp>

class GridSystem;

class Entity {
public:
    Entity(int gridX, int gridY, sf::Color color, GridSystem* grid);
    virtual ~Entity() = default;
    
    virtual void update(float deltaTime) = 0;
    void render(sf::RenderTarget& target) const;
    
    int getGridX() const { return m_gridX; }
    int getGridY() const { return m_gridY; }
    sf::FloatRect getBounds() const;

protected:
    int m_gridX, m_gridY;
    sf::Color m_color;
    sf::RectangleShape m_shape;
    GridSystem* m_gridSystem;
};