#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <list>
#include <conio.h>
#include <time.h>
#include <graphics.h>

using namespace std;

//设置窗口大小
#define WINDOW_HEIGHT 540
#define WINDOW_WIDTH  720

//设置地图行列数
#define MAP_COL 30
#define MAP_ROW 30

//设置格子大小
#define LAT WINDOW_HEIGHT/MAP_ROW

//设置贪吃蛇移动速度，加速度
#define SPEED 60
#define A_SPEED 15

//蛇朝的方向
enum Direct{
	RIGHT = 77, 
	LEFT = 75, 
	UP = 72, 
	DOWN = 80
};

//坐标
class Point{
public:
	int _x;
	int _y;

public:
	Point(){}
	Point(int x, int y)
		:_x(x),
		_y(y)
	{}

	bool operator==(const Point &p)const{
		return _x == p._x && _y == p._y;
	}
	bool operator!=(const Point &p)const{
		return _x != p._x || _y != p._y;
	}
	
	~Point(){}
};


//食物
class Food
{
public:
	Food(){
		ishavefood = true;
	}
	~Food(){
		ishavefood = false;
	}
	void RandPoint(int x, int y){
		srand((unsigned)time(0));
		f_point._x = rand() % x;
		f_point._y = rand() % y;
	}
	
public:
	Point f_point;
	bool ishavefood;
};

class Snake{
public:
	int length;
	int speed;
	int a_speed;
	list<Point> s_point;
	Direct direct;

public:
	Snake(){
		SnakeInit();
	}
	list<Point> getSnake(){
		return this->s_point;
	}
	
	void SnakeInit(){
		this->length = 3;
		this->direct = RIGHT;
		this->speed = UP;
		this->a_speed = 0;

		while (!this->s_point.empty())    //清空
			this->s_point.pop_back();

		Point pos(MAP_COL / 2, MAP_ROW / 2);   //创建第一个点  蛇的头
		for (int i = 0; i < this->length; ++i){
			this->s_point.push_back(pos);
			pos._y++;
		}
	}
	void Move(){
		Point head = s_point.front();
		switch (direct){
		case RIGHT:
			head._x++;
			break;
		case LEFT:
			head._x--;
			break;
		case UP:
			head._y--;
			break;
		case DOWN:
			head._y++;
		}
		s_point.pop_back();
		s_point.push_front(head);
	}

	void Suspend(){
		setfillcolor(BLACK);
		TCHAR *str = _T("any key continue...");
		int len = (int)_tcslen(str);
		outtextxy(WINDOW_HEIGHT / 2 - len * 5, WINDOW_HEIGHT / 2 - 5, str);
	}
	void Turn(char cmd = 0x00){
		if (cmd == 0x00){
			while (_kbhit()){    //检测键盘是否有按键  无阻塞式
				cmd = _getch();   //返回从键盘获取的字符
			}
		}

		if (direct == cmd){
			a_speed = A_SPEED;
			speed = speed + a_speed;
		}
		else {
			a_speed = 0;
			speed = SPEED;
		}

		switch (cmd){
		case RIGHT:
			if (direct != LEFT)
				direct = RIGHT;
			break;
		case LEFT:
			if (direct != RIGHT)
				direct = LEFT;
			break; 
		case UP:
			if (direct != DOWN)
				direct = UP;
			break; 
		case DOWN:
			if (direct != UP)
				direct = DOWN;
			break;
		case 'p':
			Suspend();
			FlushBatchDraw();
			_getch();
			break;
		}
	}
};

class Game{
public:
	Snake sn;
	Food food;

public:
	Game(){
		GnameInit();
	}
	~Game(){}

	//游戏初始化
	void GnameInit(){
		sn.SnakeInit();     //蛇初始化
		food.RandPoint(MAP_COL, MAP_ROW);  //食物初始化
	}

	//画地图
	void DrawMap(){
		setfillcolor(DARKGRAY);
		solidrectangle(0, 0, LAT*MAP_COL, LAT*MAP_ROW);
		setcolor(LIGHTGRAY);
		line(WINDOW_HEIGHT, 0, WINDOW_HEIGHT, WINDOW_HEIGHT);

		//画食物
		setfillcolor(RED);
		fillrectangle(food.f_point._x * LAT, food.f_point._y * LAT,
			(food.f_point._x + 1) * LAT, (food.f_point._y + 1) * LAT);

		//画蛇
		setfillcolor(WHITE);
		for (auto iter : sn.s_point){
			solidrectangle(iter._x * LAT, iter._y * LAT, (iter._x + 1) *LAT, (iter._y + 1) * LAT);
		}
	}

	//蛇吃食物
	void EatFood(){
		Point head = sn.s_point.front();
		if (head == food.f_point){
			food.ishavefood = false;
			sn.s_point.push_back(sn.s_point.back());
			sn.length++;
		}
	}

	//判断食物是否在蛇上
	bool OnSnake(Point pot){
		for (auto iter : sn.s_point){
			if (pot == iter)
				return true;
		}
		return false;
	}

