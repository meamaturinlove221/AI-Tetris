#include"Tetris.h"

using namespace std;

void Tetris::AI_initial()
{
	for (int i = 0; i < STAGE_HEIGHT; i++)
	{
		for (int j = 0; j < STAGE_WIDTH; j++)
		{
			tempField[i][j] = Field[i][j];
		}
	}

	AI_HardDrop();
}


void Tetris::AI_Logic()
{
	GAINFO* info = (GAINFO*)infoFile;

	if (flag)
	{
		// 速降
		hardDropFunc();

		// 将活动方块下降后添加至舞台
		for (int i = 0; i < 4; i++)
		{
			Field[currentSquare[i].y][currentSquare[i].x] = colorNum;
		}
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			currentSquare[i].x = getPosition_plus_return.position[i].x;
			currentSquare[i].y = getPosition_plus_return.position[i].y;
		}

		// 将活动方块下降后添加至舞台
		for (int i = 0; i < 4; i++)
		{
			Field[currentSquare[i].y][currentSquare[i].x] = colorNum;
		}
	}


	// 检查是否消行
	AI_checkLine();

	// 生成新的活动方块
	newShapeFunc();
	isWin();


	if (!gameover)
	{
		// 更改活动方块的旋转和水平位置
		score_1 = getPosition();
		shadow();

		getPosition_plus_return = getPosition_plus();
		score_2 = getPosition_plus_return.max_score;

		if (score_2 <= score_1)
		{
			flag = true;
		}
		else
		{
			flag = false;
			cout << "in our caculate | " << score << endl;
		}
	}

	if (GA_on && gameover)
	{
		for (int i = 0; i < 4; i++)
		{
			info->allFitness[info->count] += (i + 1) * score_for_GA[i];
		}

		if (info->allFitness[info->count] > info->bestFitness)
		{
			info->bestFitness = info->allFitness[info->count];
			for (int j = 0; j < NUM_WEIGHTS; j++)
			{
				info->bestWeights[j] = info->allWeights[info->count][j];
			}
		}
		if (info->count >= POPULATION_SIZE - 1)
		{
			std::stringstream ss;
			ss << "generate" << info->generationNum << ".txt";
			FILE* p;
			fopen_s(&p, ss.str().c_str(), "w+");
			if (p == NULL)
			{
				printf("failed to open file\n");
				system("pause");
			}
			else
			{
				for (int i = 0; i < POPULATION_SIZE; i++)
				{
					fprintf_s(p, "%d; ", info->allFitness[i]);
					for (int j = 0; j < NUM_WEIGHTS; j++)
					{
						fprintf_s(p, "%f; ", info->allWeights[i][j]);
					}
					fprintf_s(p, "\n");
				}
				fclose(p);
			}
			info->generationNum++;
			generateChildren();
			info->count = 0;
		}
		else
		{
			info->count++;
		}

		saveInfo();
	}
}

void Tetris::AI_checkLine()
{
	//checkLine
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
			Field[k][j] = Field[i][j];
		}
		if (xCount < STAGE_WIDTH)
		{
			k--;
		}
		else
		{
			yCount++;
		}
	}

	for (int i = 0; i < yCount; i++)
	{
		for (int j = 0; j < STAGE_WIDTH; j++)
		{
			Field[i][j] = 0;
		}
	}

	if (yCount > 0)
	{
		score_for_GA[yCount - 1]++;
	}

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

void Tetris::AI_RotateFunc()
{
	for (int i = 0; i < 4; i++)
	{
		int x = currentSquare[i].y - currentSquare[0].y;
		int y = currentSquare[i].x - currentSquare[0].x;
		currentSquare[i].x = currentSquare[0].x - x;
		currentSquare[i].y = currentSquare[0].y + y;
	}
}

