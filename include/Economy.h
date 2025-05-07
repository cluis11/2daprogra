#pragma once

class Economy {
public:
    Economy(int startingCoins = 100) : m_coins(startingCoins) {}

    //funcion que valida si le alcanza la torre o el upgrade
    bool canAfford(int cost) const { return m_coins >= cost; }
    //funcion que hace el rebajo de dinero tras una compra
    void spend(int amount) { m_coins -= amount; }
    //Funcion que suma dinero
    void earn(int amount) { m_coins += amount; }
    //retorna el dinero en un momento dado
    int getCoins() const { return m_coins; }

private:
    int m_coins; //cantidad de dinero del juegador
};