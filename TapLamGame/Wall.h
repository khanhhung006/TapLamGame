#ifndef WALL_H
#define WALL_H
#include<iostream>
#include<string>
#include<vector>
#include<SDL.h>
#include<SDL_image.h>
using namespace std;

class Wall
{
public:
	int x, y;
	SDL_Rect rect;
	bool active;
	Wall(int startX, int startY);
	void render(SDL_Renderer* renderer);
};

#endif
