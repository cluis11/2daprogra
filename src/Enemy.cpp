#include "Enemy.h"

Enemy::Enemy(int gridX, int gridY, GridSystem* grid)
    : Entity(gridX, gridY, sf::Color::Red, grid) {}

void Enemy::update(float deltaTime) {
    (void)deltaTime;
    // Movement logic would go here
}