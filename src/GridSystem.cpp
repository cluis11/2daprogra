#include "GridSystem.h"
#include <cstdlib>
#include <algorithm>

GridSystem::GridSystem(unsigned width, unsigned height, float cellSize)
    : m_width(width), m_height(height), m_cellSize(cellSize) {
    m_grid.resize(m_width, std::vector<CellType>(m_height, CellType::Empty));
    initializeGridWithObstacles();
}

void GridSystem::initializeGridWithObstacles() {
    // Create borders
    for (unsigned x = 0; x < m_width; x++) {
        for (unsigned y = 0; y < m_height; y++) {
            // Border walls
            if (x == 0 || y == 0 || x == m_width-1 || y == m_height-1) {
                m_grid[x][y] = CellType::Obstacle;
            }
            // Random obstacles (10% of cells)
            else if (rand() % 10 == 0) {
                m_grid[x][y] = CellType::Obstacle;
            }
        }
    }

    // Create path from entry to exit
    unsigned midY = m_height / 2;
    for (unsigned x = 0; x < m_width; x++) {
        m_grid[x][midY] = (x == 0) ? CellType::EntryPoint : 
                          (x == m_width-1) ? CellType::ExitPoint : 
                          CellType::Path;
    }
}

CellType GridSystem::getCell(int x, int y) const {
    if (x < 0 || y < 0 || x >= static_cast<int>(m_width) || y >= static_cast<int>(m_height))
        return CellType::Obstacle;
    return m_grid[x][y];
}

bool GridSystem::setCell(int x, int y, CellType type) {
    if (x < 0 || y < 0 || x >= static_cast<int>(m_width) || y >= static_cast<int>(m_height))
        return false;
    m_grid[x][y] = type;
    return true;
}


sf::Vector2f GridSystem::gridToWorld(int x, int y) const {
    return sf::Vector2f(
        x * m_cellSize + m_cellSize / 2.f,
        y * m_cellSize + m_cellSize / 2.f
    );
}

sf::Vector2i GridSystem::worldToGrid(float x, float y) const {
    return sf::Vector2i(
        static_cast<int>(x / m_cellSize),
        static_cast<int>(y / m_cellSize)
    );
}

sf::Vector2i GridSystem::getEntryPoint() const {
    for (unsigned x = 0; x < m_width; x++)
        for (unsigned y = 0; y < m_height; y++)
            if (m_grid[x][y] == CellType::EntryPoint)
                return sf::Vector2i(x, y);
    return sf::Vector2i(0, 0);
}

void GridSystem::render(sf::RenderTarget& target) const {
    // Construye la forma con el tamaño correcto (resta 1px para ver las líneas)
    sf::RectangleShape cell({ m_cellSize - 1.f, m_cellSize - 1.f });

    // Recorre toda la rejilla
    for (unsigned x = 0; x < m_width; ++x) {
        for (unsigned y = 0; y < m_height; ++y) {
            // Coloca la forma usando Vector2f en lugar de dos floats
            cell.setPosition({ x * m_cellSize, y * m_cellSize });          

            // Selecciona color según el tipo de celda
            switch (m_grid[x][y]) {
            case CellType::Empty:      cell.setFillColor(sf::Color(50,  50,  50)); break;
            case CellType::Path:       cell.setFillColor(sf::Color(100, 100, 100)); break;
            case CellType::Tower:      cell.setFillColor(sf::Color(50, 150,  50)); break;
            case CellType::Obstacle:   cell.setFillColor(sf::Color(150, 50,  50)); break;
            case CellType::EntryPoint: cell.setFillColor(sf::Color::Green);        break;
            case CellType::ExitPoint:  cell.setFillColor(sf::Color::Red);          break;
            }

            // Dibuja la celda en el render target
            target.draw(cell);
        }
    }
}

