#pragma once

#include<SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include<Windows.h>
#include<iostream>
#include<sstream>
#include<vector>

#define GRIDSIZE 35
#define STAGE_WIDTH 10
#define STAGE_HEIGHT 20
#define STATE 4

#define P1_STAGE_CORNER_X 156
#define P1_STAGE_CORNER_Y 174
#define P2_STAGE_CORNER_X 844
#define P2_STAGE_CORNER_Y 174

#define P1_NEXT_CORNER_X 587
#define P1_NEXT_CORNER_Y 125
#define P2_NEXT_CORNER_X 702
#define P2_NEXT_CORNER_Y 125

#define HOLD_CORNER_X 660
#define HOLD_CORNER_Y 275

#define DALAYVALUE 0.3

using namespace sf;

typedef enum PLAYROLE
{
	roleNONE,
	rolePLAYER1,
	rolePLAYER2
};

typedef enum gridShape
{
	shapeI,
	shapeS,
	shapeZ,
	shapeT,
	shapeL,
	shapeJ,
	shapeO
};

//AI
struct score_and_position
{
	double max_score;
	Vector2i position[4];
};


#define NUM_WEIGHTS 6
#define LANDING_HEIGHT 0
#define ROWS_ELILMINATED 1
#define ROWS_TRANSITION 2
#define COLUMN_TRANSITION 3
#define HOLE_NUM 4
#define WELL_SUM 5
#define POPULATION_SIZE 1000
#define FILE_SIZE 65536

struct GAINFO {
	int generationNum = 3;
	int count = 5;
	int bestFitness = 9;
	float bestWeights[NUM_WEIGHTS] = { 0 };
	int allFitness[POPULATION_SIZE] = { 0 };
	float allWeights[POPULATION_SIZE][NUM_WEIGHTS] = { 0 };
};

class Tetris
{
public:
	static int holdcolorNum, holdShapeNum;
	static Vector2i holdSquare[4];
	Tetris();
	~Tetris();

	Vector2i mCornPoint, nextSquareCornPoint, holdSquareCornPoint;//游戏区域位置

	int role;
	int gridsize;
	int img_BG_No, img_Skin_No;
	int colorNum, nextcolorNum, tempcolorNum;
	int currentShapeNum, nextShapeNum, tempShapeNum;
	bool gameover;
	Texture* tTiles;
	Texture tBackground, tButtons, tNum, tTimer, tCounter, tGameover;
	Sprite sBackground, sButtons, sNum, sTimer, sCounter, sGameover, sTiles;
	RenderWindow* window;

	int Field[STAGE_HEIGHT][STAGE_WIDTH] = { 0 };
	Vector2i currentSquare[4], nextSquare[4], tempSquare[4], shadowSquare[4];

	int Figures[7][4] =
	{
		{ 5, 3, 1, 7 },
		{ 5, 4, 2, 7 },
		{ 5, 4, 3, 6 },
		{ 5, 3, 4, 7 },
		{ 5, 3, 2, 7 },
		{ 5, 7, 3, 6 },
		{ 5, 3, 4, 2 }
	};

	int dx;
	bool rotate, hold, hardDrop, newShapeFlag, animationFlag;
	float timer, delay;
	int b7array[7] = { 0 }, b7Int;
	int animationRow[4];
	float animationCtrlValue;
	long score;

	void Initail(Texture* tex);
	void Input(Event event);
	int Bag7();
	void holdFunc();
	void yMove();
	void xMove();
	void rotateFunc();
	void checkLine();
	void Logic();
	void Draw(RenderWindow* window);
	bool hit_Test();
	void slowLoading();
	void hardDropFunc();
	void shadow();
	void traditionLogic();
	void clearLine();
	void newShapeFunc();
	void animationFunc(int i);
	void isWin();

	//AI
	int tempField[STAGE_HEIGHT][STAGE_WIDTH] = { 0 };
	void AI_initial();
	void AI_RotateFunc();
	void AI_xMove_right();
	void AI_xMove_left();
	void AI_Logic();
	void AI_HardDrop();
	void AI_checkLine();
	double getPosition();
	double calculate();
	int Landing_Height();
	int Eroded_Piece_Cells();
	int Row_transitions();
	int Colum_transitions();
	int Number_of_holes();
	int Well_sum();

	//PD改进
	double score_1, score_2;
	bool flag;
	int tempField_2[STAGE_HEIGHT][STAGE_WIDTH] = { 0 };
	score_and_position getPosition_plus();
	score_and_position getPosition_plus_return;
	bool check_is_can_drop(Vector2i Square[4], int from, int to);
	bool find_way_out(Vector2i Square[4], int dir);
	bool hit_test_plus(Vector2i Square[4]);

	void discribe_shape(Vector2i Square[4]);
	int temp_height_top, temp_height_down, temp_width_left, temp_width_right;
	int temp_height, temp_width;
	Vector2i temp_Square_relative[4];
	bool hit_Test_us(Vector2i Square[4]);

	//GA
	bool GA_on;
	long score_for_GA[4];
	unsigned char* infoFile;
	unsigned char buffer[FILE_SIZE];
	void sorting();
	void saveInfo();
	void generateInitialWeights();
	void generateChildren();
};