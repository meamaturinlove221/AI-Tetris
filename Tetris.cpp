#include"Tetris.h"

int Tetris::holdcolorNum = 0;
int Tetris::holdShapeNum = 0;
Vector2i Tetris::holdSquare[4] = { {0,0},{0,0},{0,0},{0,0} };

Tetris::Tetris()
{
	dx = 0;
	rotate = false;
	colorNum = 1;
	timer = 0;
	GA_on = true;

	// 初始化第一代权重
	generateInitialWeights();
}

Tetris::~Tetris()
{

}

void Tetris::Initail(Texture* tex)
{
	srand(time(NULL));
	tTiles = tex;
	dx = 0;
	rotate = false;
	hold = false;
	hardDrop = false;
	animationFlag = false;
	animationCtrlValue = 1.0;
	gameover = false;

	colorNum = 1;
	timer = 0;
	delay = DALAYVALUE;
	b7Int = 0;
	score = 0;
	for (int i = 0; i < 4; i++)
	{
		score_for_GA[i] = 0;
	}

	holdSquareCornPoint = { HOLD_CORNER_X,HOLD_CORNER_Y };

	if (role == rolePLAYER1)
	{
		mCornPoint = { P1_STAGE_CORNER_X,P1_STAGE_CORNER_Y };
		nextSquareCornPoint = { P1_NEXT_CORNER_X,P1_NEXT_CORNER_Y };
	}
	if (role == rolePLAYER2)
	{
		mCornPoint = { P2_STAGE_CORNER_X,P2_STAGE_CORNER_Y };
		nextSquareCornPoint = { P2_NEXT_CORNER_X,P2_NEXT_CORNER_Y };
	}
	sTiles.setTexture(*tTiles);

	holdShapeNum = -1;
	colorNum = 1 + rand() % 7;
	currentShapeNum = rand() % 7;
	nextcolorNum = 1 + rand() % 7;
	nextShapeNum = rand() % 7;
	for (int i = 0; i < 4; i++)
	{
		currentSquare[i].x = Figures[currentShapeNum][i] % 2 + STAGE_WIDTH / 2;
		currentSquare[i].y = Figures[currentShapeNum][i] / 2;
		nextSquare[i].x = Figures[nextShapeNum][i] % 2;
		nextSquare[i].y = Figures[nextShapeNum][i] / 2;

		shadowSquare[i].x = -10;
		shadowSquare[i].y = -10;

		animationRow[i] = -1;
	}

	for (int i = 0; i < STAGE_HEIGHT; i++)
	{
		for (int j = 0; j < STAGE_WIDTH; j++)
		{
			Field[i][j] = 0;
			tempField[i][j] = 0;
		}
	}

	getPosition_plus_return.max_score = -DBL_MAX;
	for (int i = 0; i < 4; i++)
	{
		getPosition_plus_return.position[i].x = 0;
		getPosition_plus_return.position[i].y = 0;
	}
}

int Tetris::Bag7()
{
	int num;
	num = rand() % 7;
	//for (int i = 0; i < b7Int; i++)
	//{
	//	if (b7array[i] == num)
	//	{
	//		i = -1;//i++后归零，数组重新遍历
	//		num = rand() % 7;
	//	}
	//}
	//b7array[b7Int] = num;

	//b7Int++;
	//if (b7Int == 7)
	//{
	//	b7Int = 0;
	//	for (int i = 0; i < 7; i++)
	//	{
	//		b7array[i] = 0;
	//	}
	//}
	return num;
}

void Tetris::yMove()
{
	//下落
	for (int i = 0; i < 4; i++)
	{
		tempSquare[i] = currentSquare[i];
		currentSquare[i].y += 1;
	}
	if (!hit_Test())
	{
		for (int i = 0; i < 4; i++)
		{
			Field[tempSquare[i].y][tempSquare[i].x] = colorNum;
		}
		newShapeFlag = true;
	}
}

void Tetris::newShapeFunc()
{
	colorNum = nextcolorNum;
	currentShapeNum = nextShapeNum;

	nextcolorNum = 1 + rand() % 7;
	nextShapeNum = Bag7();

	for (int i = 0; i < 4; i++)
	{
		currentSquare[i] = nextSquare[i];
		currentSquare[i].x = currentSquare[i].x + STAGE_WIDTH / 2;
		nextSquare[i].x = Figures[nextShapeNum][i] % 2;
		nextSquare[i].y = Figures[nextShapeNum][i] / 2;
	}

	shadow();

	newShapeFlag = false;
}

