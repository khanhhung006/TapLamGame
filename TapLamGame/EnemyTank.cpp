#include"EnemyTank.h"
#include"Game.h"

using namespace std;
EnemyTank::EnemyTank(int startX, int startY) {
	moveDelay = 15;
	shootDelay = 5;
	x = startX;
	y = startY;
	rect = { x , y , TILE_SIZE , TILE_SIZE };
	active = true;
	dirX = 0;
	dirY = 5;
}
void EnemyTank::move(const vector<Wall>& walls)
{
	if (--moveDelay > 0) {
		return;
	}
	moveDelay = 15;
	int r = rand() % 4;
	if (r == 0) {
		this->dirX = 0;
		this->dirY = -5;
	}
	else if (r == 1) {
		this->dirX = 0;
		this->dirY = 5;
	}
	else if (r == 2) {
		this->dirX = -5;
		this->dirY = 0;
	}
	else if (r == 3) {
		this->dirX = 5;
		this->dirY = 0;
	}
	int newX = x + dirX;
	int newY = y + dirY;
	SDL_Rect newRect = { newX , newY , TILE_SIZE , TILE_SIZE };
	for (const auto& wall: walls)
	{
		if (wall.active && SDL_HasIntersection(&newRect, &wall.rect))
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
void EnemyTank::shoot()
{
	if (shootDelay > 0)
	{
		shootDelay--;
	}
	shootDelay = 5;
	bullets.push_back(Bullet(x + TILE_SIZE / 2 - 5, y + TILE_SIZE / 2 - 5, this->dirX, this->dirY));
}
void EnemyTank::updateBullets()
{
	for (auto& bullet : bullets)
	{
		bullet.move();
		bullets.erase(remove_if(bullets.begin(), bullets.end(), [](Bullet& b) { return !b.active; }), bullets.end());
	}
}
void EnemyTank::render(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
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