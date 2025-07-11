#include "Game.h"
#include "Wall.h"
#include "PlayerTank.h"
#include "Bullet.h"
#include <string>
#include <fstream>

Game::Game()
    : player(((MAP_WIDTH - 1) / 2)* TILE_SIZE, (MAP_HEIGHT - 2)* TILE_SIZE)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        running = false;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        running = false;
        return;
    }

    window = SDL_CreateWindow("Tanker vs Tanker(Thap Tank Tu Do)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window is not created, error: " << SDL_GetError() << std::endl;
        running = false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer is not created, error: " << SDL_GetError() << std::endl;
        running = false;
    }
    shootSound = Mix_LoadWAV("sounds/shoot.wav");
    if (!shootSound) {
        std::cerr << "Failed to load shoot sound: " << Mix_GetError() << std::endl;
    }


    SDL_Surface* bgSurface = IMG_Load("assets/background.png");
    if (!bgSurface) {
        std::cerr << "Failed to load menu background: " << IMG_GetError() << std::endl;
    }
    else {
        menuBackground = SDL_CreateTextureFromSurface(renderer, bgSurface);
        SDL_FreeSurface(bgSurface);
    }
    SDL_Surface* wallSurface = IMG_Load("assets/wall.png");
    if (!wallSurface) {
        std::cerr << "Failed to load wall texture: " << IMG_GetError() << std::endl;
    }
    else {
        Wall::wallTexture = SDL_CreateTextureFromSurface(renderer, wallSurface);
        SDL_FreeSurface(wallSurface);
    }

    backgroundMusic = Mix_LoadMUS("sounds/background_music.mp3");
    if (!backgroundMusic) {
        std::cerr << "Failed to load background music: " << Mix_GetError() << std::endl;
    }

    SDL_Surface* tutorialSurface = IMG_Load("assets/tutorial.png");
    if (!tutorialSurface) {
        std::cerr << "Failed to load tutorial image: " << IMG_GetError() << std::endl;
    }
    else {
        tutorialTexture = SDL_CreateTextureFromSurface(renderer, tutorialSurface);
        SDL_FreeSurface(tutorialSurface);
    }
    SDL_Surface* pauseSurface = IMG_Load("assets/pause.png");
    if (!pauseSurface) {
        std::cerr << "Failed to load pause button image: " << IMG_GetError() << std::endl;
    }
    else {
        pauseButtonTexture = SDL_CreateTextureFromSurface(renderer, pauseSurface);
        SDL_FreeSurface(pauseSurface);
    }






    //generateWalls();
    generateWallsFromFile("map.txt");
    spawnEnemyTank();
    loadHighScore();

}

Game::~Game()
{
    Mix_FreeChunk(shootSound);
    Mix_CloseAudio();
    if (menuBackground) {
        SDL_DestroyTexture(menuBackground);
    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    if (Wall::wallTexture) {
        SDL_DestroyTexture(Wall::wallTexture);
        Wall::wallTexture = nullptr;
    }
    if (tutorialTexture) {
        SDL_DestroyTexture(tutorialTexture);
        tutorialTexture = nullptr;
    }
    if (pauseButtonTexture) {
        SDL_DestroyTexture(pauseButtonTexture);
        pauseButtonTexture = nullptr;
    }




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

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                if (mouseX >= soundButtonRect.x && mouseX <= soundButtonRect.x + soundButtonRect.w * 3 &&
                    mouseY >= soundButtonRect.y && mouseY <= soundButtonRect.y + soundButtonRect.h)
                {
                    soundOn = !soundOn; // Bật/Tắt âm thanh

                    if (soundOn) {
                        Mix_ResumeMusic();
                    }
                    else {
                        Mix_PauseMusic();
                    }
                }

                if (currentState == PLAYING) {
                    if (event.type == SDL_MOUSEBUTTONDOWN) {
                        int mouseX = event.button.x;
                        int mouseY = event.button.y;

                        if (mouseX >= pauseButtonRect.x && mouseX <= pauseButtonRect.x + pauseButtonRect.w &&
                            mouseY >= pauseButtonRect.y && mouseY <= pauseButtonRect.y + pauseButtonRect.h) {
                            currentState = PAUSED;
                        }
                    }
                }

            }
            if (currentState == TUTORIAL) {
                

                if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_KEYDOWN) {
                    currentState = MENU;
                }
                renderTutorial();
                continue;
            }
            if (currentState == PAUSED) {
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int mouseX = event.button.x;
                    int mouseY = event.button.y;

                    // Nút Resume
                    SDL_Rect resumeButtonRect = { SCREEN_WIDTH / 2 - 100, 200, 200, 50 };
                    if (mouseX >= resumeButtonRect.x && mouseX <= resumeButtonRect.x + resumeButtonRect.w &&
                        mouseY >= resumeButtonRect.y && mouseY <= resumeButtonRect.y + resumeButtonRect.h) {
                        currentState = PLAYING; // Tiếp tục chơi
                    }

                    // Nút Back to Menu
                    SDL_Rect menuButtonRect = { SCREEN_WIDTH / 2 - 100, 300, 200, 50 };
                    if (mouseX >= menuButtonRect.x && mouseX <= menuButtonRect.x + menuButtonRect.w &&
                        mouseY >= menuButtonRect.y && mouseY <= menuButtonRect.y + menuButtonRect.h) {
                        enemyNumber = 1;
                        walls.clear();
                        enemies.clear();
                        player.resetPosition();
                        generateWallsFromFile("map.txt");
                        spawnEnemyTank();
                        currentState = MENU;
                    }
                }
            }


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
                    case SDLK_SPACE:
                        player.shoot();
                        Mix_PlayChannel(-1, shootSound, 0);
                        break;
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
        else if (currentState == PAUSED) {
            renderPauseMenu();
        }

        SDL_Delay(16);
    }

    TTF_Quit();
}

