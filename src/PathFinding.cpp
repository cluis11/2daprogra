#include "PathFinding.h"
#include <queue>
#include <algorithm>
#include <iostream>

// Constructor de la clase PathFinding
// Inicializa con un puntero al sistema de grid y prepara las estructuras necesarias
PathFinding::PathFinding(GridSystem* grid) : m_grid(grid)
{
    if (!grid) {
        throw std::invalid_argument("GridSystem no puede ser nulo");
    }

    // Inicializa los nodos para A*
    initializeNodes();
    // Calcula las heurísticas antes de iniciar la búsqueda de caminos
    precomputerHeuristics();
}

// Inicializa la matriz de nodos con valores por defecto (gCost, fCost, etc.)
void PathFinding::initializeNodes() {
    unsigned width = m_grid->getWidth();
    unsigned height = m_grid->getHeight();
    m_nodes.resize(width, std::vector<Node>(height));

    for (unsigned x = 0; x < width; ++x) {
        for (unsigned y = 0; y < height; ++y) {
            m_nodes[x][y].gCost = INFINITY;
            m_nodes[x][y].fCost = INFINITY;
            m_nodes[x][y].closed = false;
            m_nodes[x][y].parent = { -1, -1 }; // Sin padre aún
        }
    }
}

// Precalcula la heurística de cada celda usando la distancia Manhattan al punto de salida
void PathFinding::precomputerHeuristics() {
    unsigned width = m_grid->getWidth();
    unsigned height = m_grid->getHeight();

    // Encuentra la posición objetivo (salida)
    sf::Vector2i targetPos = findExitPosition();

    m_heuristics.resize(width, std::vector<float>(height));

    for (unsigned x = 0; x < width; ++x) {
        for (unsigned y = 0; y < height; ++y) {
            float dx = std::abs(static_cast<float>(x) - targetPos.x);
            float dy = std::abs(static_cast<float>(y) - targetPos.y);
            m_heuristics[x][y] = 10.0f * (dx + dy); // Manhattan * costo unitario
        }
    }
}

// Busca la celda que contiene la salida (ExitPoint) en la grid
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

// Algoritmo principal de A* para encontrar un camino desde 'start' hasta la salida
std::vector<sf::Vector2i> PathFinding::findPath(const sf::Vector2i& start) {
    sf::Vector2i target = findExitPosition();

    std::cout << "[DEBUG] Buscando camino desde (" << start.x << "," << start.y << ") hasta (" << target.x << "," << target.y << ")\n";

    // Reinicia los nodos antes de iniciar una nueva búsqueda
    initializeNodes();

    // Función lambda para comparar nodos por su fCost
    auto compare = [this](const sf::Vector2i& a, const sf::Vector2i& b) {
        return m_nodes[a.x][a.y].fCost > m_nodes[b.x][b.y].fCost;
        };

    // Cola de prioridad para manejar nodos abiertos (openSet)
    std::priority_queue<sf::Vector2i, std::vector<sf::Vector2i>, decltype(compare)>
        openSet(compare);

    // Inicializa el nodo de inicio
    m_nodes[start.x][start.y].gCost = 0;
    m_nodes[start.x][start.y].fCost = m_heuristics[start.x][start.y];
    openSet.push(start);

    // Bucle principal de A*
    while (!openSet.empty()) {
        sf::Vector2i current = openSet.top();
        openSet.pop();

        // Si llegamos al objetivo, reconstruir el camino
        if (current == target) {
            return reconstructPath(start, target);
        }

        // Si ya se cerró este nodo, omitirlo
        if (m_nodes[current.x][current.y].closed) {
            continue;
        }

        m_nodes[current.x][current.y].closed = true;

        // Explora vecinos del nodo actual
        for (const auto& neighbor : getNeighbors(current)) {
            if (m_nodes[neighbor.x][neighbor.y].closed) {
                continue;
            }

            float moveCost = 10.0f; // Costo de movimiento recto
            float totalGCost = m_nodes[current.x][current.y].gCost + moveCost;

            // Si se encuentra un camino mejor hacia el vecino
            if (totalGCost < m_nodes[neighbor.x][neighbor.y].gCost) {
                m_nodes[neighbor.x][neighbor.y].parent = current;
                m_nodes[neighbor.x][neighbor.y].gCost = totalGCost;
                m_nodes[neighbor.x][neighbor.y].fCost = totalGCost + m_heuristics[neighbor.x][neighbor.y];
                openSet.push(neighbor);
            }
        }
    }

    // Si no se encuentra camino
    std::cout << "[WARNING] No se encontró camino al objetivo\n";
    return {};
}

// Retorna los vecinos válidos (no bloqueados por torres u obstáculos)
std::vector<sf::Vector2i> PathFinding::getNeighbors(const sf::Vector2i& pos) const {
    std::vector<sf::Vector2i> neighbors;
    int width = m_grid->getWidth();
    int height = m_grid->getHeight();

    // Direcciones cardinales (no incluye diagonales)
    const std::vector<sf::Vector2i> directions = {
        {1,0}, {-1,0}, {0,1}, {0,-1}
    };

    for (const auto& dir : directions) {
        int nx = pos.x + dir.x;
        int ny = pos.y + dir.y;

        if (nx < 0 || nx >= width || ny < 0 || ny >= height)
            continue;

        CellType celltype = m_grid->getCell(nx, ny);

        // Solo se consideran celdas transitables (no torres ni obstáculos)
        if (celltype != CellType::Obstacle && celltype != CellType::Tower) {
            neighbors.emplace_back(nx, ny);
        }
    }
    return neighbors;
}

// Reconstruye el camino desde el nodo objetivo hasta el inicio
std::vector<sf::Vector2i> PathFinding::reconstructPath(const sf::Vector2i& start, const sf::Vector2i& end) const {
    std::vector<sf::Vector2i> path;
    sf::Vector2i current = end;

    while (current != start) {
        path.push_back(current);
        current = m_nodes[current.x][current.y].parent;

        // Validación de seguridad por si hay nodos huérfanos
        if (current.x == -1 && current.y == -1) {
            std::cout << "[DEBUG] Error: camino roto, parent inválido\n";
            return {};
        }
    }

    std::reverse(path.begin(), path.end()); // Ordenar camino de inicio a fin

    // Mostrar camino reconstruido en consola
    std::cout << "[CAMINO] Pasos (" << path.size() << "): ";
    for (const auto& step : path) {
        std::cout << "(" << step.x << "," << step.y << ") ";
    }

    std::cout << std::endl;
    return path;
}