void Tetris::AI_HardDrop()
{
	Vector2i temp[4];
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
		tempField[currentSquare[i].y][currentSquare[i].x] = colorNum;
		temp[i] = currentSquare[i];
		currentSquare[i] = shadowSquare[i];
		shadowSquare[i] = temp[i];
	}
}

void Tetris::AI_xMove_right()
{
	for (int i = 0; i < 4; i++)
	{
		currentSquare[i].x += dx;
	}
}

void Tetris::AI_xMove_left()
{
	for (int i = 0; i < 4; i++)
	{
		currentSquare[i].x -= dx;
	}
}

int Tetris::Landing_Height()
{
	int count = shadowSquare[0].y;
	return STAGE_HEIGHT - count - 1;
}

int Tetris::Eroded_Piece_Cells()
{
	int k = STAGE_HEIGHT - 1;
	int yCount = 0;
	int squareCount = 0;
	for (int i = STAGE_HEIGHT - 1; i > 0; i--)
	{
		int xCount = 0;
		for (int j = 0; j < STAGE_WIDTH; j++)
		{
			if (tempField[i][j])
			{
				xCount++;
			}
			tempField[k][j] = tempField[i][j];
		}
		if (xCount < STAGE_WIDTH)
		{
			k--;
		}
		else
		{
			squareCount = 0;
			for (int j = 0; j < 4; j++)
			{
				if (shadowSquare[j].y == i)
				{
					squareCount++;
				}
			}
			yCount++;
		}
	}
	return yCount * squareCount;
}

int Tetris::Row_transitions()
{
	int count = 0;
	for (int i = 0; i < STAGE_HEIGHT; i++)
	{
		if (tempField[i][0] == 0)
		{
			count++;
		}
		for (int j = 1; j < STAGE_WIDTH; j++)
		{
			if ((tempField[i][j - 1] != 0 && tempField[i][j] == 0) || (tempField[i][j - 1] == 0 && tempField[i][j] != 0))
			{
				count++;
			}
		}
		if (tempField[i][STAGE_WIDTH - 1] == 0)
		{
			count++;
		}
	}
	//std::cout << "rowTransition: " << count << std::endl;
	return count;
}

int Tetris::Colum_transitions()
{
	int count = 0;
	for (int i = 1; i < STAGE_HEIGHT; i++)
	{
		for (int j = 0; j < STAGE_WIDTH; j++)
		{
			if ((tempField[i - 1][j] != 0 && tempField[i][j] == 0) || (tempField[i - 1][j] == 0 && tempField[i][j] != 0))
			{
				count++;
			}
		}
	}

	for (int i = 0; i < STAGE_WIDTH; i++)
	{
		if (tempField[STAGE_HEIGHT - 1][i] == 0)
		{
			count++;
		}
	}

	//std::cout << "columnTransition: " << count << std::endl;
	return count;
}

int Tetris::Number_of_holes()
{
	int count, count_hole;
	count = 0;
	for (int i = 0; i < STAGE_WIDTH; i++)
	{
		for (int j = 0; j < STAGE_HEIGHT; j++)
		{
			if (tempField[j][i] != 0)
			{
				count_hole = 0;
				for (int k = j; k < STAGE_HEIGHT; k++)
				{
					if (tempField[k][i] != 0)
					{
						count += count_hole;
						count_hole = 0;
					}
					else
					{
						count_hole++;
					}
				}
				count += count_hole;
				break;
			}
		}
	}

	//std::cout << "holeNum: " << count << std::endl;
	return count;
}

