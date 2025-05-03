#include "Game.h"
#include <sstream>
#include <stdexcept>

//Constructor del juego
Game::Game() 
    : m_window(sf::VideoMode( sf::Vector2u(800u, 800u) ), "Genetic Kingdom"), //diferente en 3.0
      m_grid(50, 50, 16.f), //crea la matriz mxm y asigna el cellSize
      m_font(), //Se define la fuente aqui por cambio en 3.0
      m_waveText(m_font, "Wave: 0/3",    20), //Se define el texto aqui por cambio en 3.0
      m_timerText(m_font, "Time: 0s",     20), //Se define la texto aqui por cambio en 3.0
      m_towersText(m_font, "Towers: 0/10",20), //Se define la texto aqui por cambio en 3.0
      m_leaksText(m_font, "Leaks: 0/5",   20) //Se define la texto aqui por cambio en 3.0
{
    //Limite de framerates
    m_window.setFramerateLimit(60);
    
    //busca la fuente y la asigna a m_font
    //La fuente se copia a la carpeta build cuando se ejecuta el cmakelist
    //nombre de la funcion cambio en 3.0
    if (!m_font.openFromFile("assets/fonts/arial.ttf")) {
        throw std::runtime_error("Failed to load font file 'arial.ttf'");
    }
    
    // Posiciona los textos
    m_waveText.setPosition(sf::Vector2f(10.f, 10.f));
    m_timerText.setPosition(sf::Vector2f(350.f, 10.f));
    m_towersText.setPosition(sf::Vector2f(650.f, 10.f));
    m_leaksText.setPosition(sf::Vector2f(10.f, 750.f));

    //como se asignaron los textos se actualiza la interfaz
    updateUI();
}

//Funcion encargada de procesar los inputs del usuario
//Eventos cambiaron un poco en 3.0
void Game::processEvents() {
    while (const auto event = m_window.pollEvent()) { //Diferente en 3.0
        //Cerrar ventana
        if (event->is<sf::Event::Closed>()) { //Diferente en 3.0
            m_window.close();
        }

        //Permite colocar torres solo en el stage Prep y si son menos de 10
        //Esto es solo para probar version final se colocan torres durante el wave con la logica de econimia
        if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>();
            mb
         && m_currentState == GameState::Prep
         && m_towersPlaced  < 10) //aqui se tiene que agregar la logica para comprar
        {
            // Convertir posición de click a matriz
            //diferente en 3.0
            auto gridPos = m_grid.worldToGrid(
                static_cast<float>(mb->position.x),
                static_cast<float>(mb->position.y)
            );

            //Revisiones de la posicion de la matriz donde se dio click
            //Si la celda esta vacia permite colocar una torre
            //crea la torre y la agrega a la lista de torres
            if (m_grid.getCell(gridPos.x, gridPos.y) == CellType::Empty) {
                m_towers.emplace_back(
                    std::make_unique<Tower>(m_nextTowerType, gridPos.x, gridPos.y, &m_grid)
                );
                m_grid.setCell(gridPos.x, gridPos.y, CellType::Tower);
                m_towersPlaced++;

                // Rota el tipo de torre, solo de momento se cambia por seleccion en pantalla y economia
                m_nextTowerType = static_cast<Tower::Type>((static_cast<int>(m_nextTowerType) + 1) % 3);
                
                //Actualiza la interfaz para mostrar torre
                updateUI();
            }
        }
    }  
}

