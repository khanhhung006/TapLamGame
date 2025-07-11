#ifndef ENEMY_TANK_H
#define ENEMY_TANK_H
#include<iostream>
#include<string>
#include<algorithm>
#include<vector>
#include<SDL.h>
#include "Wall.h"
#include "Bullet.h"
class EnemyTank {
public:
	int x, y;
	int dirX, dirY;
	int moveDelay, shootDelay;
	SDL_Rect rect;
	bool active;
	vector<Bullet> bullets;
	EnemyTank(int startX, int startY);
	void move(const vector<Wall>& walls);
	void shoot();
	void updateBullets();
	void render(SDL_Renderer* renderer);
	

};

#endif