int Tetris::Well_sum()
{
	int wellSums = 0;

	for (int i = STAGE_HEIGHT - 1; i > 0; i--)
	{
		for (int j = 0; j < STAGE_WIDTH; j++)
		{
			if (j == 0)
			{
				if (tempField[i][j] == 0 && tempField[i][j + 1])
				{
					wellSums++;
					for (int k = i - 1; k > 0; k--)
					{
						if (tempField[k][j] == 0 && tempField[k][j + 1])
						{
							wellSums++;
						}
						else
						{
							break;
						}
					}
				}
			}
			else if (j == STAGE_WIDTH - 1)
			{
				if (tempField[i][j] == 0 && tempField[i][j - 1])
				{
					wellSums++;
					for (int k = i - 1; k > 0; k--)
					{
						if (tempField[k][j] == 0 && tempField[k][j - 1])
						{
							wellSums++;
						}
						else
						{
							break;
						}
					}
				}
			}
			else if (tempField[i][j] == 0 && tempField[i][j - 1] && tempField[i][j + 1])
			{
				wellSums++;
				for (int k = i - 1; k > 0; k--)
				{
					if (tempField[k][j] == 0 && tempField[k][j - 1] && tempField[k][j + 1])
					{
						wellSums++;
					}
					else
					{
						break;
					}
				}
			}
		}
	}

	return wellSums;
}

double Tetris::getPosition()
{
	int i, j, maxI = 0, maxJ = 0;
	int rotate = STATE, width = STAGE_WIDTH;
	double scores[STAGE_WIDTH][STATE];
	double maxScore = -DBL_MAX;

	// 将活动方块移至最左边开始遍历，并保存初始值
	dx = -currentSquare[0].x;
	AI_xMove_right();
	for (i = 0; i < 4; i++)
	{
		tempSquare[i] = currentSquare[i];
	}

	// 根据旋转中心的位置和活动方块的旋转状态遍历计算每种落点的权重
	dx = 1;
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < rotate; j++)
		{
			// 相关初始化
			AI_initial();

			// 如果这种落点没有产生碰撞，则开始计算当前落点的权重
			if (hit_Test())
			{
				//std::cout << "i: " << i << " j: " << j << std::endl;
				scores[i][j] = calculate();
				if (scores[i][j] > maxScore)
				{
					maxI = i;
					maxJ = j;
					maxScore = scores[i][j];
				}
			}
			// 旋转至下一个状态
			AI_RotateFunc();
		}
		// 活动方块往右移动一格
		AI_xMove_right();
	}



	// 根据计算出的maxI,maxJ更改活动方块的位置和旋转状态
	//std::cout << "maxI: " << maxI << "; maxJ: " << maxJ << std::endl;
	for (i = 0; i < 4; i++)
	{
		currentSquare[i] = tempSquare[i];
	}
	if (maxScore > (long)INT_MIN)
	{
		for (int k = 0; k < maxJ; k++)
		{
			AI_RotateFunc();
		}
		dx = maxI;
		AI_xMove_right();
	}
	else
	{
		dx = STAGE_WIDTH / 2 + 1;
		AI_xMove_right();
	}

	return maxScore;
}

