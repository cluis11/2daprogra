#include "Entity.h"
#include "GridSystem.h"

Entity::Entity(int gridX, int gridY, sf::Color color, GridSystem* grid) 
    : m_gridX(gridX), m_gridY(gridY), m_grid(grid) {
    float cellSize = grid->getCellSize();
    m_shape.setSize({cellSize - 2, cellSize - 2});
    m_shape.setOrigin({(cellSize - 2)/2, (cellSize - 2)/2});
    m_shape.setFillColor(color);
    m_shape.setPosition(grid->gridToWorld(gridX, gridY));
}

void Entity::render(sf::RenderTarget& target) const {
    target.draw(m_shape);
}

sf::Vector2f Entity::getPosition() const {
    // Calcula la posición exacta basada en el grid
    return m_grid->gridToWorld(m_gridX, m_gridY);
}

sf::FloatRect Entity::getBounds() const {
    return m_shape.getGlobalBounds();
}