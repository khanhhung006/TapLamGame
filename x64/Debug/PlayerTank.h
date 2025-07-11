#ifndef PLAYER_TANK_H
#define PLAYER_TANK_H
#include<iostream>
#include<string>
#include<algorithm>
#include<vector>
#include<SDL.h>
#include "Wall.h"
#include "Bullet.h"
#include<SDL_image.h>
class PlayerTank {
public:
	int x, y;
	int dirX, dirY;
	SDL_Rect rect;
	vector<Bullet> bullets;
	PlayerTank(int startX, int startY);
	void move(int dx,int dy, const vector<Wall>walls);
	void render(SDL_Renderer* renderer);
	void shoot();
	void updateBullets();
	void resetPosition();

};

#endif