double Tetris::calculate()
{
	GAINFO* info = (GAINFO*)infoFile;

	double sum;
	int landingHeight = Landing_Height();
	int rowsEliminated = Eroded_Piece_Cells();
	int rowsTransition = Row_transitions();
	int columnTransition = Colum_transitions();
	int holeNUm = Number_of_holes();
	int wellSum = Well_sum();

	//double sum;
	//int rowsEliminated = getRowsEliminated();
	//int totalHeight = getTotalHeight();
	//int heightDifferences = getHeightDifferences();
	//int holeNUm = getHoleNum();

	if (landingHeight < 0 || rowsEliminated < 0 || rowsTransition < 0
		|| columnTransition < 0 || holeNUm < 0 || wellSum < 0)
	{
		return -DBL_MAX;
	}

	//if (rowsEliminated < 0 || totalHeight < 0 || heightDifferences < 0 || holeNUm < 0)
	//	return -DBL_MAX;

	if (tempField[2][5] != 0 || tempField[2][6] != 0 || tempField[3][5] != 0 || tempField[3][6] != 0)
	{
		return -DBL_MAX;
	}

	if (!GA_on)
	{
		sum = landingHeight * -4.500158825082766
			+ rowsEliminated * 3.4181268101392694
			+ rowsTransition * -3.2178882868487753
			+ columnTransition * -9.348695305445199
			+ holeNUm * -7.899265427351652
			+ wellSum * -3.3855972247263626;
	}
	else
	{
		sum = 1.0 * landingHeight * info->allWeights[info->count][LANDING_HEIGHT]
			+ 1.0 * rowsEliminated * info->allWeights[info->count][ROWS_ELILMINATED]
			+ 1.0 * rowsTransition * info->allWeights[info->count][ROWS_TRANSITION]
			+ 1.0 * columnTransition * info->allWeights[info->count][COLUMN_TRANSITION]
			+ 1.0 * holeNUm * info->allWeights[info->count][HOLE_NUM]
			+ 1.0 * wellSum * info->allWeights[info->count][WELL_SUM];
	}

	//sum = landingHeight * -0.410342
	//	+ rowsEliminated * 0.269555
	//	+ rowsTransition * -0.205071
	//	+ columnTransition * -0.553468
	//	+ holeNUm * -0.558999
	//	+ wellSum * -0.313205;



		//sum = 1.0 * rowsEliminated * info->allWeights[info->count][ROWS_ELILMINATED]
		//	+ 1.0 * totalHeight * info->allWeights[info->count][TOTAL_HEIGHT];
		//	+ 1.0 * heightDifferences * info->allWeights[info->count][HEIGHT_DIFFERENCES];
		//	+ 1.0 * holeNUm * info->allWeights[info->count][HOLE_NUM];

	return sum;
}

void Tetris::discribe_shape(Vector2i Square[4]) {
	temp_height_top = STAGE_HEIGHT;
	temp_height_down = -1;
	temp_width_left = STAGE_WIDTH;
	temp_width_right = -1;
	for (int i = 0; i < 4; i++) {
		if (Square[i].x > temp_width_right) {
			temp_width_right = Square[i].x;
		}
		if (Square[i].x < temp_width_left) {
			temp_width_left = Square[i].x;
		}
		if (Square[i].y > temp_height_down) {
			temp_height_down = Square[i].y;
		}
		if (Square[i].y < temp_height_top) {
			temp_height_top = Square[i].y;
		}
	}
	temp_height = temp_height_down - temp_height_top + 1;
	temp_width = temp_width_right - temp_width_left + 1;

	for (int i = 0; i < 4; i++) {
		temp_Square_relative[i].x = Square[i].x - temp_width_left;
		temp_Square_relative[i].y = Square[i].y - temp_height_top;
	}
}


