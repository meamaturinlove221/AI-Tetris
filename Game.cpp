#include"Game.h"

using namespace std;

Game::Game()
{
	window_width = 1350;
	window_height = 1000;

	imgBGno = 1;
	imgSkinNo = 1;
	imgSetNo = 1;
	game_over == false;
	ButtonState_Start = Start_Dark;
	auto_restart = false;
	GA_on = true;

	window.create(VideoMode(window_width, window_height), L"Tetris");
}

Game::~Game()
{}

void Game::gameInitial()
{
	window.setFramerateLimit(0);
	LoadMediaData();

	is_game_hold = false;
	ButtonState_Hold = Hold_Dark;
	player1.role = rolePLAYER1;
	player2.role = rolePLAYER2;
	player1.Initail(&tTiles);
	player2.Initail(&tTiles);

	if (auto_restart ||  GA_on)
	{
		game_over = false;
		is_game_begin = true;
	}
	else
	{
		is_game_begin = false;
	}
}

void Game::LoadMediaData()
{
	stringstream ss;

	ss << "data/images/bg" << imgSetNo << ".jpg";
	if (!tBackground.loadFromFile(ss.str()))
	{
		cout << "BK image û���ҵ�" << endl;
	}

	ss.str("");
	ss << "data/images/tiles" << imgSetNo << ".jpg";
	if (!tTiles.loadFromFile(ss.str()))
	{
		cout << "tiles û���ҵ�" << endl;
	}

	if (!tFrame.loadFromFile("data/images/frame.png"))
	{
		cout << "frame û���ҵ�" << endl;
	}
	if (!tCover.loadFromFile("data/images/cover.png"))
	{
		cout << "cover û���ҵ�" << endl;
	}
	if (!tGameOver.loadFromFile("data/images/end.png"))
	{
		cout << "end û���ҵ�" << endl;
	}
	if (!tButtons.loadFromFile("data/images/button.png"))
	{
		cout << "button.png û���ҵ�" << endl;
	}
	if (!tSwitcher.loadFromFile("data/images/bgSwitch.png"))
	{
		cout << "bgSwap.png û���ҵ�" << endl;
	}

	sBackground.setTexture(tBackground);
	sTiles.setTexture(tTiles);
	sFrame.setTexture(tFrame);
	sCover.setTexture(tCover);
	sGameOver.setTexture(tGameOver);
	sButtons.setTexture(tButtons);
	sSwitcher.setTexture(tSwitcher);
	sSwitcher.setOrigin(sSwitcher.getLocalBounds().width / 2.0, sSwitcher.getLocalBounds().height / 2.0);

	if (!font.loadFromFile("data/Fonts/simsun.ttc"))
	{
		cout << "����û���ҵ�" << endl;
	}
	text.setFont(font);
}

void Game::gameInput()
{
	Event event;
	window.setKeyRepeatEnabled(false);
	while (window.pollEvent(event))
	{
		if (event.type == Event::Closed)
		{
			if (GA_on)
			{
				player2.saveInfo();
			}
			window.close();
			game_quit = true;
		}
		if (event.type == Event::EventType::KeyReleased && event.key.code == Keyboard::Escape)
		{
			if (GA_on)
			{
				player2.saveInfo();
			}
			window.close();
			game_quit = true;
		}
		if (event.type == Event::EventType::KeyReleased && event.key.code == Keyboard::F6)
		{
			//��F6 ����/�ر��Զ��ؿ�����
			auto_restart = !auto_restart;
			if (auto_restart || GA_on)
			{
				cout << "�Զ��ؿ�������" << endl;
			}
			else
			{
				cout << "�Զ��ؿ����ر�" << endl;
			}
		}
		if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Left)
		{
			if (ButtonRectStart.contains(event.mouseButton.x, event.mouseButton.y))
			{
				if (is_game_begin == false)
				{
					is_game_begin = true;
					player1.animationFlag = false;
					player2.animationFlag = false;
					ButtonState_Start = Close_Light;
				}
				else
				{
					is_game_begin = false;
					ButtonState_Start = Start_Light;
				}
			}
			if (ButtonRectHold.contains(event.mouseButton.x, event.mouseButton.y))
			{
				if (is_game_hold == false)
				{
					is_game_hold = true;
					ButtonState_Hold = Continue_Light;
				}
				else
				{
					is_game_hold = false;
					ButtonState_Hold = Hold_Light;
				}
			}

			if (ButtonRectLeft.contains(event.mouseButton.x, event.mouseButton.y))
			{
				imgSetNo--;
				if (imgSetNo < 1)//С�� 1 ��ʱ���Ƥ�� 4 ��ʼ��ѭ��
				{
					imgSetNo = 4;//�����ֻ�Ƥ��ͼ
				}
				LoadMediaData();
			}
			if (ButtonRectRight.contains(event.mouseButton.x, event.mouseButton.y))
			{
				imgSetNo++;
				if (imgSetNo > 4)//����Ƥ��ͼ������ʱ��
				{
					imgSetNo = 1;//�����ֻ�Ƥ��ͼ
				}
				LoadMediaData();
			}

		}

		if (event.type == sf::Event::MouseMoved)
		{
			if (ButtonRectStart.contains(event.mouseMove.x, event.mouseMove.y))
			{
				if (is_game_begin == false)
				{
					ButtonState_Start = Start_Light;

				}
				else
				{
					ButtonState_Start = Close_Light;
				}
			}
			else
			{

				if (is_game_begin == false)
				{
					ButtonState_Start = Start_Dark;
				}
				else
				{
					ButtonState_Start = Close_Dark;
				}
			}
			if (ButtonRectHold.contains(event.mouseMove.x, event.mouseMove.y))
			{
				if (is_game_hold == false)
				{
					ButtonState_Hold = Hold_Light;
				}
				else
				{
					ButtonState_Hold = Continue_Light;
				}
			}
			else
			{
				if (is_game_hold == false)
				{
					ButtonState_Hold = Hold_Dark;
				}
				else
				{
					ButtonState_Hold = Continue_Dark;
				}
			}
		}
		player1.Input(event);
		player2.Input(event);
	}
}


