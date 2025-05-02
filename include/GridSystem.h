#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

//Numero en la matriz
enum class CellType {
    Empty,      // 0
    Path,       // 1
    Tower,      // 2
    Obstacle,   // 3
    EntryPoint, // 4
    ExitPoint   // 5
};

class GridSystem {
public:
    GridSystem(unsigned width, unsigned height, float cellSize);
    
    CellType getCell(int x, int y) const;
    bool setCell(int x, int y, CellType type);
    
    sf::Vector2f gridToWorld(int x, int y) const;
    sf::Vector2i worldToGrid(float x, float y) const;
    sf::Vector2i getEntryPoint() const;
    
    void render(sf::RenderTarget& target) const;
    
    unsigned getWidth() const { return m_width; }
    unsigned getHeight() const { return m_height; }
    float getCellSize() const { return m_cellSize; }

private:
    void initializeGridWithObstacles();
    
    unsigned m_width, m_height;
    float m_cellSize;
    std::vector<std::vector<CellType>> m_grid;
};