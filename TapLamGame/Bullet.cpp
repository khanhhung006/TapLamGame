#include"Bullet.h"
#include"Game.h"
#include"PlayerTank.h"
Bullet::Bullet(int startX, int startY, int dirX, int dirY)
{
	x = startX;
	y = startY;
	dx = dirX;
	dy = dirY;
	active = true;
	rect = { x,y ,10 ,10 };
}
void Bullet::move()
{
	x += dx;
	y += dy;
	rect.x = x;
	rect.y = y;
	if (x < TILE_SIZE || x > SCREEN_WIDTH - TILE_SIZE || y < TILE_SIZE || y > SCREEN_WIDTH - TILE_SIZE)
	{
		active = false;
	}
}
void Bullet::render(SDL_Renderer* renderer)
{
	if (active == true) {
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
		SDL_RenderFillRect(renderer, &rect);
	}
}