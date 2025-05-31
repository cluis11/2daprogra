#include "Game.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

//Constructor del juego
Game::Game() 
    : m_window(sf::VideoMode( sf::Vector2u(1000u, 800u) ), "Genetic Kingdom"), //diferente en 3.0
      m_grid(50, 50, 16.f), //crea la matriz mxm y asigna el cellSize
      m_pathfinder(&m_grid),
      m_font(), //Se define la fuente aqui por cambio en 3.0
      m_waveText(m_font, "Wave: 0/5",    20), //Se define el texto aqui por cambio en 3.0
      m_timerText(m_font, "Time: 0s",     20), //Se define la texto aqui por cambio en 3.0
      m_towersText(m_font, "Towers: 0/50",20), //Se define la texto aqui por cambio en 3.0
      m_leaksText(m_font, "Leaks: 0/5",   20), //Se define la texto aqui por cambio en 3.0
      m_coinsText(m_font, "Coins: ", 20),
      m_towerInfoText(m_font, "No tower selected", 16),
      m_towerLevelText(m_font, "Level: 0/3", 16),
      m_upgradeButtonText(m_font, "Upgrade (100)", 16),
      m_restartText(m_font, "Reiniciar", 40),
      m_exitText(m_font, "Exit", 40),
      m_statsText(m_font, "Stats", 40),
      m_titleEnd(m_font, "Game Over", 80),
      m_statsTitle(m_font),
      m_isPlacingTower(false)
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
    initUI();
    updateUI();
}

//Funcion encargada de procesar los inputs del usuario
//Eventos cambiaron un poco en 3.0
void Game::processEvents() {
    while (const auto event = m_window.pollEvent()) {
        // Close window
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }

        if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>()) {
            sf::Vector2i mousePos(mb->position.x, mb->position.y);

            // Common behavior for both PREP and WAVE
            if (m_currentState == GameState::Prep || m_currentState == GameState::Wave) {
                // Click in UI panel (right side)
                if (mousePos.x >= 800) {
                    // Check if clicking upgrade button
                    if (m_upgradeButton.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                        handleTowerUpgrade();
                    }
                    // Otherwise handle tower selection
                    else {
                        handleTowerSelection(mousePos);
                    }
                }
                // Click in game grid
                else {
                    auto gridPos = m_grid.worldToGrid(static_cast<float>(mousePos.x), 
                                                     static_cast<float>(mousePos.y));

                    // Unified tower placement for both states
                    if (m_isPlacingTower) {
                        int cost = Tower::getCost(m_selectedTowerType);

                        if (m_grid.getCell(gridPos.x, gridPos.y) == CellType::Empty && 
                            m_economy.canAfford(cost) && 
                            m_towersPlaced < 51) {

                            m_grid.setCell(gridPos.x, gridPos.y, CellType::Tower); 

                            std::vector<sf::Vector2i> m_ = m_pathfinder.findPath(gridPos);
                            
                            if (m_.empty()) {
                                m_grid.unregisterTower(gridPos.x, gridPos.y);
                                return;
                            }

                            m_economy.spend(cost);
                            m_towers.emplace_back(
                                std::make_unique<Tower>(m_selectedTowerType, gridPos.x, gridPos.y, &m_grid)
                            );
                            m_towersPlaced++;
                            updateUI();
                        }
                        m_isPlacingTower = false;
                    }
                    // Tower selection (common for both states)
                    else {
                        m_selectedTower = nullptr;
                        for (auto& tower : m_towers) {
                            if (tower->getGridX() == gridPos.x && tower->getGridY() == gridPos.y) {
                                m_selectedTower = tower.get();
                                break;
                            }
                        }
                        updateTowerInfo();
                    }
                }
            }
            else if (m_currentState == GameState::EndScreen) {
                if (m_restartButton.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                    restartGame();
                }
                else if (m_exitButton.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                    m_window.close();
                }
                else if (m_statsButton.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                    showStats();
                }
            }
            else if (m_currentState == GameState::StatsScreen) {
                // Botón para volver al EndScreen
                if (mousePos.x >= 300 && mousePos.x <= 500 &&
                    mousePos.y >= 600 && mousePos.y <= 650) {
                    m_currentState = GameState::EndScreen;
                }
            }
        }    
    }
}

