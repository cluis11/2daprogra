#include "PathFinding.h"
#include <queue>
#include <algorithm>
#include <iostream>

PathFinding::PathFinding(GridSystem* grid) : m_grid(grid)
{
    if (!grid) {
        throw std::invalid_argument("GridSystem no puede ser nulo");
    }

    // Inicializar estructuras para A*
    initializeNodes();
    precomputerHeuristics();
}

void PathFinding::initializeNodes() {
    unsigned width = m_grid->getWidth();
    unsigned height = m_grid->getHeight();
    m_nodes.resize(width, std::vector<Node>(height));

    for (unsigned x = 0; x < width; ++x) {
        for (unsigned y = 0; y < height; ++y) {
            m_nodes[x][y].gCost = INFINITY;
            m_nodes[x][y].fCost = INFINITY;
            m_nodes[x][y].closed = false;
            m_nodes[x][y].parent = { -1, -1 };
        }
    }
}

void PathFinding::precomputerHeuristics() {
    unsigned width = m_grid->getWidth();
    unsigned height = m_grid->getHeight();

    // Encontrar la posición de salida (ExitPoint)
    sf::Vector2i targetPos = findExitPosition();

    m_heuristics.resize(width, std::vector<float>(height));

    // Precalcular heurísticas (distancia Manhattan)
    for (unsigned x = 0; x < width; ++x) {
        for (unsigned y = 0; y < height; ++y) {
            float dx = std::abs(static_cast<float>(x) - targetPos.x);
            float dy = std::abs(static_cast<float>(y) - targetPos.y);
            m_heuristics[x][y] = 10.0f * (dx + dy);
        }
    }
}

sf::Vector2i PathFinding::findExitPosition() const
{
    for (unsigned x = 0; x < m_grid->getWidth(); ++x) {
        for (unsigned y = 0; y < m_grid->getHeight(); ++y) {
            if (m_grid->getCell(x, y) == CellType::ExitPoint) {
                return sf::Vector2i(x, y);
            }
        }
    }
    throw std::runtime_error("ExitPoint not found in grid");
}

std::vector<sf::Vector2i> PathFinding::findPath(const sf::Vector2i& start) {
    sf::Vector2i target = findExitPosition();
    //std::cout << "[DEBUG] ExitPoint encontrado en (" << target.x << "," << target.y << ")\n";
    std::cout << "[DEBUG] Buscando camino desde (" << start.x << "," << start.y << ") hasta (" << target.x << "," << target.y << ")\n";
    
    //Reset Nodes
    initializeNodes();

    auto compare = [this](const sf::Vector2i& a, const sf::Vector2i& b) {
        return m_nodes[a.x][a.y].fCost > m_nodes[b.x][b.y].fCost;
    };

    std::priority_queue<sf::Vector2i, std::vector<sf::Vector2i>, decltype(compare)>
        openSet(compare);

    //Inicializar nodo incial
    m_nodes[start.x][start.y].gCost = 0;
    m_nodes[start.x][start.y].fCost = m_heuristics[start.x][start.y];
    openSet.push(start);

    while (!openSet.empty()) {
        sf::Vector2i current = openSet.top();
        openSet.pop();

        if (current == target) { 
            return reconstructPath(start, target); 
        }

        if (m_nodes[current.x][current.y].closed) { 
            continue; 
        }

        m_nodes[current.x][current.y].closed = true;

        for (const auto& neighbor : getNeighbors(current)) {
            if (m_nodes[neighbor.x][neighbor.y].closed) { 
                continue; 
            }

            //Costo de movimiento (10 recto, 14 diagonal)
            float moveCost = 10.0f;
            float totalGCost = m_nodes[current.x][current.y].gCost + moveCost;

            if (totalGCost < m_nodes[neighbor.x][neighbor.y].gCost) {
                m_nodes[neighbor.x][neighbor.y].parent = current;
                m_nodes[neighbor.x][neighbor.y].gCost = totalGCost;
                m_nodes[neighbor.x][neighbor.y].fCost = totalGCost + m_heuristics[neighbor.x][neighbor.y];
                openSet.push(neighbor);
            }
        }
    }
    std::cout << "[WARNING] No se encontró camino al objetivo\n";
    return {}; //No se encontro camino
}

std::vector<sf::Vector2i> PathFinding::getNeighbors(const sf::Vector2i& pos) const {
    std::vector<sf::Vector2i> neighbors;
    int width = m_grid->getWidth();
    int height = m_grid->getHeight();

    const std::vector<sf::Vector2i> directions = {
        {1,0}, {-1,0}, {0,1}, {0,-1}
    };

    for (const auto& dir : directions) {
        int nx = pos.x + dir.x;
        int ny = pos.y + dir.y;

        if (nx < 0 || nx >= width || ny < 0 || ny >= height)
            continue;

        CellType celltype = m_grid->getCell(nx, ny);
        if (celltype != CellType::Obstacle && celltype != CellType::Tower) {
            neighbors.emplace_back(nx, ny);
        }
    }
    return neighbors;
}

std::vector<sf::Vector2i> PathFinding::reconstructPath(const sf::Vector2i& start, const sf::Vector2i& end) const {
    std::vector<sf::Vector2i> path;
    sf::Vector2i current = end;

    while (current != start) {
        path.push_back(current);
        current = m_nodes[current.x][current.y].parent;
        if (current.x == -1 && current.y == -1) {
            std::cout << "[DEBUG] Error: camino roto, parent inválido\n";
            return {};
        }
    }

    std::reverse(path.begin(), path.end());

    // Mostrar el camino en consola
    std::cout << "[CAMINO] Pasos (" << path.size() << "): ";
    for (const auto& step : path) {
        std::cout << "(" << step.x << "," << step.y << ") ";
    }
    std::cout << std::endl;
    return path;
}
