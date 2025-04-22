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
	GAME_OVER
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

	vector<EnemyTank> enemies;
	
	GameState currentState = MENU;
	bool playerWon = false;

	Mix_Chunk* shootSound = nullptr;


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

};
#endif