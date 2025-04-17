#include"PlayerTank.h"
#include"Game.h"

using namespace std;
PlayerTank::PlayerTank(int startX, int startY) {
	x = startX;
	y = startY;
	dirX = 0;
	dirY = -5;
	rect = { x , y , TILE_SIZE , TILE_SIZE };
}
void PlayerTank::render(SDL_Renderer* renderer)
{

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &rect);
	
	SDL_Rect gun;
	int gunWidth = 20, gunHeight = 20;


	if (dirX == 0 && dirY == -5) {  // H??ng lên
		gun = { x + TILE_SIZE / 2 - gunWidth / 2, y + TILE_SIZE / 4 - gunHeight / 2, gunWidth, gunHeight };
	}
	else if (dirX == 0 && dirY == 5) {  // H??ng xu?ng
		gun = { x + TILE_SIZE / 2 - gunWidth / 2, y + (3 * TILE_SIZE) / 4 - gunHeight / 2, gunWidth, gunHeight };
	}
	else if (dirX == -5 && dirY == 0) {  // H??ng trái
		gun = { x + TILE_SIZE / 4 - gunWidth / 2, y + TILE_SIZE / 2 - gunHeight / 2, gunHeight, gunWidth };
	}
	else if (dirX == 5 && dirY == 0) {  // H??ng ph?i
		gun = { x + (3 * TILE_SIZE) / 4 - gunWidth / 2, y + TILE_SIZE / 2 - gunHeight / 2, gunHeight, gunWidth };
	}
	else {
		// M?c ??nh n?u ch?a di chuy?n (h??ng lên)
		gun = { x + TILE_SIZE / 2 - gunWidth / 2, y + TILE_SIZE / 4 - gunHeight / 2, gunWidth, gunHeight };
	}

	// V? nòng súng (hình ch? nh?t nh?)
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(renderer, &gun);

	for (auto& bullet : bullets)
	{
		bullet.render(renderer);
	}
}
void PlayerTank::move(int dx, int dy, const vector<Wall> walls)
{
	int newX = x + dx;
	int newY = y + dy;
	this->dirX = dx;
	this->dirY = dy;

	SDL_Rect newRect = { newX , newY , TILE_SIZE , TILE_SIZE };
	for (int i = 0;i < walls.size();i++)
	{
		if (walls[i].active && SDL_HasIntersection(&newRect, &walls[i].rect))
		{
			return;
		}
	}
	if (newX >= TILE_SIZE && newX <= SCREEN_WIDTH - TILE_SIZE * 2 && newY >= TILE_SIZE && newY <= SCREEN_HEIGHT - TILE_SIZE * 2)
	{
		x = newX;
		y = newY;
		rect.x = x;
		rect.y = y;
	}
}
void PlayerTank::shoot()
{
	bullets.push_back(Bullet(x + TILE_SIZE / 2 - 5, y + TILE_SIZE / 2 - 5, this->dirX, this-> dirY));

}
//Dòng d??i khó hi?u quá
void PlayerTank::updateBullets()
{
	for (auto& bullet : bullets)
	{
		bullet.move();
	}
	bullets.erase(remove_if(bullets.begin(), bullets.end(), [](Bullet& b) { return !b.active; }), bullets.end());
}
void PlayerTank::resetPosition() {
	x = ((MAP_WIDTH - 1) / 2) * TILE_SIZE;
	y = (MAP_HEIGHT - 2) * TILE_SIZE;
	rect.x = x;
	rect.y = y;
	bullets.clear();
}

