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

/*
 * Actualiza la lógica del juego cada frame
 * Maneja las transiciones entre estados del juego:
 * - Prep -> Wave después de 8 segundos
 * - Wave -> Cooldown cuando termina la oleada
 * - Cooldown -> Prep despues de 5 segundos
 */
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

/*
 * Inicia una nueva oleada de enemigos
 * Configura los parametros de la oleada basados en el numero de wave actual
 */
void Game::startNewWave() {
    m_currentState = GameState::Wave;
    m_stateTimer = 0.f;

    Wave::Config config;
    config.spawnInterval = 1.0f;
    config.enemiesPerSpawn = 1;

    // Formula mejorada para tamaño de oleadas
    config.maxEnemies = 20 * m_waveNumber; // Oleada 1: 20, Oleada 2: 40, Oleada 3: 60

    config.waveDuration = 60.0f;
    config.maxSpawnPoints = std::min(3 + m_waveNumber, 6);
    config.totalWaves = 3;

    m_currentWave = std::make_unique<Wave>(
        m_waveNumber,
        m_grid.getSpawnPoints(),
        config,
        &m_grid,
        &m_geneticManager);

    // Solo genera población inicial si es la primera oleada
    if (m_waveNumber == 1) {
        generateInitialEnemies(config.maxEnemies);
    }

    std::cout << "=== INICIANDO OLEADA " << m_waveNumber << " ===\n"
              << "Enemigos totales: " << config.maxEnemies << "\n"
              << "Puntos de spawn: " << config.maxSpawnPoints << "\n"
              << "Genomas disponibles: " << m_geneticManager.getCurrentGenomes().size() << "\n";
}

void Game::generateInitialEnemies(int totalEnemies) {
    // Verifica si ya hay genomas generados
    if (m_geneticManager.getCurrentGenomes().empty()) {
        std::cout << "Generando población inicial de " << totalEnemies << " enemigos\n";

        // 1. Crea 2 de cada tipo (8 enemigos base)
        for (int type = 0; type < 4; ++type) {
            m_geneticManager.generateEnemyGenome(static_cast<EnemyType>(type));
            m_geneticManager.generateEnemyGenome(static_cast<EnemyType>(type));
        }

        // 2. Crea el resto de forma aleatoria
        for (int i = 8; i < totalEnemies; ++i) {
            EnemyType randomType = static_cast<EnemyType>(rand() % 4);
            m_geneticManager.generateEnemyGenome(randomType);
        }
    } else {
        std::cout << "Población existente detectada ("
                 << m_geneticManager.getCurrentGenomes().size()
                 << " genomas). No se generan nuevos.\n";
    }
}


/*
 * Actualiza el estado de la oleada actual
 * Genera nuevos enemigos y actualiza los existentes
 */
void Game::updateWaveState(float deltaTime) {
    m_currentWave->update(deltaTime);

    // Obtiene genomas para nuevos enemigos y los spawnea
    auto genomes = m_currentWave->getGenomesForNextSpawn(); // Ahora devuelve Ptr
    for (const auto& genome : genomes) {
        spawnEnemy(genome); // genome ya es Ptr
    }

    updateEnemies(deltaTime);

    // Verifica si la oleada ha terminado
    if (m_currentWave->isCompleted()) {
        endCurrentWave();
    }
}
/*
 * Finaliza la oleada actual
 * Evalua la generacion de enemigos, crea nueva generación
 * y elimina los enemigos existentes
 */
void Game::endCurrentWave() {
    std::cout << "=== FINALIZANDO OLEADA " << m_waveNumber << " ===\n";

    const auto& waveConfig = m_currentWave->getWaveConfig();

    if (m_waveNumber < waveConfig.totalWaves) {
        // 1. Evaluar la generacion actual (calcula fitness)
        m_geneticManager.evaluateGeneration(m_enemies);

        // 2. Preparar para la siguiente oleada
        int nextWaveNumber = m_waveNumber + 1;
        int requiredPopulation = 20 + (20 * (nextWaveNumber - 1));

        // 3. Crear nueva generación BASADA EN LOS GENOMAS ACTUALES
        m_geneticManager.createNextGeneration(requiredPopulation);

        // 4. Limpiar enemigos existentes (pero no los genomas)
        m_enemies.clear();

        // 5. Incrementa numero de oleada
        m_waveNumber++;
    } else {
        std::cout << "JUEGO COMPLETADO - Todas las oleadas finalizadas\n";
    }

    // Cambiar estado y reiniciar temporizador
    m_currentState = GameState::Cooldown;
    m_stateTimer = 0.f;
}


/*
 * Crea un nuevo enemigo en un punto de spawn aleatorio
 * Recibe Genoma compartido que define los atributos del enemigo
 */
void Game::spawnEnemy(const EnemyGenome::Ptr& genome) {
    const auto& spawnPoints = m_currentWave->getSpawnPoints();
    if (spawnPoints.empty()) return;

    static size_t spawnIndex = 0;
    const auto& spawnPos = spawnPoints[spawnIndex];
    spawnIndex = (spawnIndex + 1) % spawnPoints.size();

    m_enemies.emplace_back(std::make_unique<Enemy>(genome, spawnPos.x, spawnPos.y, &m_grid));

    // Log detallado del spawn
    const auto& attrs = genome->getAttributes();
    std::cout << "Spawning Enemy ID:" << genome->getId()
              << " Type:" << static_cast<int>(genome->getType())
              << " at (" << spawnPos.x << "," << spawnPos.y << ")"
              << " [H:" << attrs.health
              << " S:" << attrs.speed
              << " A:" << attrs.armor
              << " MR:" << attrs.magicResist << "]\n";
}


//Genera enemigos para la oleada actual ,Solo genera un enemigo por llamado, usando el primer genoma disponible
void Game::spawnWaveEnemies() {
    // Obtiene genomas para el siguiente spawn
    auto genomes = m_currentWave->getGenomesForNextSpawn();
    if (!genomes.empty()) {
        // Obtiene puntos de spawn disponibles
        auto spawnPoints = m_currentWave->getSpawnPoints();
        if (!spawnPoints.empty()) {
            // Selecciona punto de spawn de forma rotativa
            static size_t spawnIndex = 0;
            spawnIndex = (spawnIndex + 1) % spawnPoints.size();
            // Crea un enemigo con el primer genoma disponible
            spawnEnemy(genomes[0]);
        }
    }
}

//Actualiza todos los enemigos activos y Eliimina los enemigos que han muerto
void Game::updateEnemies(float deltaTime) {
    for (auto it = m_enemies.begin(); it != m_enemies.end(); ) {
        (*it)->update(deltaTime);
        if (!(*it)->isAlive()) {
            logEnemyDeath(*it); // Registra la muerte
            it = m_enemies.erase(it); // Elimina el enemigo
        } else {
            ++it;
        }
    }
}
//Para registrar informacion de enemigos muertos
void Game::logEnemyDeath(const std::unique_ptr<Enemy>& enemy) {
    std::cout << "Enemigo murio - Tipo: " << static_cast<int>(enemy->getType())
              << " Pasos: " << enemy->getStepsTaken()
              << " Fitness: " << enemy->getGenome()->getFitness()
              << " Pos: (" << enemy->getGridX() << "," << enemy->getGridY() << ")\n";
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
