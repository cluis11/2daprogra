#include "GridSystem.h"
#include <algorithm>
#include "Enemy.h"
#include <iostream>

GridSystem::GridSystem(unsigned width, unsigned height, float cellSize)
    : m_width(width), m_height(height), m_cellSize(cellSize), m_rng(std::random_device{}()) {
    initializeGrid();
}

//define el mapa en la matriz
void GridSystem::initializeGrid() {
    //asigna todos los valores de la matriz a libres
    m_grid.resize(m_width, std::vector<CellType>(m_height, CellType::Empty));
    m_enemyGrid.resize(m_width, std::vector<Enemy*>(m_height, nullptr)); //matriz de punteros a enemigos para buscar y atacar

    //definir donde se originan enemigos
    m_spawnPoints = {{0, 0}, {0, 1}, {1, 0}, {1, 1}, {0, 25}, {0, 26}};

    //asigna la puerta del castillo en el borde derecho
    m_grid[m_width-1][m_height-1] = CellType::ExitPoint;

    //asigna obstaculos
    addObstacleCluster(10, 10, 3);
    addObstacleCluster(30, 20, 2);
    addObstacleCluster(15, 35, 4);
}

//guarda un enemy en la matriz y un puntero a este en la matriz de punteros
void GridSystem::registerEnemy(Enemy* enemy, int gridX, int gridY) {
    if (gridX >= 0 && gridY >= 0 && gridX < static_cast<int>(m_width) && gridY < static_cast<int>(m_height)) {
        m_enemyGrid[gridX][gridY] = enemy;
        m_grid[gridX][gridY] = CellType::Enemy;
    }
}

//hace la casilla vacia al moverse o matar al enemigo
void GridSystem::unregisterEnemy(int gridX, int gridY) {
    if (gridX >= 0 && gridY >= 0 && gridX < static_cast<int>(m_width) && gridY < static_cast<int>(m_height)) {
        m_enemyGrid[gridX][gridY] = nullptr;
        m_grid[gridX][gridY] = CellType::Empty;
    }
}

std::vector<Enemy*> GridSystem::getEnemiesInRadius(int gridX, int gridY, int radius) const {
    std::vector<Enemy*> enemies;
    std::cout << "=== Tower at [" << gridX << "," << gridY << "] checking radius " << radius << " ===" << std::endl;
    std::cout << "Search area: X[" << gridX-radius << " to " << gridX+radius 
              << "], Y[" << gridY-radius << " to " << gridY+radius << "]" << std::endl;
    for (int x = gridX - radius; x <= gridX + radius; ++x) {
        for (int y = gridY - radius; y <= gridY + radius; ++y) {
            if (x >= 0 && y >= 0 && x < 50 && y < 50) {
                if (m_enemyGrid[x][y] != nullptr) {
                    enemies.push_back(m_enemyGrid[x][y]);
                }
            }
        }
    }
    return enemies;
}

//funcion para crear obstaculos
void GridSystem::addObstacleCluster(int centerX, int centerY, int radius){
    std::uniform_int_distribution<int> dist(0, 100);

    for(int x = centerX - radius; x<= centerX + radius; x++) {
        for(int y = centerY - radius; y <= centerY + radius; y++) {
            if(x >= 0 && x < static_cast<int>(m_width) && y < static_cast<int>(m_height)) {
                if(dist(m_rng) < 70) {
                    m_grid[x][y] = CellType::Obstacle;
                }
            }
        }
    }
}

//Retorna el tipo de celda, si esta fuera de la matriz retorna el valor de obstaculo
CellType GridSystem::getCell(int x, int y) const {
    if (x < 0 || y < 0 || x >= static_cast<int>(m_width) || y >= static_cast<int>(m_height))
        return CellType::Obstacle;
    return m_grid[x][y];
}

//Asigna un valor a una celda, para asignar torres o mover enemigos
bool GridSystem::setCell(int x, int y, CellType type) {
    if (x < 0 || y < 0 || x >= static_cast<int>(m_width) || y >= static_cast<int>(m_height))
        return false;
    m_grid[x][y] = type;
    return true;
}

//Determina si un enemigo se puede mover a una celda
bool GridSystem::isCellWalkable(int x, int y) const {
    CellType type = getCell(x, y);
    return type == CellType::Empty || type == CellType::ExitPoint;
}

//conversion grid a pixel
sf::Vector2f GridSystem::gridToWorld(int x, int y) const {
    return sf::Vector2f(
        x * m_cellSize + m_cellSize / 2.f,
        y * m_cellSize + m_cellSize / 2.f
    );
}

//conversion pixel a pos matriz
sf::Vector2i GridSystem::worldToGrid(float x, float y) const {
    return sf::Vector2i(
        static_cast<int>(x / m_cellSize),
        static_cast<int>(y / m_cellSize)
    );
}

//Dibuja los elementos del grid
void GridSystem::render(sf::RenderTarget& target) const {
    // Construye la forma con el tamaño correcto
    sf::RectangleShape cell({ m_cellSize - 1.f, m_cellSize - 1.f }); //diferente en 3.0

    for (unsigned x = 0; x < m_width; ++x) {
        for (unsigned y = 0; y < m_height; ++y) {
            cell.setPosition({ x * m_cellSize, y * m_cellSize }); //diferente en 3.0      

            // Selecciona color según el tipo de celda
            switch (m_grid[x][y]) {
                case CellType::Empty: cell.setFillColor(sf::Color(50,  50,  50)); break;
                case CellType::Obstacle: cell.setFillColor(sf::Color(150, 150,  150)); break;
                case CellType::Tower: cell.setFillColor(sf::Color(50, 50,  150)); break;
                case CellType::ExitPoint: cell.setFillColor(sf::Color(150,0,0));          break;
            }
            target.draw(cell);
        }
    }
}