//Funcion que se encarga de lo que pasa en el juego
//Aqui se cambian los state y se determina que hacer segun estos
void Game::update(float deltaTime) {
    //Inicia el timer del juego, se reinicia en cada cambio de estado
    m_stateTimer += deltaTime; 

    //Transisiones de state
    //Transicion de state Prep a Wave despues de 10 segundos
    if (m_currentState == GameState::Prep && m_stateTimer >= 10.f) {
        m_currentState = GameState::Wave;
        m_stateTimer = 0.f;
    }
    //Transicion de state Wave a Cooldown despues de 60 segundos
    //Aumenta el numero de wave
    //Hay que implementar logica de gameover por leaks
    else if (m_currentState == GameState::Wave) {
        if (!m_currentWave) {
            // Inicializa la wave con parámetros configurables
            Wave::Config config;
            config.maxEnemies = 40 * m_waveNumber;
            m_currentWave = std::make_unique<Wave>(
                m_waveNumber, 
                m_grid.getSpawnPoints(),
                config, // Configuración default
                &m_grid,
                &m_geneticManager
            );
            std::cout << "=== INICIANDO OLEADA " << m_waveNumber << " ===\n"
              << "Enemigos totales: " << config.maxEnemies << "\n"
              << "Puntos de spawn: " << config.maxSpawnPoints << "\n"
              << "Genomas disponibles: " << m_geneticManager.getCurrentGenomes().size() << "\n";
            recalculatePaths();
            m_gameStats.recordWaveStart(m_waveNumber,config.maxEnemies);
        }
       m_currentWave->update(deltaTime, m_enemies);

        
        auto it = m_enemies.begin();
        while (it != m_enemies.end()) {
            (*it)->update(deltaTime);
            // Eliminar enemigos muertos
            if ((*it)->EndGame()) {
                m_currentState = GameState::EndScreen;
                std::cout << "Game Over" << std::endl;
                //m_window.close();               
                m_enemies.clear();
                break;
            }
            else if (!(*it)->isAlive()) {
                m_currentWave->enemyDead((*it)->getGenome(), (*it)->getStepsTaken());
                //suma el oro ganado
                m_economy.earn((*it)->getBounty());
                // La limpieza del grid se hace automáticamente en el destructor de Enemy
                m_gameStats.recordEnemyDeath((*it)->killer);
                it = m_enemies.erase(it);
            } else {
                ++it;
            }
        }

        for (auto& tower : m_towers) {
            tower->update(deltaTime); 
        }
        //Mas comportamientos del wave irian aqui tambien

        if (m_currentWave->isCompleted()) {  
            for (auto& tower : m_towers) {
                m_gameStats.recordTower(tower->typeToString(tower->getType()),tower->getLevel());
            }
            std::cout << "=== FINALIZANDO OLEADA " << m_waveNumber << " ===\n";      
            m_currentState = GameState::Cooldown;
            m_currentWave.reset();
            m_waveNumber++;
            m_stateTimer = 0.f;            
        }
    }
    //Transicion de state Cooldown a Prep despues de 10 segundos
    else if (m_currentState == GameState::Cooldown && m_stateTimer >= 2.f) {
        //Logica de ganar si completa 3 waves
        if (m_waveNumber > 5) {
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
    if (m_currentState == GameState::EndScreen) {
        m_window.clear(sf::Color::Black);
        
        //m_window.setSize({ 600, 500 });
        

        m_window.draw(m_restartButton);
        m_window.draw(m_exitButton);
        m_window.draw(m_statsButton);

        m_window.draw(m_restartText);
        m_window.draw(m_exitText);
        m_window.draw(m_statsText);
        m_window.draw(m_titleEnd);

        m_window.display();
        return;
    }
    if (m_currentState == GameState::StatsScreen) {
        renderStats();
        return;
    }


    m_window.clear();
    
    //Dibuja matriz
    m_grid.render(m_window);
    
    //Dibuja torres y enemigos
    //Llama a la funcion render de cada objeto
    //render se maneja a nivel de Entity
    for (const auto& tower : m_towers) tower->render(m_window);
    for (const auto& enemy : m_enemies) enemy->render(m_window);

    // Draw UI panel
    m_window.draw(m_uiPanel);
    for (const auto& button : m_towerButtons) m_window.draw(button);
    for (const auto& text : m_towerButtonTexts) m_window.draw(text);
    
    //Dibuja textos
    m_window.draw(m_waveText);
    m_window.draw(m_timerText);
    m_window.draw(m_towersText);
    m_window.draw(m_leaksText);
    m_window.draw(m_coinsText);

    if (m_isPlacingTower) {
        m_window.draw(m_towerGhost);
    }

    // Draw tower info panel
    m_window.draw(m_towerInfoPanel);
    m_window.draw(m_towerInfoText);
    m_window.draw(m_towerLevelText);
    m_window.draw(m_upgradeButton);
    m_window.draw(m_upgradeButtonText);
    
    //Abre la pantalla
    m_window.display();
}


// Función que recalcula los caminos del PathFinding
void Game::recalculatePaths() {
    m_grid.m_precomputedPaths.clear();
    auto spawnPoints = m_currentWave->getActiveSpawnPoints();
    
    // Calcula y guarda el camino desde cada punto de aparición
    for (const auto& spawn : spawnPoints) {
        auto path = m_pathfinder.findPath(spawn);
        m_grid.m_precomputedPaths[spawn] = path;
    }
}

//inicializa el panel de la derecha
void Game::initUI() {
    //Panel de la derecha
    m_uiPanel.setSize({200.f, 800.f});
    m_uiPanel.setPosition({800.f,0.f});
    m_uiPanel.setFillColor(sf::Color(50,50,70));

    //Opcion para seleccion de torres
    const std::vector<Tower::Type> towerTypes = {
        Tower::Type::Archer, Tower::Type::Mage, Tower::Type::Artillery
    };

    //crea rectagulo y lo configura segun tipo de torre
    for (size_t i = 0; i < towerTypes.size(); i++){
        sf::RectangleShape button({20.f, 20.f});
        button.setPosition({810.f, 20.f + static_cast<float>(i) * 50.f}); //pos inicial y baja con cada repeticion
        button.setFillColor(Tower::getColorForType(towerTypes[i]));
        m_towerButtons.push_back(button);
        sf::Text text(m_font);
        text.setCharacterSize(16);
        text.setString(Tower::typeToString(towerTypes[i]) + " (" + 
                      std::to_string(Tower::getCost(towerTypes[i])) + ")");
        text.setPosition({840.f, 20.f + static_cast<float>(i) * 50.f});
        m_towerButtonTexts.push_back(text);
    }
    m_coinsText.setPosition({810.f, 180.f});
    

    // Tower ghost (for placement preview)
    m_towerGhost.setSize({16.f, 16.f});
    m_towerGhost.setOrigin({8.f, 8.f});
    m_towerGhost.setFillColor(sf::Color(255, 255, 255, 150));


    // Panel de información de torre (debajo de coins)
    m_towerInfoPanel.setSize({180.f, 120.f});
    m_towerInfoPanel.setPosition({810.f, 230.f});
    m_towerInfoPanel.setFillColor(sf::Color(70, 70, 90));
    m_towerInfoPanel.setOutlineThickness(1.f);
    m_towerInfoPanel.setOutlineColor(sf::Color::White);

    m_towerInfoText.setPosition({820.f, 240.f});
    m_towerLevelText.setPosition({820.f, 260.f});

    // Botón de upgrade
    m_upgradeButton.setSize({160.f, 30.f});
    m_upgradeButton.setPosition({820.f, 300.f});
    m_upgradeButton.setFillColor(sf::Color(100, 100, 150));
    m_upgradeButton.setOutlineThickness(1.f);
    m_upgradeButton.setOutlineColor(sf::Color::White);

    m_upgradeButtonText.setPosition({830.f, 305.f});

    //Botones de EndScreen
    m_restartButton.setSize({ 400, 75 });
    m_restartButton.setPosition({ 300, 350 });
    m_restartButton.setFillColor(sf::Color(30, 30, 30));
    m_restartButton.setOutlineColor(sf::Color(190, 0, 0));
    m_restartButton.setOutlineThickness(3);

    m_exitButton.setSize({ 400, 75 });
    m_exitButton.setPosition({ 300, 500 });
    m_exitButton.setFillColor(sf::Color(30, 30, 30));
    m_exitButton.setOutlineColor(sf::Color(190, 0, 0));
    m_exitButton.setOutlineThickness(3);

    m_statsButton.setSize({ 400, 75 });
    m_statsButton.setPosition({ 300, 650 });
    m_statsButton.setFillColor(sf::Color(30, 30, 30));
    m_statsButton.setOutlineColor(sf::Color(190, 0, 0));
    m_statsButton.setOutlineThickness(3);

    //Textos
    m_restartText.setPosition({ 420, 360 });
    m_restartText.setFillColor(sf::Color::Black);
    m_restartText.setOutlineColor(sf::Color(190, 0, 0, 230));
    m_restartText.setOutlineThickness(2);

    m_exitText.setPosition({ 460, 510 });
    m_exitText.setFillColor(sf::Color::Black);
    m_exitText.setOutlineColor(sf::Color(190, 0, 0, 230));
    m_exitText.setOutlineThickness(2);

    m_statsText.setPosition({ 450, 660 });
    m_statsText.setFillColor(sf::Color::Black);
    m_statsText.setOutlineColor(sf::Color(190, 0, 0, 230));
    m_statsText.setOutlineThickness(2);

    m_titleEnd.setPosition({ 300, 100 });
    m_titleEnd.setFillColor(sf::Color::Black); // Rojo oscuro + un poco de transparencia
    m_titleEnd.setOutlineColor(sf::Color(190, 0, 0, 230));
    m_titleEnd.setOutlineThickness(2);


    // Configurar elementos de la pantalla de stats
    m_statsBackground.setSize({700.f, 500.f});
    m_statsBackground.setPosition({150.f, 150.f});
    m_statsBackground.setFillColor(sf::Color(50, 50, 70, 220));
    m_statsBackground.setOutlineThickness(2.f);
    m_statsBackground.setOutlineColor(sf::Color::White);
    
    m_statsTitle.setFont(m_font);
    m_statsTitle.setString("Game Statistics");
    m_statsTitle.setCharacterSize(40);
    m_statsTitle.setPosition({400.f, 170.f});
    m_statsTitle.setFillColor(sf::Color::White);
}

void Game::updateTowerGhost(const sf::Vector2i& mousePos) {
    sf::Vector2f pos{static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)};
    if (mousePos.x < 800) { // Only show in game area
        m_towerGhost.setPosition(pos);
    }
}

void Game::handleTowerSelection(const sf::Vector2i& mousePos) {
    sf::Vector2f pos{static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)};
    for (size_t i = 0; i < m_towerButtons.size(); ++i) {
        if (m_towerButtons[i].getGlobalBounds().contains(pos)) {
            int cost = Tower::getCost(static_cast<Tower::Type>(i));
            if (m_economy.canAfford(cost)) {
                m_selectedTowerType = static_cast<Tower::Type>(i);
                m_isPlacingTower = true;
                m_towerGhost.setFillColor(Tower::getColorForType(m_selectedTowerType));
                m_towerGhost.setFillColor(sf::Color(255, 255, 255, 150));
            }
            break;
        }
    }
}


