#include "Tower.h"

Tower::Tower(int gridX, int gridY, GridSystem* grid)
    : Entity(gridX, gridY, sf::Color::Green, grid) {}

void Tower::update(float deltaTime) {
    (void)deltaTime;
    // Attack logic would go here
}