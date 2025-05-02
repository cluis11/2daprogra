#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

//Numero en la matriz
enum class CellType { 
    Empty,       // 0 - Gris oscuro
    Obstacle,    // 1 - Gris claro
    Tower,       // 2 - Colores variados
    SpawnArea,   // 3 - Verde
    ExitPoint    // 4 - Rojo
};

class GridSystem {
public:
    GridSystem(unsigned width, unsigned height, float cellSize);
    
    //pos de la matriz
    CellType getCell(int x, int y) const;
    bool setCell(int x, int y, CellType type);
    bool isCellWalkable (int x, int y) const;
    
    //conversiones de pos a cordenadas
    sf::Vector2f gridToWorld(int x, int y) const;
    sf::Vector2i worldToGrid(float x, float y) const;

    //areas de spawn
    sf::Vector2i getRandomSpawnPoint() const;
    std::vector<sf::Vector2i> getAllSpawnPoints() const;
    
    void render(sf::RenderTarget& target) const;

    //Getters    
    unsigned getWidth() const { return m_width; }
    unsigned getHeight() const { return m_height; }
    float getCellSize() const { return m_cellSize; }
    std::mt19937& getRNG() { return m_rng; }

private:
    //funciones
    void initializeGrid();
    void addObstacleCluster(int centerX, int centerY, int radius);
    
    //atributos
    unsigned m_width, m_height;
    float m_cellSize;
    std::vector<std::vector<CellType>> m_grid;
    mutable std::mt19937 m_rng; 
};