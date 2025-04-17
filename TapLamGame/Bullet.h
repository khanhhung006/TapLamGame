#ifndef BULLET_H
#define BULLET_H
#include<iostream>
#include<string>
#include<vector>
#include<SDL.h>
#include<SDL_image.h>

class Bullet
{
public:
	int x, y;
	int dx, dy;
	SDL_Rect rect;
	bool active;
	Bullet(int startX, int startY, int dirX, int dirY);
	void move();
	void render(SDL_Renderer* renderer);
};


#endif