//our_plus_function
score_and_position Tetris::getPosition_plus()
{
	int start_find_line = STAGE_HEIGHT - 1;
	//向下寻找直到有格子的那一行
	for (int i = STAGE_HEIGHT - 1; i >= 0; i--)
	{
		vector<int> field_in_a_line_data(10);
		int sum = 0;//如果sum大于0，则说明这一行有东西
		for (int j = 0; j < field_in_a_line_data.size(); j++)
		{
			field_in_a_line_data[j] = Field[i][j];
		}
		for (int k = 0; k < field_in_a_line_data.size(); k++)
		{
			sum += field_in_a_line_data[k];
		}
		if (sum == 0)
		{
			start_find_line = i + 1;
			break;
		}
	}

	Vector2i tempSquare[4];
	// 将活动方块移至最左边开始遍历，并保存初始值
	//dx = -currentSquare[0].x;
	//??
	//for (int i = 0; i < 4; i++)
	//{
	//	currentSquare[i].x += dx;
	//}
	//
	for (int i = 0; i < 4; i++)
	{
		tempSquare[i] = currentSquare[i];
	}
	discribe_shape(tempSquare);


	score_and_position result;
	result.max_score = -DBL_MAX;
	for (int i = 0; i < 4; i++)
	{
		result.position[i].x = 0;
		result.position[i].y = 0;
	}

	for (int r = 0; r < 4; r++)//四种旋转状态
	{
		//平移到起始位置,左上角坐标 x=0 y=start
		for (int i = 0; i < 4; i++) {
			tempSquare[i].x = temp_Square_relative[i].x;
			tempSquare[i].y += start_find_line;
		}
		//遍历
		for (int i = start_find_line; i < STAGE_HEIGHT; i++)
		{
			discribe_shape(tempSquare);
			if (temp_height_down > STAGE_HEIGHT) {
				break;
			}
			for (int j = 0; j < STAGE_WIDTH; j++)
			{
				// 如果这种落点没有产生碰撞，则开始计算当前落点的权重
				if (hit_Test_us(tempSquare))
				{
					if (!check_is_can_drop(tempSquare, i, start_find_line))
					{
						if (find_way_out(tempSquare, 3))
						{
							//计算分数
							double this_score = calculate();
							if (this_score > result.max_score)
							{
								result.max_score = this_score;
								for (int w = 0; w < 4; w++)
								{
									result.position[w].x = tempSquare[w].x;
									result.position[w].y = tempSquare[w].y;
								}
							}
						}
					}
				}
				// 活动方块往右移动一格
				dx = 1;
				bool can_move = true;
				for (int q = 0; q < 4; q++)
				{
					if (tempSquare[q].x >= STAGE_WIDTH)
					{
						can_move = false;
					}
				}
				if (can_move)
				{
					for (int s = 0; s < 4; s++)
					{
						tempSquare[s].x += dx;
					}
				}
				else {
					break;
				}
			}
			//移动要最左边
			for (int r = 0; r < 4; r++)
			{
				dx = -tempSquare[r].x;
				tempSquare[r].x += dx;
			}
			//下移一行
			for (int p = 0; p < 4; p++)
			{
				tempSquare[p].y += 1;
			}
		}
		// 旋转至下一个状态
		AI_RotateFunc();
	}
	return result;
}

bool Tetris::check_is_can_drop(Vector2i Square[4], int from, int to)
{
	
	for (int i = 0; i < 4; i++)
	{
		for (int q = Square[i].y - 1; q >= 0; q--) {
			if (Field[q][Square[i].x]) {
				return false;
			}
		}
	}
	return true;
}