void Tetris::xMove()
{
	for (int i = 0; i < 4; i++)
	{
		tempSquare[i] = currentSquare[i];
		currentSquare[i].x += dx;
	}
	if (!hit_Test())
	{
		for (int i = 0; i < 4; i++)
		{
			currentSquare[i] = tempSquare[i];
		}
	}
}

void Tetris::checkLine()
{
	int k = STAGE_HEIGHT - 1;
	int yCount = 0;
	for (int i = STAGE_HEIGHT - 1; i > 0; i--)
	{
		int xCount = 0;
		for (int j = 0; j < STAGE_WIDTH; j++)
		{
			if (Field[i][j])
			{
				xCount++;
			}
		}
		if (xCount < STAGE_WIDTH)
		{
			k--;
		}
		else
		{
			animationRow[yCount] = i;
			yCount++;
			animationFlag = true;
		}
	}

	score_for_GA[yCount - 1]++;

	switch (yCount)
	{
	case 1:
		score += 1;
		break;
	case 2:
		score += 2;
		break;
	case 3:
		score += 3;
		break;
	case 4:
		score += 4;
		break;
	}
}

void Tetris::rotateFunc()
{
	int originalHeight = currentSquare[0].y;

	for (int j = 0; j < 4; j++)
	{
		Vector2i p = currentSquare[j];

		for (int i = 0; i < 4; i++)
		{
			int x = currentSquare[i].y - p.y;
			int y = currentSquare[i].x - p.x;

			currentSquare[i].x = p.x - x;
			currentSquare[i].y = p.y + y;
		}

		if (hit_Test())
		{
			int detaY = 0;
			detaY = currentSquare[0].y - originalHeight;
			if (detaY != 0)
			{
				for (int i = 0; i < 4; i++)
				{
					currentSquare[i].y -= detaY;
				}
			}

			if (!hit_Test())
			{
				for (int i = 0; i < 4; i++)
				{
					currentSquare[i] = tempSquare[i];
				}
			}
			else
			{
				break;
			}
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				currentSquare[i] = tempSquare[i];
			}
		}
	}

}

void Tetris::holdFunc()
{
	Vector2i backUpSquare[4];
	tempcolorNum = holdcolorNum;
	tempShapeNum = holdShapeNum;

	holdcolorNum = colorNum;
	holdShapeNum = currentShapeNum;

	for (int i = 0; i < 4; i++)
	{
		holdSquare[i].x = Figures[holdShapeNum][i] % 2;
		holdSquare[i].y = Figures[holdShapeNum][i] / 2;
		tempSquare[i].x = Figures[tempShapeNum][i] % 2;
		tempSquare[i].y = Figures[tempShapeNum][i] / 2;
		backUpSquare[i] = currentSquare[i];
	}

	if (tempShapeNum < 0)
	{
		newShapeFunc();
	}
	else
	{
		colorNum = tempcolorNum;
		currentShapeNum = tempShapeNum;

		int minCurrentX = currentSquare[0].x;
		int minCurrentY = currentSquare[0].y;
		int minTempX = tempSquare[0].x;
		int minTempY = tempSquare[0].y;
		int dx, dy;
		for (int i = 1; i < 4; i++)
		{
			if (currentSquare[i].x < minCurrentX)
			{
				minCurrentX = currentSquare[i].x;
			}
			if (currentSquare[i].y < minCurrentY)
			{
				minCurrentY = currentSquare[i].y;
			}
			if (tempSquare[i].x < minTempX)
			{
				minTempX = tempSquare[i].x;
			}
			if (tempSquare[i].y < minTempY)
			{
				minTempY = tempSquare[i].y;
			}
		}
		dx = minCurrentX - minTempX;
		dy = minCurrentY - minTempY;

		for (int i = 0; i < 4; i++)
		{
			currentSquare[i].x = tempSquare[i].x + dx;
			currentSquare[i].y = tempSquare[i].y + dy;
			holdSquare[i].x = Figures[holdShapeNum][i] % 2;
			holdSquare[i].y = Figures[holdShapeNum][i] / 2;
		}
	}

	if (!hit_Test())
	{
		colorNum = holdcolorNum;
		holdcolorNum = tempcolorNum;
		holdShapeNum = tempShapeNum;
		for (int i = 0; i < 4; i++)
		{
			currentSquare[i] = backUpSquare[i];
			holdSquare[i].x = Figures[holdShapeNum][i] % 2;
			holdSquare[i].y = Figures[holdShapeNum][i] / 2;
		}
	}
}