void Game::handleMenuEvents(SDL_Event& event)
{
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_SPACE) {
            if (!playerWon) {
                enemyNumber = 1;
            }
            if (soundOn && Mix_PlayingMusic()) {
                Mix_HaltMusic();
            }

 

            walls.clear();
            enemies.clear();
            player.resetPosition();
            //generateWalls();
			generateWallsFromFile("map.txt");
            spawnEnemyTank();
            currentState = PLAYING;
        }
        else if (event.key.keysym.sym == SDLK_ESCAPE) {
            running = false;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN) { 
        int mouseX = event.button.x;
        int mouseY = event.button.y;

        // Kiểm tra click vào nút Tutorial
        SDL_Rect tutorialButtonRect = { SCREEN_WIDTH / 2 - 100, 420, 200, 50 };
        if (mouseX >= tutorialButtonRect.x && mouseX <= tutorialButtonRect.x + tutorialButtonRect.w &&
            mouseY >= tutorialButtonRect.y && mouseY <= tutorialButtonRect.y + tutorialButtonRect.h  )
        {
            currentState = TUTORIAL;
        }

    }

}

void Game::renderMenu()
{
    if (soundOn && !Mix_PlayingMusic()) {
        Mix_PlayMusic(backgroundMusic, -1);
    }

    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);


    if (menuBackground) {
        SDL_Rect bgRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
        SDL_RenderCopy(renderer, menuBackground, NULL, &bgRect);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);
    }

    TTF_Font* font = TTF_OpenFont("font/mine.ttf", 38);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Color color = { 255, 255, 255 };

    std::string title;
    if (currentState == GAME_OVER) {
        title = playerWon ? "You Win!" : "GAME OVER";
    }
    else {
        title = "tank";
    }

    std::string instruction;
	if (currentState == GAME_OVER) {
		instruction = playerWon ? "Press SPACE to continue" : "Press SPACE to restart";
	}
	else {
		instruction = "Press SPACE to start";
	}
    std::string levelText = "Level " + std::to_string(enemyNumber);
    std::string highText = "High Score: " + std::to_string(highScore);

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

    SDL_Surface* levelSurface = TTF_RenderText_Solid(font, levelText.c_str(), color);
    SDL_Texture* levelTexture = SDL_CreateTextureFromSurface(renderer, levelSurface);
    SDL_Rect levelRect = { 20, 20, levelSurface->w, levelSurface->h };
    SDL_RenderCopy(renderer, levelTexture, NULL, &levelRect);
    SDL_FreeSurface(levelSurface);
    SDL_DestroyTexture(levelTexture);


    SDL_Surface* highSurface = TTF_RenderText_Solid(font, highText.c_str(), color);
    SDL_Texture* highTexture = SDL_CreateTextureFromSurface(renderer, highSurface);
    SDL_Rect highRect = { SCREEN_WIDTH / 2 - highSurface->w / 2, 360, highSurface->w, highSurface->h };
    SDL_RenderCopy(renderer, highTexture, NULL, &highRect);
    SDL_FreeSurface(highSurface);
    SDL_DestroyTexture(highTexture);




    TTF_CloseFont(font);



    TTF_Font* smallFont = TTF_OpenFont("font/mine.ttf", 24);
    if (smallFont) {
        SDL_Color textColor = { 255, 255, 255 };
        std::string soundText = soundOn ? "Sound: ON" : "Sound: OFF";

        SDL_Surface* textSurface = TTF_RenderText_Solid(smallFont, soundText.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        SDL_Rect textRect = { soundButtonRect.x, soundButtonRect.y, textSurface->w, textSurface->h };
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
        TTF_CloseFont(smallFont);

    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect tutorialButtonRect = { SCREEN_WIDTH / 2 - 100, 320, 200, 50 };


    TTF_Font* fonts = TTF_OpenFont("font/mine.ttf", 34);
    if (fonts) {
        SDL_Color color = { 0 , 255, 255 };
        SDL_Surface* surface = TTF_RenderText_Solid(fonts, "TUTORIAL", color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect textRect = { tutorialButtonRect.x + 30, tutorialButtonRect.y + 120, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, NULL, &textRect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
        TTF_CloseFont(fonts);
    }

    SDL_RenderPresent(renderer);

}

//trong khi chơi
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
    TTF_Font* font = TTF_OpenFont("font/mine.ttf", 24);
    if (font) {
        SDL_Color color = { 0, 0, 0 };
        std::string levelText = "Level " + std::to_string(enemyNumber);
        std::string highText = "High Score: " + std::to_string(highScore);

        SDL_Surface* surface = TTF_RenderText_Solid(font, levelText.c_str(), color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect levelRect = { 10, 10, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, NULL, &levelRect);

        SDL_Surface* highSurface = TTF_RenderText_Solid(font, highText.c_str(), color);
        SDL_Texture* highTexture = SDL_CreateTextureFromSurface(renderer, highSurface);
        SDL_Rect highRect = { 180 , 10 , highSurface->w, highSurface->h };
        SDL_RenderCopy(renderer, highTexture, NULL, &highRect);
        SDL_FreeSurface(highSurface);
        SDL_DestroyTexture(highTexture);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
        TTF_CloseFont(font);
    }
    if (pauseButtonTexture) {
        SDL_RenderCopy(renderer, pauseButtonTexture, NULL, &pauseButtonRect);
    }

    SDL_RenderPresent(renderer);


}

//void Game::generateWalls() {
//    for (int i = 3; i < MAP_HEIGHT - 3; i += 2) {
//        for (int j = 3; j < MAP_WIDTH - 3; j += 2) {
//            Wall w = Wall(j * TILE_SIZE, i * TILE_SIZE);
//            walls.push_back(w);
//        }
//    }
//}
void Game::generateWallsFromFile(const string& filename)
{
    walls.clear();
    ifstream file(filename);
    string line;
    int row = 0;

    while (getline(file, line) && row < MAP_HEIGHT) {
        for (int col = 0; col < min((int)line.size(), MAP_WIDTH) - 1; col++) {
            if (line[col] == '1') {
                int x = col * TILE_SIZE + TILE_SIZE;
                int y = row * TILE_SIZE + TILE_SIZE;
                walls.emplace_back(x, y);
            }
        }
        row++;
    }
	for (int i = 0; i < walls.size(); i++)
	{
		walls[i].active = true;
	}
	file.close();
}


//trạng thái game 
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
		enemyNumber++;
        currentState = GAME_OVER;
    }

    for (auto& enemy : enemies) {
        for (auto& bullet : enemy.bullets) {
            if (SDL_HasIntersection(&bullet.rect, &player.rect)) {
                playerWon = false;
                if (enemyNumber > highScore) {
                    highScore = enemyNumber;
                    saveHighScore();
                }
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

void Game::loadHighScore() {
    std::ifstream file("highscore.txt");
    if (file.is_open()) {
        file >> highScore;
        file.close();
    }
    else {
        highScore = 1;
    }
}

void Game::saveHighScore() {
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << highScore;
        file.close();
    }
}
void Game::renderTutorial()
{
    SDL_RenderClear(renderer);

    if (tutorialTexture) {
        SDL_Rect fullScreen = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
        SDL_RenderCopy(renderer, tutorialTexture, NULL, &fullScreen);
    }

    SDL_RenderPresent(renderer);
}

void Game::renderPauseMenu()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180); // nền mờ
    SDL_RenderClear(renderer);

    TTF_Font* font = TTF_OpenFont("font/mine.ttf", 36);
    if (!font) return;

    SDL_Color color = { 255, 255, 255 };

    // Resume Button
    SDL_Rect resumeButton = { SCREEN_WIDTH / 2 - 100, 200, 200, 50 };
    SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
    SDL_RenderFillRect(renderer, &resumeButton);

    SDL_Surface* resumeSurface = TTF_RenderText_Solid(font, "RESUME", color);
    SDL_Texture* resumeTexture = SDL_CreateTextureFromSurface(renderer, resumeSurface);
    SDL_Rect resumeText = { resumeButton.x + 30, resumeButton.y + 10, resumeSurface->w, resumeSurface->h };
    SDL_RenderCopy(renderer, resumeTexture, NULL, &resumeText);
    SDL_FreeSurface(resumeSurface);
    SDL_DestroyTexture(resumeTexture);

    // Back to Menu Button
    SDL_Rect menuButton = { SCREEN_WIDTH / 2 - 100, 300, 200, 50 };
    SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
    SDL_RenderFillRect(renderer, &menuButton);

    SDL_Surface* menuSurface = TTF_RenderText_Solid(font, "MENU", color);
    SDL_Texture* menuTexture = SDL_CreateTextureFromSurface(renderer, menuSurface);
    SDL_Rect menuText = { menuButton.x + 50, menuButton.y + 10, menuSurface->w, menuSurface->h };
    SDL_RenderCopy(renderer, menuTexture, NULL, &menuText);
    SDL_FreeSurface(menuSurface);
    SDL_DestroyTexture(menuTexture);

    TTF_CloseFont(font);

    SDL_RenderPresent(renderer);
}