bool Tetris::find_way_out(Vector2i Square[4], int dir)
{
	Vector2i origin_Square[4];
	for (int i = 0; i < 4; i++)
	{
		origin_Square[i] = Square[i];
	}

	Vector2i Square_after_rorate[3][4];//0-顺时针90 1-180度 2-逆时针90
	bool rotate_flag[3] = { false };//0-顺时针90 1-180度 2-逆时针90
	if (check_is_can_drop(Square, Square[0].y, 0))
	{
		return true;
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			int x = Square[i].y - Square[0].y;
			int y = Square[i].x - Square[0].x;
			Square[i].x = Square[0].x - x;
			Square[i].y = Square[0].y + y;
			Square_after_rorate[0][i] = Square[i];
		}
		for (int i = 0; i < 4; i++)
		{
			int x = Square[i].y - Square[0].y;
			int y = Square[i].x - Square[0].x;
			Square[i].x = Square[0].x - x;
			Square[i].y = Square[0].y + y;
			Square_after_rorate[1][i] = Square[i];
		}
		for (int i = 0; i < 4; i++)
		{
			int x = origin_Square[i].y - origin_Square[0].y;
			int y = origin_Square[i].x - origin_Square[0].x;
			origin_Square[i].x = origin_Square[0].x + x;
			origin_Square[i].y = origin_Square[0].y + y;
			Square_after_rorate[2][i] = origin_Square[i];
		}
	}

	for (int i = 0; i < 2; i++)
	{
		if (hit_test_plus(Square_after_rorate[i]))
		{
			if (check_is_can_drop(Square_after_rorate[i], Square_after_rorate[i][0].y, 0))
			{
				return true;
			}
			rotate_flag[i] = true;
		}
	}

	if (rotate_flag[0] || rotate_flag[1])
	{
		if (hit_test_plus(Square_after_rorate[2]))
		{
			if (check_is_can_drop(Square_after_rorate[2], Square_after_rorate[2][0].y, 0))
			{
				return true;
			}
			rotate_flag[2] = true;
		}
	}

	//旋转结束 开始平移
	bool totalflag = false;
	Vector2i Square_temp[4];
	//up
	for (int i = 0; i < 4; i++) {
		Square_temp[i] = Square[i];
		Square_temp[i].y -= 1;
	}
	if (hit_test_plus(Square_temp)) {
		if (!totalflag) {
			totalflag = find_way_out(Square_temp, 0);
		}
	}
	for (int i = 0; i < 3; i++) {
		if (rotate_flag[i]) {
			for (int j = 0; j < 4; j++) {
				Square_temp[j] = Square_after_rorate[i][j];
				Square_temp[j].y -= 1;
			}
			if (hit_test_plus(Square_temp)) {
				if (!totalflag) {
					totalflag = find_way_out(Square_temp, 0);
				}
			}
		}
	}
	//left
	if (dir == 1 || dir == 3) {
		bool flag = true;
		for (int i = 0; i < 4; i++) {
			Square_temp[i] = Square[i];
			Square_temp[i].x -= 1;
			if (Square_temp[i].x < 0) {
				flag = false;
			}
		}
		if (hit_test_plus(Square_temp) && flag) {
			//
			if (check_is_can_drop(Square_temp, Square_temp[0].y, 0))
			{
				return true;
			}
			if (!totalflag) {
				totalflag = find_way_out(Square_temp, 1);
			}
		}

		bool rotateflag[3] = { true };
		for (int i = 0; i < 3; i++) {
			if (rotate_flag[i]) {
				for (int j = 0; j < 4; j++) {
					Square_temp[j] = Square_after_rorate[i][j];
					Square_temp[j].x -= 1;
					if (Square_temp[j].x < 0) {
						rotateflag[i] = false;
					}
				}
				if (hit_test_plus(Square_temp) && rotateflag[i]) {
					if (check_is_can_drop(Square_temp, Square_temp[0].y, 0))
					{
						return true;
					}
					if (!totalflag) {
						totalflag = find_way_out(Square_temp, 1);
					}
				}
			}
		}
	}
	//right
	if (dir == 2 || dir == 3) {
		bool flag = true;
		for (int i = 0; i < 4; i++) {
			Square_temp[i] = Square[i];
			Square_temp[i].x += 1;
			if (Square_temp[i].x > STAGE_WIDTH - 1) {
				flag = false;
			}
		}
		if (hit_test_plus(Square_temp) && flag) {
			//
			if (check_is_can_drop(Square_temp, Square_temp[0].y, 0))
			{
				return true;
			}
			if (!totalflag) {
				totalflag = find_way_out(Square_temp, 2);
			}
		}

		bool rotateflag[3] = { true };
		for (int i = 0; i < 3; i++) {
			if (rotate_flag[i]) {
				for (int j = 0; j < 4; j++) {
					Square_temp[j] = Square_after_rorate[i][j];
					Square_temp[j].x += 1;
					if (Square_temp[j].x > STAGE_WIDTH - 1) {
						rotateflag[i] = false;
					}
				}
				if (hit_test_plus(Square_temp) && rotateflag[i]) {
					if (check_is_can_drop(Square_temp, Square_temp[0].y, 0))
					{
						return true;
					}
					if (!totalflag) {
						totalflag = find_way_out(Square_temp, 2);
					}
				}
			}
		}
	}

	return totalflag;
}

bool Tetris::hit_test_plus(Vector2i Square[4])
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
	}
	return true;
}