#pragma once

#include<SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include<Windows.h>
#include<iostream>
#include<sstream>
#include<fstream>  
#include<iomanip>
#include<assert.h>
#include"Tetris.h"

using namespace sf;

#define B_START_CORNER_X 621
#define B_START_CORNER_Y 763
#define B_HOLD_CORNER_X 621
#define B_HOLD_CORNER_Y 822
#define B_LEFT_CORNER_X 70
#define B_LEFT_CORNER_Y 460
#define B_RIGHT_CORNER_X 1295
#define B_RIGHT_CORNER_Y 460


#define P1_SCORE_CORNER_X 366
#define P1_SCORE_CORNER_Y 110
#define P2_SCORE_CORNER_X 1195
#define P2_SCORE_CORNER_Y 110
#define INFO_CORNER_X 565
#define INFO_CORNER_Y 436

class Game
{
public:
	Game();
	~Game();

	RenderWindow window;
	Tetris player1, player2;
	bool game_over, game_quit;
	Clock clock;
	int window_width, window_height, stage_width, stage_height;;
	bool is_game_begin, is_game_hold;
	bool auto_restart;//自动重开
	bool GA_on;//使用GA算法
	int is_game_over_state;
	Vector2i mCornPoint;
	int gridsize;
	int imgBGno, imgSkinNo, imgSetNo;
	Texture tBackground, tTiles, tButtons, tSwitcher, tFrame, tCover, tScore, tGameOver;
	Sprite sBackground, sTiles, sButtons, sSwitcher, sFrame, sCover, sScore, sGameOver;
	IntRect ButtonRectStart, ButtonRectHold, ButtonRectLeft, ButtonRectRight;
	int ButtonState_Start, ButtonState_Hold;
	SoundBuffer sbWin, sbBoom;
	Sound soundWin, soundBoom;
	Music bkMusic;
	Font font;
	Text text;
	Clock gameClock, mouseClickTimer;

	void gameInitial();
	void LoadMediaData();

	void gameInput();
	void gameLogic();
	void gameDraw();
	void DrawButton();
	void gameRun();
	void Textout();
	void DrawResults();
};

typedef enum ButtonState
{
	Continue_Dark,
	Continue_Light,
	Hold_Dark,
	Hold_Light,
	Close_Dark,
	Close_Light,
	Start_Dark,
	Start_Light
};
