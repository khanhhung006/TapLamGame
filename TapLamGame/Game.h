#ifndef GAME_H
#define GAME_H
#include<iostream>
#include<string>
#include<vector>
#include"Wall.h"
#include"PlayerTank.h"
#include"EnemyTank.h"
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include <SDL_mixer.h>



const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int TILE_SIZE = 40;
const int MAP_WIDTH = SCREEN_WIDTH / TILE_SIZE;
const int MAP_HEIGHT = SCREEN_HEIGHT / TILE_SIZE;


enum GameState
{
	MENU,
	PLAYING,
	GAME_OVER,
	TUTORIAL,
	PAUSED
};
class Game
{
public:
	bool running = true;
	SDL_Window* window;
	SDL_Renderer* renderer;
	vector<Wall> walls;
	PlayerTank player;
	int enemyNumber = 1;
	int highScore = 1;

	SDL_Texture* menuBackground = nullptr;
	SDL_Texture* tutorialTexture = nullptr;
	SDL_Texture* pauseButtonTexture = nullptr;
	SDL_Rect pauseButtonRect = { SCREEN_WIDTH - 60, 10, 50, 50 }; // góc ph?i 50x50


	vector<EnemyTank> enemies;
	
	GameState currentState = MENU;
	bool playerWon = false;

	Mix_Chunk* shootSound = nullptr;
	Mix_Music* backgroundMusic = nullptr;
	bool soundOn = true;

	SDL_Rect soundButtonRect = { SCREEN_WIDTH - 150, 20, 40, 40 };




	Game();
	void render();
	void run();
	~Game();
	//void generateWalls();
	void generateWallsFromFile(const std::string& filename);
	void handleMenuEvents(SDL_Event& event);
	void update();
	void spawnEnemyTank();
	void renderMenu();
	void loadHighScore();
	void saveHighScore();
	void renderTutorial();
	void renderPauseMenu();

};
#endif