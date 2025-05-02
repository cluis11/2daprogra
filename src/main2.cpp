#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

const int GRID_SIZE = 10; // 10x10 grid
const int CELL_SIZE = 50; // Each cell is 50x50 pixels

int main()
{
    // Create the window
    sf::RenderWindow window(sf::VideoMode({GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE}), "Matrix Grid");

    // Create a 10x10 matrix filled with numbers (0-3 for different colors)
    std::vector<std::vector<int>> matrix(GRID_SIZE, std::vector<int>(GRID_SIZE));
    
    // Fill the matrix with some values (0-3)
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            matrix[i][j] = (i + j) % 4; // Pattern that creates a gradient
        }
    }

    // Main game loop
    while (window.isOpen()) {
        // Event handling
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            
            // Optional: Change matrix values on mouse click
            if (event->is<sf::Event::MouseButtonPressed>()) {
                const auto& mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
                if (mouseEvent) {
                    int x = mouseEvent->position.x / CELL_SIZE;
                    int y = mouseEvent->position.y / CELL_SIZE;
                    
                    if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
                        matrix[x][y] = (matrix[x][y] + 1) % 4;
                    }
                }
            }
        }

        // Clear the window
        window.clear(sf::Color::White);

        // Draw the grid based on matrix values
        for (int i = 0; i < GRID_SIZE; ++i) {
            for (int j = 0; j < GRID_SIZE; ++j) {
                sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2)); // -2 for grid lines
                cell.setPosition(sf::Vector2f(i * CELL_SIZE + 1, j * CELL_SIZE + 1));
                
                // Set color based on matrix value
                switch (matrix[i][j]) {
                    case 0: cell.setFillColor(sf::Color::Red); break;
                    case 1: cell.setFillColor(sf::Color::Green); break;
                    case 2: cell.setFillColor(sf::Color::Blue); break;
                    case 3: cell.setFillColor(sf::Color::Yellow); break;
                }
                
                window.draw(cell);
            }
        }

        // Display the window
        window.display();
    }

    return 0;
}