//Funcion que se encarga de lo que pasa en el juego
//Aqui se cambian los state y se determina que hacer segun estos
//Cada objeto tiene su propio update
void Game::update(float deltaTime) {
    //Inicia el timer del juego, se reinicia en cada cambio de estado
    m_stateTimer += deltaTime; 

    //Transisiones de state
    //Transicion de state Prep a Wave despues de 30 segundos
    if (m_currentState == GameState::Prep && m_stateTimer >= 30.f) {
        m_currentState = GameState::Wave;
        m_stateTimer = 0.f;
    }
    //Transicion de state Wave a Cooldown despues de 60 segundos
    //Aumenta el numero de wave
    //Hay que implementar logica de gameover por leaks
    else if (m_currentState == GameState::Wave) {
        if (m_stateTimer >= 60.f) {
            m_currentState = GameState::Cooldown;
            m_stateTimer = 0.f;
            m_waveNumber++;
        }
        
        int currentSecond = static_cast<int>(m_stateTimer);
        //Mientras stase = Wave crea enemigos cada 10 segundos
        if (currentSecond % 10 == 0 && currentSecond != m_lastSpawnSecond) {
            spawnEnemy();
            m_lastSpawnSecond = currentSecond;
        }
        //Mas comportamientos del wave irian aqui tambien
    }
    //Transicion de state Cooldown a Prep despues de 10 segundos
    else if (m_currentState == GameState::Cooldown && m_stateTimer >= 10.f) {
        //Logica de ganar si completa 3 waves
        if (m_waveNumber > 3) {
            // Cierra la ventana pero hay que implementar un stade de victory
            //stade victory muestra datos de la partida y permite cerrar o volver a jugar
            m_window.close();
        }
        //si no ha completado waves para a prep para la siguiente
        m_currentState = GameState::Prep;
        m_stateTimer = 0.f;
    }
    //actualiza UI, ver como cambiar lo que se muestra segun state
    updateUI();
}

//funcion que dibuja en la pantalla
void Game::render() {
    m_window.clear();
    
    //Dibuja matriz
    m_grid.render(m_window);
    
    //Dibuja torres y enemigos
    //Llama a la funcion render de cada objeto
    //render se maneja a nivel de Entity
    for (const auto& tower : m_towers) tower->render(m_window);
    for (const auto& enemy : m_enemies) enemy->render(m_window);
    
    //Dibuja textos
    m_window.draw(m_waveText);
    m_window.draw(m_timerText);
    m_window.draw(m_towersText);
    m_window.draw(m_leaksText);
    
    //Abre la pantalla
    m_window.display();
}

//Funcion encargada de crear enemigos durante el wave
//Hay que agregar el algoritmo genetico en la clase enemy
//Segun el numero de wave se puede modificar
void Game::spawnEnemy() {
    auto spawnPos = m_grid.getRandomSpawnPoint(); //obtiene {x,y} de la matriz
    //validacion pos correcta
    if (spawnPos.x != -1) {
        std::uniform_int_distribution<int> dist(0, 99);
        int roll = dist(m_grid.getRNG()); //Para determinar que enemigo crear
        Enemy::Type type;
        //usa roll para crear un enemigo especifico
        if (roll < 40) type = Enemy::Type::Ogre;
        else if (roll < 70) type = Enemy::Type::DarkElf;
        else if (roll < 90) type = Enemy::Type::Harpy;
        else type = Enemy::Type::Mercenary;
        //Crea el enemigo y lo agrega a la lista
        m_enemies.emplace_back(std::make_unique<Enemy>(type, spawnPos.x, spawnPos.y, &m_grid));
    }
}

//Funcion encargada de actualizar la interfaz cuando para un evento de usuario u objetos
void Game::updateUI() {
    std::string stateStr;
    //Asigna un string de acuerdo al state
    switch (m_currentState) {
        case GameState::Prep: stateStr = "PREP";  break;
        case GameState::Wave: stateStr = "WAVE";  break;
        case GameState::Cooldown: stateStr = "END";   break;
    }
    //Actualiza los textos con los datos actuales
    m_waveText.setString("Wave:  " + std::to_string(m_waveNumber)   + "/3");
    m_timerText.setString(stateStr + ": " + std::to_string(int(m_stateTimer)) + "s");
    m_towersText.setString("Towers: " + std::to_string(m_towersPlaced) + "/10");
    m_leaksText.setString("Leaks: "  + std::to_string(m_leaks)         + "/5");
}

//Funcion responsable de todo lo que pasa en el juego
//Llama a las funciones basicas de SFML 
void Game::run() {
    sf::Clock clock;
    while (m_window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        processEvents();
        update(deltaTime);
        render();
    }
}
