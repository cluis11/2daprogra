#pragma once
#include "GridSystem.h"
#include <vector>
#include <SFML/Graphics.hpp>


class PathFinding {
	public:
		PathFinding(GridSystem* grid);
		std::vector<sf::Vector2i> findPath(const sf::Vector2i& start);

	private:
		GridSystem* m_grid; // Referencia a la matriz

		struct Node {
			float gCost = INFINITY; //Costo desde inicio
			float fCost = INFINITY; //Costo total (g+ h)
			sf::Vector2i parent = { -1, -1 };
			bool closed = false;
		};

		// Estructuras para A*
		std::vector<std::vector<Node>> m_nodes;
		std::vector<std::vector<float>> m_heuristics;

		void initializeNodes();
		void precomputerHeuristics();
		sf::Vector2i findExitPosition() const;

		//Helpers para A*
		std::vector<sf::Vector2i> getNeighbors(const sf::Vector2i& pos) const;
		std::vector<sf::Vector2i> reconstructPath(const sf::Vector2i& start, const sf::Vector2i& end) const;
};