void Game::gameLogic()
{
	if (is_game_hold == true)
	{
		return;
	}

	/*float time = clock.getElapsedTime().asSeconds();
	clock.restart();
	player1.timer += time;
	player2.timer += time;

	player1.Logic();
	player2.Logic();*/

	if (player2.gameover && (auto_restart || GA_on))
	{
		gameInitial();
		is_game_begin = true;
		ButtonState_Start = Close_Light;
		player1.animationFlag = false;
		player2.animationFlag = false;
		player1.newShapeFunc();
		player2.newShapeFunc();
		return;
	}


	player2.AI_Logic();
}

void Game::DrawButton()
{
	int ButtonWidth, ButtonHeight;
	ButtonWidth = 110;
	ButtonHeight = sButtons.getLocalBounds().height;
	//ButtonRectStart
	sButtons.setTextureRect(IntRect(ButtonState_Start * ButtonWidth, 0, ButtonWidth, ButtonHeight));
	sButtons.setPosition(B_START_CORNER_X, B_START_CORNER_Y);
	ButtonRectStart.left = B_START_CORNER_X;
	ButtonRectStart.top = B_START_CORNER_Y;
	ButtonRectStart.width = ButtonWidth;
	ButtonRectStart.height = ButtonHeight;
	window.draw(sButtons);
	//ButtonRectHold
	sButtons.setTextureRect(IntRect(ButtonState_Hold * ButtonWidth, 0, ButtonWidth, ButtonHeight));
	sButtons.setPosition(B_HOLD_CORNER_X, B_HOLD_CORNER_Y);
	ButtonRectHold.left = B_HOLD_CORNER_X;
	ButtonRectHold.top = B_HOLD_CORNER_Y;
	ButtonRectHold.width = ButtonWidth;
	ButtonRectHold.height = ButtonHeight;
	window.draw(sButtons);
	//�����ز��л�
	ButtonWidth = sSwitcher.getLocalBounds().width;
	ButtonHeight = sSwitcher.getLocalBounds().height;
	//ButtonRectLeft
	sSwitcher.setPosition(B_LEFT_CORNER_X, B_LEFT_CORNER_Y);
	ButtonRectLeft.left = B_LEFT_CORNER_X - ButtonWidth / 2;
	ButtonRectLeft.top = B_LEFT_CORNER_Y - ButtonHeight / 2;
	ButtonRectLeft.width = ButtonWidth;
	ButtonRectLeft.height = ButtonHeight;
	window.draw(sSwitcher);
	//ButtonRectRight
	sSwitcher.setPosition(B_RIGHT_CORNER_X, B_RIGHT_CORNER_Y);
	ButtonRectRight.left = B_RIGHT_CORNER_X - ButtonWidth / 2;
	ButtonRectRight.top = B_RIGHT_CORNER_Y - ButtonHeight / 2;
	ButtonRectRight.width = ButtonWidth;
	ButtonRectRight.height = ButtonHeight;
	sSwitcher.rotate(180);
	window.draw(sSwitcher);
	sSwitcher.rotate(180);
}

