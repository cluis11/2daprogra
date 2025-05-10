#pragma once
#include "GridSystem.h"
#include <vector>
#include <SFML/Graphics.hpp>

// Clase encargada de realizar el pathfinding utilizando el algoritmo A*
class PathFinding {
public: 
    PathFinding(GridSystem* grid);
    std::vector<sf::Vector2i> findPath(const sf::Vector2i& start);

private:
    GridSystem* m_grid; // Referencia a la grid del mapa (matriz de celdas)

    // Estructura interna para representar un nodo del algoritmo A*
    struct Node {
        float gCost = INFINITY; // Costo desde el nodo inicial hasta este nodo
        float fCost = INFINITY; // Costo total estimado (gCost + heur�stica)
        sf::Vector2i parent = { -1, -1 }; // Coordenadas del nodo padre en el camino
        bool closed = false; // Indica si este nodo ya fue procesado
    };
  
    std::vector<std::vector<Node>> m_nodes;  // Matriz de nodos que representan el estado del mapa durante la ejecuci�n de A* 
    std::vector<std::vector<float>> m_heuristics;  // Matriz de heur�sticas precalculadas (distancia estimada hasta el objetivo)
  
    // Funciones
    void initializeNodes(); 
    void precomputerHeuristics(); 
    sf::Vector2i findExitPosition() const;
    std::vector<sf::Vector2i> reconstructPath(const sf::Vector2i& start, const sf::Vector2i& end) const;

    // Get
    std::vector<sf::Vector2i> getNeighbors(const sf::Vector2i& pos) const;

    
};