void Tetris::traditionLogic()
{
	//hold
	if (hold)
	{
		holdFunc();
		hold = false;
	}

	//水平移动
	xMove();

	//旋转
	if (rotate)
	{
		rotateFunc();
		rotate = false;
	}

	slowLoading();
	//下落
	if (timer > delay)
	{
		yMove();
		timer = 0;
	}
	shadow();
	if (hardDrop)
	{
		hardDropFunc();
		hardDrop = false;
	}
}

void Tetris::Logic()
{
	if (!animationFlag)
	{
		traditionLogic();
	}

	if (newShapeFlag)
	{
		if (animationFlag == false)
		{
			//检查消行
			checkLine();
			if (animationFlag == false)
			{
				newShapeFunc();
			}
			isWin();
		}
		else
		{
			animationCtrlValue -= 0.1;
			if (animationCtrlValue < 0)
			{
				animationFlag = false;
				animationCtrlValue = 1.0;
				for (int i = 0; i < 4; i++)
				{
					animationRow[i] = -1;
				}
				clearLine();
				newShapeFunc();
			}
		}
	}

}

bool Tetris::hit_Test_us(Vector2i Square[4])
{
	for (int i = 0; i < 4; i++)
	{

		if (Square[i].x < 0 ||
			Square[i].x >= STAGE_WIDTH ||
			Square[i].y >= STAGE_HEIGHT)
		{
			return false;
		}
		else if (Field[Square[i].y][Square[i].x])
		{
			return false;
		}
		else if (!Field[Square[i].y + 1][Square[i].x]) {
			return false;
		}
	}
	return true;
}



bool Tetris::hit_Test()
{
	for (int i = 0; i < 4; i++)
	{
		if (currentSquare[i].x < 0 ||
			currentSquare[i].x >= STAGE_WIDTH ||
			currentSquare[i].y >= STAGE_HEIGHT)
		{
			return false;
		}
		else if (Field[currentSquare[i].y][currentSquare[i].x])
		{
			return false;
		}
	}
	return true;
}

void Tetris::Input(Event event)
{
	if (role == rolePLAYER1)
	{
		if (event.type == Event::KeyPressed)
		{
			if (event.key.code == Keyboard::W)
			{
				if (currentShapeNum != shapeO)
				{
					rotate = true;
				}
			}

			if (event.key.code == Keyboard::A)
			{
				dx = -1;
			}
			else if (event.key.code == Keyboard::D)
			{
				dx = 1;
			}

			if (event.key.code == Keyboard::S)
			{
				delay = 0.05;
			}
		}

		if (event.type == Event::KeyReleased)
		{
			if (event.key.code == Keyboard::LControl)
			{
				hold = true;
			}
			if (event.key.code == Keyboard::Space)
			{
				hardDrop = true;
			}
			if (event.key.code == Keyboard::A || event.key.code == Keyboard::D)
			{
				dx = 0;
			}
			if (event.key.code == Keyboard::S)
			{
				delay = 0.3;
			}
		}
	}

	if (role == rolePLAYER2)
	{
		if (event.type == Event::KeyPressed)
		{
			if (event.key.code == Keyboard::Up)
			{
				if (currentShapeNum != shapeO)
				{
					rotate = true;
				}
			}

			if (event.key.code == Keyboard::Left)
			{
				dx = -1;
			}
			else if (event.key.code == Keyboard::Right)
			{
				dx = 1;
			}

			if (event.key.code == Keyboard::Down)
			{
				delay = 0.05;
			}
		}

		if (event.type == Event::KeyReleased)
		{
			if (event.key.code == Keyboard::RControl)
			{
				hold = true;
			}
			if (event.key.code == Keyboard::Enter)
			{
				hardDrop = true;
			}
			if (event.key.code == Keyboard::Left || event.key.code == Keyboard::Right)
			{
				dx = 0;
			}
			if (event.key.code == Keyboard::Down)
			{
				delay = 0.3;
			}
		}
	}
}

