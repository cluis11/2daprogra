#include "Entity.h"
#include "GridSystem.h"

Entity::Entity(int gridX, int gridY, sf::Color color, GridSystem* grid) 
    : m_gridX(gridX), m_gridY(gridY), m_color(color), m_gridSystem(grid) {
    const float cellSize = m_gridSystem->getCellSize();
    m_shape.setSize({cellSize - 2, cellSize - 2});
    m_shape.setFillColor(color);
    m_shape.setPosition(m_gridSystem->gridToWorld(m_gridX, m_gridY));
}

void Entity::render(sf::RenderTarget& target) const {
    target.draw(m_shape);
}

sf::FloatRect Entity::getBounds() const {
    return m_shape.getGlobalBounds();
}