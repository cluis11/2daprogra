#pragma once
#include <SFML/Graphics.hpp>

class GridSystem;

class Entity {
public:
    //Cada entidad tiene la pos en el grid, color y una referencia al grid
    Entity(int gridX, int gridY, sf::Color color, GridSystem* grid);
    virtual ~Entity() = default;
    
    virtual void update(float deltaTime) = 0; //Funcion que se encarga de la logica del objeto
    void render(sf::RenderTarget& target) const; //Funcion encargada de dibujar el opjeto
    
    //Getters y Setters
    int getGridX() const { return m_gridX; }
    int getGridY() const { return m_gridY; }
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;

protected:
    int m_gridX, m_gridY;
    sf::RectangleShape m_shape;
    GridSystem* m_grid;
};