#include "Game.h"
#include <sstream>
#include <stdexcept>
#include <iostream>

//Constructor del juego
Game::Game() 
    : m_window(sf::VideoMode( sf::Vector2u(800u, 800u) ), "Genetic Kingdom"), //diferente en 3.0
      m_grid(50, 50, 16.f), //crea la matriz mxm y asigna el cellSize
      m_font(), //Se define la fuente aqui por cambio en 3.0
      m_waveText(m_font, "Wave: 0/3",    20), //Se define el texto aqui por cambio en 3.0
      m_timerText(m_font, "Time: 0s",     20), //Se define la texto aqui por cambio en 3.0
      m_towersText(m_font, "Towers: 0/10",20), //Se define la texto aqui por cambio en 3.0
      m_leaksText(m_font, "Leaks: 0/5",   20), m_geneticManager() // Inicializa el GeneticManager //Se define la texto aqui por cambio en 3.0


{
    //Limite de framerates
    m_window.setFramerateLimit(60);
    
    //busca la fuente y la asigna a m_font
    //La fuente se copia a la carpeta build cuando se ejecuta el cmakelist
    //nombre de la funcion cambio en 3.0
    if (!m_font.openFromFile("assets/fonts/arial.ttf")) {
        throw std::runtime_error("Failed to load font file 'arial.ttf'");
    }
    
    //Posiciona los textos
    m_waveText.setPosition(sf::Vector2f(10.f, 10.f));
    m_timerText.setPosition(sf::Vector2f(350.f, 10.f));
    m_towersText.setPosition(sf::Vector2f(650.f, 10.f));
    m_leaksText.setPosition(sf::Vector2f(10.f, 750.f));

    //Inicializa primera generacion de genomas
    m_geneticManager.initializeFirstGeneration();

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

void Game::update(float deltaTime) {
    m_stateTimer += deltaTime;

    if (m_currentState == GameState::Prep && m_stateTimer >= 8.f) {
        startNewWave();
    }
    else if (m_currentState == GameState::Wave) {
        updateWaveState(deltaTime);
    }
    else if (m_currentState == GameState::Cooldown && m_stateTimer >= 5.f) {
        handleCooldownEnd();
    }

    updateUI();
}

void Game::startNewWave() {
    m_currentState = GameState::Wave;
    m_stateTimer = 0.f;

    // Configuracion de la oleada
    Wave::Config config;
    config.spawnInterval = 5.0f;  // Cada 5 segundos
    config.enemiesPerSpawn = 1;   // 1 enemigo por spawn
    config.maxEnemies = 20 + (20 * (m_waveNumber - 1)); // 20, 40, 60
    config.waveDuration = 60.0f;
    config.maxSpawnPoints = std::min(3 + m_waveNumber, 6);

    // Obtiene spawn points del grid
    const auto& spawnPoints = m_grid.getSpawnPoints();

    std::cout << "\n=== OLEADA " << m_waveNumber << " ===" << std::endl;
    std::cout << "Max enemigos: " << config.maxEnemies
              << " | Spawn cada: " << config.spawnInterval << "s"
              << " | Puntos de spawn: " << config.maxSpawnPoints << "\n";

    m_currentWave = std::make_unique<Wave>(
        m_waveNumber,
        spawnPoints,
        config,
        &m_grid,
        m_geneticManager.getCurrentGenomes());
}

void Game::updateWaveState(float deltaTime) {
    m_currentWave->update(deltaTime);

    // Intenta spawnear enemigos
    auto genomes = m_currentWave->getGenomesForNextSpawn();
    for (const auto& genome : genomes) {
        spawnEnemy(genome);
    }

    updateEnemies(deltaTime);

    if (m_currentWave->isCompleted()) {
        endCurrentWave();
    }
}

void Game::spawnEnemy(const EnemyGenome& genome) {
    // Obtiene puntos de spawn activos
    const auto& spawnPoints = m_currentWave->getSpawnPoints();
    if (spawnPoints.empty()) return;

    // Selecciona punto de spawn rota entre cada punto
    static size_t spawnIndex = 0;
    spawnIndex = (spawnIndex + 1) % spawnPoints.size();
    const auto& spawnPos = spawnPoints[spawnIndex];

    // Verifica si la celda esta disponible
    if (m_grid.getCell(spawnPos.x, spawnPos.y) != CellType::Empty) {
        return;
    }

    // Crea el enemigo
    auto enemy = std::make_unique<Enemy>(genome.getType(), spawnPos.x, spawnPos.y, &m_grid);
    enemy->setGenome(genome);
    m_enemies.emplace_back(std::move(enemy));

    // Depurar
    const auto& attrs = genome.getAttributes();
    std::cout << "--> Spawned enemy at (" << spawnPos.x << "," << spawnPos.y << ")"
              << " | Type: " << static_cast<int>(genome.getType())
              << " | Health: " << attrs.health
              << " | Speed: " << attrs.speed << "\n";
}


void Game::spawnWaveEnemies() {
    auto genomes = m_currentWave->getGenomesForNextSpawn();
    if (!genomes.empty()) {
        auto spawnPoints = m_currentWave->getSpawnPoints();
        if (!spawnPoints.empty()) {
            static size_t spawnIndex = 0;
            spawnIndex = (spawnIndex + 1) % spawnPoints.size();
            spawnEnemy(genomes[0]);
        }
    }
}
void Game::updateEnemies(float deltaTime) {
    for (auto it = m_enemies.begin(); it != m_enemies.end(); ) {
        (*it)->update(deltaTime);
        if (!(*it)->isAlive()) {
            logEnemyDeath(*it);
            it = m_enemies.erase(it);
        } else {
            ++it;
        }
    }
}

void Game::logEnemyDeath(const std::unique_ptr<Enemy>& enemy) {
    std::cout << "Enemigo murio - Tipo: " << static_cast<int>(enemy->getType())
              << " Pasos: " << enemy->getStepsTaken()
              << " Pos: (" << enemy->getGridX() << "," << enemy->getGridY() << ")"
              << std::endl;
}

void Game::endCurrentWave() {
    m_geneticManager.evaluateGeneration(m_enemies);
    m_geneticManager.createNextGeneration();
    m_currentState = GameState::Cooldown;
    m_currentWave.reset();
    m_waveNumber++;
    m_enemies.clear();
}

void Game::handleCooldownEnd() {
    if (m_waveNumber > 3) {
        m_window.close();
    }
    m_currentState = GameState::Prep;
    m_stateTimer = 0.f;
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

//Funcion responsable de tod0 lo que pasa en el juego
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
