#pragma once
#include "Entity.h"

class Tower : public Entity {
public:
    Tower(int gridX, int gridY, GridSystem* grid);
    void update(float deltaTime) override;
};