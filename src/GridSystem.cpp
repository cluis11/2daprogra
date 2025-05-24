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
    for (int col = 0; col < 2; ++col) {
        for (int row = 0; row < 50; ++row) {
            m_spawnPoints.push_back({ col, row });
        }
    }

    //asigna la puerta del castillo en el borde derecho
    m_grid[49][24] = CellType::ExitPoint;
    m_grid[49][25] = CellType::ExitPoint;
    m_grid[49][26] = CellType::ExitPoint;
    

    //asigna obstaculos
    addObstacleCluster(15, 10, 11);
    addObstacleCluster(15, 40, 11);
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
void GridSystem::addObstacleCluster(int centerX, int centerY, int radius) {
    int startX = std::max(0, centerX - radius);
    int endX = std::min(static_cast<int>(m_width) - 1, centerX + radius);
    int startY = std::max(0, centerY - radius);
    int endY = std::min(static_cast<int>(m_height) - 1, centerY + radius);

    // Llena todo el bloque con obstáculos
    for (int x = startX; x <= endX; ++x) {
        for (int y = startY; y <= endY; ++y) {
            m_grid[x][y] = CellType::Obstacle;
        }
    }
    int spacing = 4; // cada cuántas celdas se hace un camino

    // Crea caminos horizontales internos (de grosor 2)
    for (int y = startY + spacing; y <= endY - 2; y += spacing) {
        for (int x = startX; x <= endX; ++x) {
            m_grid[x][y] = CellType::Empty;
            if (y + 1 <= endY)
                m_grid[x][y + 1] = CellType::Empty;
        }

        // Abrir accesos en los bordes para este camino
        if (startX > 0) m_grid[startX][y] = CellType::Empty;
        if (endX < static_cast<int>(m_width) - 1) m_grid[endX][y] = CellType::Empty;
    }

    // Crea caminos verticales internos (de grosor 2)
    for (int x = startX + spacing; x <= endX - 2; x += spacing) {
        for (int y = startY; y <= endY; ++y) {
            m_grid[x][y] = CellType::Empty;
            if (x + 1 <= endX)
                m_grid[x + 1][y] = CellType::Empty;
        }

        // Abrir accesos en los bordes para este camino
        if (startY > 0) m_grid[x][startY] = CellType::Empty;
        if (endY < static_cast<int>(m_height) - 1) m_grid[x][endY] = CellType::Empty;
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

void GridSystem::unregisterTower(int gridX, int gridY) {
    if (gridX >= 0 && gridY >= 0 && gridX < static_cast<int>(m_width) && gridY < static_cast<int>(m_height)) {
        //m_enemyGrid[gridX][gridY] = nullptr;
        m_grid[gridX][gridY] = CellType::Empty;
        std::cout << "Ahora se hizo vacio el campo" << std::endl;
    }
}

