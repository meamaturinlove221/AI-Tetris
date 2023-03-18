#include"Game.h"

int main()
{
	Game tetris;
	while (tetris.window.isOpen())
	{
		tetris.gameRun();
	}
	return 0;
}