void Tetris::Draw(RenderWindow* w)
{
	window = w;

	if (animationFlag == false)
	{
		//shadow
		for (int i = 0; i < 4; i++)
		{
			sTiles.setTextureRect(IntRect(colorNum * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
			sTiles.setPosition(shadowSquare[i].x * GRIDSIZE, shadowSquare[i].y * GRIDSIZE);
			sTiles.setColor(Color(50, 50, 50, 255));
			sTiles.move(mCornPoint.x, mCornPoint.y);
			window->draw(sTiles);
			sTiles.setColor(Color(255, 255, 255, 255));
		}

		for (int i = 0; i < 4; i++)
		{
			sTiles.setTextureRect(IntRect(colorNum * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
			sTiles.setPosition(currentSquare[i].x * GRIDSIZE, currentSquare[i].y * GRIDSIZE);
			sTiles.move(mCornPoint.x, mCornPoint.y);
			window->draw(sTiles);
		}


	}

	//绘制固定的方块
	for (int i = 0; i < STAGE_HEIGHT; i++)
	{
		if (i == animationRow[0] || i == animationRow[1] || i == animationRow[2] || i == animationRow[3])
		{
			animationFunc(i);//遇到动画行的时候，执行特效动画
		}
		else
		{
			for (int j = 0; j < STAGE_WIDTH; j++)
			{
				if (Field[i][j] == 0)
				{
					continue;
				}

				sTiles.setTextureRect(IntRect(Field[i][j] * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
				sTiles.setPosition(j * GRIDSIZE, i * GRIDSIZE);
				sTiles.move(mCornPoint.x, mCornPoint.y);
				window->draw(sTiles);
			}
		}
	}



	//Next区
	for (int i = 0; i < 4; i++)
	{
		sTiles.setTextureRect(IntRect(nextcolorNum * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
		sTiles.setPosition(nextSquare[i].x * GRIDSIZE, nextSquare[i].y * GRIDSIZE);
		sTiles.move(nextSquareCornPoint.x, nextSquareCornPoint.y);
		window->draw(sTiles);
	}

	//hold区
	if (holdShapeNum > -1)
	{
		for (int i = 0; i < 4; i++)
		{
			sTiles.setTextureRect(IntRect(holdcolorNum * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
			sTiles.setPosition(holdSquare[i].x * GRIDSIZE, holdSquare[i].y * GRIDSIZE);
			sTiles.move(holdSquareCornPoint.x, holdSquareCornPoint.y);
			window->draw(sTiles);
		}
	}
}

void Tetris::slowLoading()
{
	for (int i = 0; i < 4; i++)
	{
		tempSquare[i] = currentSquare[i];
		currentSquare[i].y += 1;
	}

	if (!hit_Test())
	{
		delay = DALAYVALUE * 1.5;
	}

	for (int i = 0; i < 4; i++)
	{
		currentSquare[i] = tempSquare[i];
	}
}

void Tetris::hardDropFunc()
{
	for (int i = 0; i < 4; i++)
	{
		currentSquare[i] = shadowSquare[i];
	}
}

void Tetris::shadow()
{
	for (int i = 0; i < 4; i++)
	{
		shadowSquare[i] = currentSquare[i];
	}

	for (int j = 0; j < STAGE_HEIGHT; j++)
	{
		for (int i = 0; i < 4; i++)
		{
			currentSquare[i].y += 1;
		}
		if (!hit_Test())
		{
			for (int i = 0; i < 4; i++)
			{
				currentSquare[i].y -= 1;
			}
			break;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		tempSquare[i] = currentSquare[i];
		currentSquare[i] = shadowSquare[i];
		shadowSquare[i] = tempSquare[i];
	}
}

void Tetris::clearLine()
{
	int k = STAGE_HEIGHT - 1;
	for (int i = STAGE_HEIGHT - 1; i > 0; i--)
	{
		int xCount = 0;
		for (int j = 0; j < STAGE_WIDTH; j++)
		{
			if (Field[i][j])
			{
				xCount++;
			}
			Field[k][j] = Field[i][j];
		}
		if (xCount < STAGE_WIDTH)
		{
			k--;
		}
	}
}

void Tetris::animationFunc(int i)
{
	Vector2f p;
	sTiles.scale(animationCtrlValue, animationCtrlValue);
	p = sTiles.getOrigin();
	sTiles.setOrigin(GRIDSIZE / 2, GRIDSIZE / 2);
	sTiles.rotate(360 * animationCtrlValue);
	for (int j = 0; j < STAGE_WIDTH; j++)
	{
		sTiles.setTextureRect(IntRect(Field[i][j] * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
		sTiles.setPosition(j * GRIDSIZE, i * GRIDSIZE);
		sTiles.move(mCornPoint.x + GRIDSIZE / 2, mCornPoint.y + GRIDSIZE / 2); //offset
		window->draw(sTiles);
	}
	sTiles.scale(1.0 / animationCtrlValue, 1.0 / animationCtrlValue);
	sTiles.rotate(-360 * animationCtrlValue);
	sTiles.setOrigin(p);
}

void Tetris::isWin()
{
	if (Field[2][5] || Field[2][6])
	{
		gameover = true;
	}
}