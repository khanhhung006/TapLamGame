#include "Game.h"
#include "Wall.h"
#include "PlayerTank.h"
#include "Bullet.h"

Game::Game()
    : player(((MAP_WIDTH - 1) / 2)* TILE_SIZE, (MAP_HEIGHT - 2)* TILE_SIZE)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        running = false;
    }

    window = SDL_CreateWindow("Game Battle City v1.0", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window is not created, error: " << SDL_GetError() << std::endl;
        running = false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer is not created, error: " << SDL_GetError() << std::endl;
        running = false;
    }

    generateWalls();
    spawnEnemyTank();
}

Game::~Game()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::run()
{
    if (TTF_Init() < 0) {
        std::cerr << "Failed to initialize TTF: " << TTF_GetError() << std::endl;
        running = false;
        return;
    }

    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;

            if (currentState == MENU || currentState == GAME_OVER) {
                handleMenuEvents(event);
            }
            else if (currentState == PLAYING) {
                // X? lý di chuy?n và b?n t?i ?ây luôn:
                if (event.type == SDL_KEYDOWN) {
                    switch (event.key.keysym.sym) {
                    case SDLK_UP: player.move(0, -5, walls); break;
                    case SDLK_DOWN: player.move(0, 5, walls); break;
                    case SDLK_LEFT: player.move(-5, 0, walls); break;
                    case SDLK_RIGHT: player.move(5, 0, walls); break;
                    case SDLK_SPACE: player.shoot(); break;
                    }
                }
            }
        }

        if (currentState == MENU || currentState == GAME_OVER)
            renderMenu();
        else if (currentState == PLAYING) {
            update();
            render();
        }

        SDL_Delay(16);
    }

    TTF_Quit();
}

void Game::handleMenuEvents(SDL_Event& event)
{
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_SPACE) {
            walls.clear();
            enemies.clear();
            player.resetPosition();
            generateWalls();
            spawnEnemyTank();
            currentState = PLAYING;
        }
        else if (event.key.keysym.sym == SDLK_ESCAPE) {
            running = false;
        }
    }
}

void Game::renderMenu()
{
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    TTF_Font* font = TTF_OpenFont("font/arial.ttf", 28);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Color color = { 255, 255, 255 };
    std::string title;
    if (currentState == GAME_OVER) {
        title = playerWon ? "You Win!" : "You Lose!";
    }
    else {
        title = "Battle City";
    }

    std::string instruction = "Press SPACE to start";

    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, title.c_str(), color);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = { SCREEN_WIDTH / 2 - titleSurface->w / 2, 150, titleSurface->w, titleSurface->h };
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    SDL_Surface* insSurface = TTF_RenderText_Solid(font, instruction.c_str(), color);
    SDL_Texture* insTexture = SDL_CreateTextureFromSurface(renderer, insSurface);
    SDL_Rect insRect = { SCREEN_WIDTH / 2 - insSurface->w / 2, 250, insSurface->w, insSurface->h };
    SDL_RenderCopy(renderer, insTexture, NULL, &insRect);
    SDL_FreeSurface(insSurface);
    SDL_DestroyTexture(insTexture);

    TTF_CloseFont(font);
    SDL_RenderPresent(renderer);
}

void Game::render()
{
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 1; i < MAP_HEIGHT - 1; i++) {
        for (int j = 1; j < MAP_WIDTH - 1; j++) {
            SDL_Rect tile = { j * TILE_SIZE, i * TILE_SIZE , TILE_SIZE , TILE_SIZE };
            SDL_RenderFillRect(renderer, &tile);
        }
    }
    for (int i = 0; i < walls.size(); i++) {
        walls[i].render(renderer);
    }
    player.render(renderer);
    for (auto& enemy : enemies) {
        enemy.render(renderer);
    }
    SDL_RenderPresent(renderer);
}

void Game::generateWalls() {
    for (int i = 3; i < MAP_HEIGHT - 3; i += 2) {
        for (int j = 3; j < MAP_WIDTH - 3; j += 2) {
            Wall w = Wall(j * TILE_SIZE, i * TILE_SIZE);
            walls.push_back(w);
        }
    }
}


void Game::update()
{
    player.updateBullets();

    for (auto& bullet : player.bullets) {
        for (auto& wall : walls) {
            if (wall.active && SDL_HasIntersection(&bullet.rect, &wall.rect)) {
                bullet.active = false;
                wall.active = false;
            }
        }
    }

    for (auto& enemy : enemies) {
        enemy.move(walls);
        enemy.updateBullets();
        if (rand() % 100 < 2) {
            enemy.shoot();
        }
    }

    for (auto& enemy : enemies) {
        for (auto& bullet : enemy.bullets) {
            for (auto& wall : walls) {
                if (wall.active && SDL_HasIntersection(&bullet.rect, &wall.rect)) {
                    bullet.active = false;
                    wall.active = false;
                    break;
                }
            }
        }
    }

    for (auto& bullet : player.bullets) {
        for (auto& enemy : enemies) {
            if (enemy.active && SDL_HasIntersection(&bullet.rect, &enemy.rect)) {
                bullet.active = false;
                enemy.active = false;
                break;
            }
        }
    }

    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](EnemyTank& e) { return !e.active; }), enemies.end());

    if (enemies.empty()) {
        playerWon = true;
        currentState = GAME_OVER;
    }

    for (auto& enemy : enemies) {
        for (auto& bullet : enemy.bullets) {
            if (SDL_HasIntersection(&bullet.rect, &player.rect)) {
                playerWon = false;
                currentState = GAME_OVER;
                break;
            }
        }
    }
}

void Game::spawnEnemyTank()
{
    enemies.clear();
    for (int i = 0; i < enemyNumber; i++) {
        int ex, ey;
        bool validPosition = false;
        while (!validPosition) {
            ex = (rand() % (MAP_WIDTH - 2) + 1) * TILE_SIZE;
            ey = (rand() % (MAP_HEIGHT - 2) + 1) * TILE_SIZE;
            validPosition = true;
            for (const auto& wall : walls) {
                if (wall.active && wall.x == ex && wall.y == ey) {
                    validPosition = false;
                    break;
                }
            }
        }
        enemies.emplace_back(ex, ey);
    }
}