void Game::gameDraw()
{
	window.clear();

	sBackground.setPosition(0, 0);
	window.draw(sBackground);
	window.draw(sFrame);

	player1.Draw(&window);
	player2.Draw(&window);

	sCover.setPosition(P1_STAGE_CORNER_X, P1_STAGE_CORNER_Y);
	window.draw(sCover);
	sCover.setPosition(P2_STAGE_CORNER_X, P2_STAGE_CORNER_Y);
	window.draw(sCover);

	DrawButton();
	Textout();
	DrawResults();

	window.display();
}

void Game::gameRun()
{
	do
	{
		gameInitial();

		while (game_over == false)
		{

			while (window.isOpen() && game_over == false)
			{
				gameInput();

				if (is_game_begin == true)
				{
					gameLogic();
				}
				else
				{
					gameInitial();
				}

				gameDraw();
			}
		}

	} while (!game_quit);
}

void Game::Textout()
{
	int initialY;
	int CharacterSize = 48;
	text.setCharacterSize(CharacterSize);
	text.setFillColor(Color(255, 0, 0, 255));
	text.setStyle(Text::Bold);
	text.setPosition(P1_SCORE_CORNER_X, P1_SCORE_CORNER_Y);
	std::stringstream ss;
	ss << player1.score;
	text.setString(ss.str());
	window.draw(text);
	text.setPosition(P2_SCORE_CORNER_X - CharacterSize * 3, P2_SCORE_CORNER_Y);
	ss.str("");
	ss << player2.score;
	text.setString(ss.str());
	window.draw(text);

	/*CharacterSize = 24;
	text.setCharacterSize(CharacterSize);
	text.setFillColor(Color(255, 255, 255, 255));
	text.setStyle(Text::Bold);
	initialY = INFO_CORNER_Y;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L"��� 1:");
	window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" �������WASD");
	window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" �ٽ��� Space ��");
	window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" ������ �� Ctrl ��");
	window.draw(text);
	initialY += CharacterSize * 1.5;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L"��� 2:");
	window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" ���������������");
	window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" �ٽ��� Enter ��");
	window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" ������ �� Ctrl ��");
	window.draw(text);
	initialY += 2 * CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L"�Դ��ײ����幦��");
	window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L"�˳��� Esc ��");
	window.draw(text);*/
}

void Game::DrawResults()
{
	int ButtonWidth, ButtonHeight;
	ButtonWidth = 250;
	ButtonHeight = sGameOver.getLocalBounds().height;

	if (player1.gameover || player2.gameover)
	{
		sGameOver.setTextureRect(IntRect(player1.gameover * ButtonWidth, 0, ButtonWidth, ButtonHeight));
		sGameOver.setPosition(P1_STAGE_CORNER_X + GRIDSIZE * 1.5, 0);
		window.draw(sGameOver);

		sGameOver.setTextureRect(IntRect(player2.gameover * ButtonWidth, 0, ButtonWidth, ButtonHeight));
		sGameOver.setPosition(P2_STAGE_CORNER_X + GRIDSIZE * 1.5, 0);
		window.draw(sGameOver);

		if (auto_restart || GA_on)
		{
			//���ڵ�ǰϵͳ�ĵ�ǰ����/ʱ��
			SYSTEMTIME s_time;
			GetLocalTime(&s_time);

			//���ֺܷ�ʱ���¼���ļ��� Ȼ���ؿ�
			ofstream in;
			in.open("./result.txt", ios::app);
			if (in.is_open() && !GA_on)
			{
				in << s_time.wYear << "��" << s_time.wMonth << "��" << s_time.wDay << "��";
				in << s_time.wHour << ":" << setw(2) << setfill('0') << s_time.wMinute << ":" << setw(2) << setfill('0') << s_time.wSecond;
				in << "  player1 : " << player1.score << "  player2 : " << player2.score << endl;
				in << endl;
			}
			in.close();//�ر��ļ�

			cout << s_time.wYear << "��" << s_time.wMonth << "��" << s_time.wDay << "��";
			cout << s_time.wHour << ":" << setw(2) << setfill('0') << s_time.wMinute << ":" << setw(2) << setfill('0') << s_time.wSecond;
			cout << "  player1 : " << player1.score << "  player2 : " << player2.score;
			if (GA_on)
			{
				GAINFO * info = (GAINFO*)player2.infoFile;
				cout << "  GA  " << "��" << info->generationNum - 100 << "��  " << "��" << info->count << "��Ⱥ" << endl;
			}
			else
			{
				cout << endl;
			}
		}
	}
}