	//创建食物
	void CreatFood(){
		while(food.ishavefood == false){
			food.RandPoint(MAP_COL, MAP_ROW); 
			if (OnSnake(food.f_point))
				break;
			food.ishavefood = true;
		}
	}

	//是否撞墙
	bool OnWall(Point head){
		return !(head._x >= 0 && head._y >= 0 && head._x < MAP_ROW && head._y < MAP_COL);
	}

	//是否填充所有空间
	bool isFill(){
		return sn.length == MAP_COL * MAP_ROW;
	}

	//判断是否在除蛇头之外的其他点上
	bool OnOutHeadSnake(Point pot){
		auto head = sn.s_point.begin();
		for (head++; head != sn.s_point.end(); head++){
			if (*head == pot)
				return true;
		}
		return false;
	}

	//判断游戏结束
	bool isGameOver(){
		if (OnOutHeadSnake(sn.s_point.front()) || OnWall(sn.s_point.front()) || isFill()){
			return true;
		}
		return false;
	}

	//画结束画面
	void DrawGameEnd(){
		setfillcolor(BLACK);

		TCHAR *str = _T("GAME OVER...");
		if (isFill())
			str = _T("you are very six..");
		int len = (int)_tcslen(str);

		outtextxy(WINDOW_HEIGHT / 2 - len * 5, WINDOW_HEIGHT / 2 - 5, str);
	}

	//画当前分数
	void DrawGameInfo(){
		setfillcolor(BLACK);
		solidrectangle(WINDOW_HEIGHT, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		IMAGE img;
		loadimage(&img, "C:\\Users\\zx\\Desktop\\xx.jpg");
		putimage(WINDOW_HEIGHT + 20, 20, &img);

		outtextxy(WINDOW_HEIGHT + 20, 130, _T("当前速度:"));
		TCHAR str_v[10];
		sprintf(str_v, "%d", sn.speed);
		outtextxy(WINDOW_HEIGHT + 90, 130, str_v);

		outtextxy(WINDOW_HEIGHT + 20, 160, _T("当前加速度:"));
		TCHAR str_av[10];
		sprintf(str_av, "%d", sn.a_speed);
		outtextxy(WINDOW_HEIGHT + 120, 160, str_av);

		outtextxy(WINDOW_HEIGHT + 20, 190, _T("食物坐标:"));
		TCHAR str_food[20];
		sprintf(str_food, "( %d,", food.f_point._x);
		outtextxy(WINDOW_HEIGHT + 90, 190, str_food);
		sprintf(str_food, " %d )", food.f_point._y);
		outtextxy(WINDOW_HEIGHT + 120, 190, str_food);

		outtextxy(WINDOW_HEIGHT + 20, 220, _T("得分数:"));
		TCHAR str[20];
		sprintf(str, "%d", sn.length);
		outtextxy(WINDOW_HEIGHT + 80, 220, str);
		// 方向信息
		TCHAR *str_dir;
		switch (sn.direct) {
		case RIGHT:	str_dir = _T("方向:  RIGHT");	break;
		case DOWN:	str_dir = _T("方向:  DOWN");		break;
		case LEFT:	str_dir = _T("方向:  LEFT"); 	break;
		case UP:	str_dir = _T("方向:  UP");		break;
		default:	str_dir = _T("方向:  None");
		}
		outtextxy(WINDOW_HEIGHT + 20, 270, str_dir);
		// 游戏操作说明
		outtextxy(WINDOW_HEIGHT + 20, 320, _T("操作说明:"));
		outtextxy(WINDOW_HEIGHT + 20, 340, _T("使用方向键控制"));
		outtextxy(WINDOW_HEIGHT + 20, 360, _T("长按方向键加速"));
		outtextxy(WINDOW_HEIGHT + 20, 380, _T("上  :      ↑"));
		outtextxy(WINDOW_HEIGHT + 20, 400, _T("下  :      ↓"));
		outtextxy(WINDOW_HEIGHT + 20, 420, _T("左  :      ←"));
		outtextxy(WINDOW_HEIGHT + 20, 440, _T("右  :      →"));
		outtextxy(WINDOW_HEIGHT + 20, 460, _T("p   :    暂停"));
	}

};



int main(){
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
	BeginBatchDraw();

	Game game;
	game.DrawMap();
	FlushBatchDraw();
	_getch();
	while (true){
		game.EatFood();
		game.CreatFood();
		game.sn.Turn();
		game.sn.Move();
		if (game.isGameOver()){
			game.DrawMap();
			game.DrawGameEnd();
			game.DrawGameInfo();
			FlushBatchDraw();
			_getch();
			game.GnameInit();
		}
		game.DrawMap();
		game.DrawGameInfo();
		FlushBatchDraw();
		Sleep(10000 / (game.sn.speed + game.sn.a_speed));
	}
	EndBatchDraw();
	closegraph();
	system("pause");
	return 0;
}





















