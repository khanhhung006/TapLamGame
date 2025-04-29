#include"Wall.h"
#include"Game.h"

SDL_Texture* Wall::wallTexture = nullptr;

Wall::Wall(int startX,int startY)
{
	x = startX;
	y = startY;
	rect = { x , y , TILE_SIZE , TILE_SIZE };
	active = true;
}
void Wall::render(SDL_Renderer* renderer)
{
	if (active && wallTexture) {
		SDL_RenderCopy(renderer, wallTexture, NULL, &rect);
	}
}