void Game::handleTowerUpgrade() {
    if (!m_selectedTower) return;
    
    int nextLevel = m_selectedTower->getLevel() + 1;
    if (nextLevel > 3) return;
    
    int cost = Tower::getUpgradeCost(nextLevel);
    if (m_economy.canAfford(cost)) {
        m_economy.spend(cost);
        m_selectedTower->upgrade();
        updateTowerInfo();
        updateUI();
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
    m_waveText.setString("Wave:  " + std::to_string(m_waveNumber)   + "/5");
    m_timerText.setString(stateStr + ": " + std::to_string(int(m_stateTimer)) + "s");
    m_towersText.setString("Towers: " + std::to_string(m_towersPlaced) + "/15");
    m_leaksText.setString("Leaks: "  + std::to_string(m_leaks)         + "/5");
    m_coinsText.setString("Coins: " + std::to_string(m_economy.getCoins()));

    if (m_selectedTower) {
        updateTowerInfo();
    }
}

void Game::updateTowerInfo() {
    if (m_selectedTower) {
        m_towerInfoText.setString("Tower: " + Tower::typeToString(m_selectedTower->getType()));
        m_towerLevelText.setString("Level: " + std::to_string(m_selectedTower->getLevel()) + "/3");
        
        // Actualizar texto del botón de upgrade
        if (m_selectedTower->getLevel() < 3) {
            int cost = Tower::getUpgradeCost(m_selectedTower->getLevel() + 1);
            m_upgradeButtonText.setString("Upgrade (" + std::to_string(cost) + ")");
            m_upgradeButton.setFillColor(m_economy.canAfford(cost) ? 
                sf::Color(100, 200, 100) : sf::Color(200, 100, 100));
        } else {
            m_upgradeButtonText.setString("MAX LEVEL");
            m_upgradeButton.setFillColor(sf::Color(150, 150, 150));
        }
    } else {
        m_towerInfoText.setString("No tower selected");
        m_towerLevelText.setString("Level: 0/3");
        m_upgradeButtonText.setString("Upgrade (---)");
        m_upgradeButton.setFillColor(sf::Color(150, 150, 150));
    }
}

void Game::restartGame() {
    m_window.clear();
    m_window.close();
    Game game;
    game.run();
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


void Game::showStats() {
    m_currentState = GameState::StatsScreen;
    m_statsTexts.clear();
    
    if (!m_gameStats.hasData()) {
        // Mostrar mensaje de no datos
        sf::Text noData(m_font);
        noData.setString("No statistics available");
        noData.setCharacterSize(30);
        noData.setPosition({400.f, 300.f});
        noData.setFillColor(sf::Color::White);
        m_statsTexts.push_back(noData);
        return;
    }
    
    // Títulos de columnas
    sf::Text header(m_font);
    header.setString("Wave  Kills    Fitness  Mutations  Towers (L1/L2/L3)  Kills");
    header.setCharacterSize(24);
    header.setPosition({180.f, 230.f});
    header.setFillColor(sf::Color::Yellow);
    m_statsTexts.push_back(header);
    
    // Línea divisoria
    sf::RectangleShape line({700.f - 60.f, 2.f});
    line.setPosition({180.f, 260.f});
    line.setFillColor(sf::Color::White);
    
    // Datos de cada wave
    float yPos = 280.f;
    for (const auto& wave : m_gameStats.getAllStats()) {
        // Wave number
        sf::Text waveText(m_font);
        waveText.setString(std::to_string(wave.waveNumber));
        waveText.setPosition({180.f, yPos});
        waveText.setFillColor(getWaveColor(wave.waveNumber));
        m_statsTexts.push_back(waveText);
        
        // Kills
        std::string killsStr = std::to_string(wave.killedEnemies) + "/" + std::to_string(wave.totalEnemies);
        sf::Text killsText(m_font, killsStr, 20);
        killsText.setPosition({250.f, yPos});
        killsText.setFillColor(sf::Color::White);
        m_statsTexts.push_back(killsText);
        
        // Fitness
        std::stringstream fitnessSS;
        fitnessSS << std::fixed << std::setprecision(2) << wave.averageFitness;
        sf::Text fitnessText(m_font, fitnessSS.str(), 20);
        fitnessText.setPosition({350.f, yPos});
        fitnessText.setFillColor(getFitnessColor(wave.averageFitness));
        m_statsTexts.push_back(fitnessText);
        
        // Mutations
        sf::Text mutationsText(m_font, std::to_string(wave.mutationsCount), 20);
        mutationsText.setPosition({450.f, yPos});
        mutationsText.setFillColor(sf::Color::White);
        m_statsTexts.push_back(mutationsText);
        
        // Towers info
        std::string towersInfo = "A:" + std::to_string(wave.towerStats.archerCount) + "(" +
            std::to_string(wave.towerStats.archerLevel1) + "/" +
            std::to_string(wave.towerStats.archerLevel2) + "/" +
            std::to_string(wave.towerStats.archerLevel3) + ") " +
            "M:" + std::to_string(wave.towerStats.mageCount) + "(" +
            std::to_string(wave.towerStats.mageLevel1) + "/" +
            std::to_string(wave.towerStats.mageLevel2) + "/" +
            std::to_string(wave.towerStats.mageLevel3) + ") " +
            "R:" + std::to_string(wave.towerStats.artilleryCount) + "(" +
            std::to_string(wave.towerStats.artilleryLevel1) + "/" +
            std::to_string(wave.towerStats.artilleryLevel2) + "/" +
            std::to_string(wave.towerStats.artilleryLevel3) + ")";
        
        sf::Text towersText(m_font, towersInfo, 18);
        towersText.setPosition({180.f, yPos + 25.f});
        towersText.setFillColor(sf::Color(200, 200, 255));
        m_statsTexts.push_back(towersText);
        
        // Kills por torre
        std::string killsInfo = "A:" + std::to_string(wave.towerStats.archerKills) + 
                              " M:" + std::to_string(wave.towerStats.mageKills) +
                              " R:" + std::to_string(wave.towerStats.artilleryKills);
        
        sf::Text killsInfoText(m_font, killsInfo, 18);
        killsInfoText.setPosition({180.f, yPos + 50.f});
        killsInfoText.setFillColor(sf::Color(200, 255, 200));
        m_statsTexts.push_back(killsInfoText);
        
        yPos += 90.f; // Espacio para la siguiente wave
    }
}

sf::Color Game::getWaveColor(int waveNumber) const {
    static const sf::Color colors[] = {
        sf::Color(255, 100, 100), // Rojo
        sf::Color(255, 200, 100), // Naranja
        sf::Color(255, 255, 100), // Amarillo
        sf::Color(100, 255, 100), // Verde
        sf::Color(100, 200, 255)  // Azul
    };
    return colors[waveNumber % 5];
}

sf::Color Game::getFitnessColor(float fitness) const {
    if (fitness < 10) return sf::Color(255, 100, 100); // Rojo para bajo fitness
    if (fitness < 20) return sf::Color(255, 255, 100); // Amarillo
    return sf::Color(100, 255, 100); // Verde para alto fitness
}
void Game::renderStats() {
    m_window.clear(sf::Color::Black);
    
    // Fondo semitransparente
    m_window.draw(m_statsBackground);
    m_window.draw(m_statsTitle);
    
    // Mostrar todos los textos de estadísticas
    for (const auto& text : m_statsTexts) {
        m_window.draw(text);
    }
    
    // Botón para volver
    sf::RectangleShape backButton({200.f, 50.f});
    backButton.setPosition({400.f - 100.f, 600.f});
    backButton.setFillColor(sf::Color(100, 100, 150));
    
    sf::Text backText(m_font);
    backText.setString("Back");
    backText.setCharacterSize(30);
    backText.setPosition({400.f, 610.f});
    m_window.draw(backButton);
    m_window.draw(backText);
    m_window.display();
}
