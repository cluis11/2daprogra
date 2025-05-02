#pragma once
#include "Entity.h"

class Enemy : public Entity {
public:
    Enemy(int gridX, int gridY, GridSystem* grid);
    void update(float deltaTime) override;
};