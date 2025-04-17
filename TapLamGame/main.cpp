#include "Game.h"
#include "Wall.h"




int main(int argc, char* argv[])
{
	Game game;
	if (game.running)
	{
		game.run();
	}
	return 0;
}
