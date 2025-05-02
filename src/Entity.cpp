#include "Entity.h"
#include "GridSystem.h"

Entity::Entity(int gridX, int gridY, sf::Color color, GridSystem* grid) 
    : m_gridX(gridX), m_gridY(gridY), m_grid(grid) {
    float cellSize = grid->getCellSize();
    m_shape.setSize({cellSize - 4, cellSize - 4});
    m_shape.setFillColor(color);
    m_shape.setPosition(grid->gridToWorld(gridX, gridY));
}

void Entity::render(sf::RenderTarget& target) const {
    target.draw(m_shape);
}

sf::FloatRect Entity::getBounds() const {
    return m_shape.getGlobalBounds();
}