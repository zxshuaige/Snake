#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <list>
#include <conio.h>
#include <time.h>
#include <graphics.h>

using namespace std;

//���ô��ڴ�С
#define WINDOW_HEIGHT 540
#define WINDOW_WIDTH  720

//���õ�ͼ������
#define MAP_COL 30
#define MAP_ROW 30

//���ø��Ӵ�С
#define LAT WINDOW_HEIGHT/MAP_ROW

//����̰�����ƶ��ٶȣ����ٶ�
#define SPEED 60
#define A_SPEED 15

//�߳��ķ���
enum Direct{
	RIGHT = 77, 
	LEFT = 75, 
	UP = 72, 
	DOWN = 80
};

//����
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


//ʳ��
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

		while (!this->s_point.empty())    //���
			this->s_point.pop_back();

		Point pos(MAP_COL / 2, MAP_ROW / 2);   //������һ����  �ߵ�ͷ
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
			while (_kbhit()){    //�������Ƿ��а���  ������ʽ
				cmd = _getch();   //���شӼ��̻�ȡ���ַ�
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

	//��Ϸ��ʼ��
	void GnameInit(){
		sn.SnakeInit();     //�߳�ʼ��
		food.RandPoint(MAP_COL, MAP_ROW);  //ʳ���ʼ��
	}

	//����ͼ
	void DrawMap(){
		setfillcolor(DARKGRAY);
		solidrectangle(0, 0, LAT*MAP_COL, LAT*MAP_ROW);
		setcolor(LIGHTGRAY);
		line(WINDOW_HEIGHT, 0, WINDOW_HEIGHT, WINDOW_HEIGHT);

		//��ʳ��
		setfillcolor(RED);
		fillrectangle(food.f_point._x * LAT, food.f_point._y * LAT,
			(food.f_point._x + 1) * LAT, (food.f_point._y + 1) * LAT);

		//����
		setfillcolor(WHITE);
		for (auto iter : sn.s_point){
			solidrectangle(iter._x * LAT, iter._y * LAT, (iter._x + 1) *LAT, (iter._y + 1) * LAT);
		}
	}

	//�߳�ʳ��
	void EatFood(){
		Point head = sn.s_point.front();
		if (head == food.f_point){
			food.ishavefood = false;
			sn.s_point.push_back(sn.s_point.back());
			sn.length++;
		}
	}

	//�ж�ʳ���Ƿ�������
	bool OnSnake(Point pot){
		for (auto iter : sn.s_point){
			if (pot == iter)
				return true;
		}
		return false;
	}

	//����ʳ��
	void CreatFood(){
		while(food.ishavefood == false){
			food.RandPoint(MAP_COL, MAP_ROW); 
			if (OnSnake(food.f_point))
				break;
			food.ishavefood = true;
		}
	}

	//�Ƿ�ײǽ
	bool OnWall(Point head){
		return !(head._x >= 0 && head._y >= 0 && head._x < MAP_ROW && head._y < MAP_COL);
	}

	//�Ƿ�������пռ�
	bool isFill(){
		return sn.length == MAP_COL * MAP_ROW;
	}

	//�ж��Ƿ��ڳ���ͷ֮�����������
	bool OnOutHeadSnake(Point pot){
		auto head = sn.s_point.begin();
		for (head++; head != sn.s_point.end(); head++){
			if (*head == pot)
				return true;
		}
		return false;
	}

	//�ж���Ϸ����
	bool isGameOver(){
		if (OnOutHeadSnake(sn.s_point.front()) || OnWall(sn.s_point.front()) || isFill()){
			return true;
		}
		return false;
	}

	//����������
	void DrawGameEnd(){
		setfillcolor(BLACK);

		TCHAR *str = _T("GAME OVER...");
		if (isFill())
			str = _T("you are very six..");
		int len = (int)_tcslen(str);

		outtextxy(WINDOW_HEIGHT / 2 - len * 5, WINDOW_HEIGHT / 2 - 5, str);
	}

	//����ǰ����
	void DrawGameInfo(){
		setfillcolor(BLACK);
		solidrectangle(WINDOW_HEIGHT, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		IMAGE img;
		loadimage(&img, "C:\\Users\\zx\\Desktop\\xx.jpg");
		putimage(WINDOW_HEIGHT + 20, 20, &img);

		outtextxy(WINDOW_HEIGHT + 20, 130, _T("��ǰ�ٶ�:"));
		TCHAR str_v[10];
		sprintf(str_v, "%d", sn.speed);
		outtextxy(WINDOW_HEIGHT + 90, 130, str_v);

		outtextxy(WINDOW_HEIGHT + 20, 160, _T("��ǰ���ٶ�:"));
		TCHAR str_av[10];
		sprintf(str_av, "%d", sn.a_speed);
		outtextxy(WINDOW_HEIGHT + 120, 160, str_av);

		outtextxy(WINDOW_HEIGHT + 20, 190, _T("ʳ������:"));
		TCHAR str_food[20];
		sprintf(str_food, "( %d,", food.f_point._x);
		outtextxy(WINDOW_HEIGHT + 90, 190, str_food);
		sprintf(str_food, " %d )", food.f_point._y);
		outtextxy(WINDOW_HEIGHT + 120, 190, str_food);

		outtextxy(WINDOW_HEIGHT + 20, 220, _T("�÷���:"));
		TCHAR str[20];
		sprintf(str, "%d", sn.length);
		outtextxy(WINDOW_HEIGHT + 80, 220, str);
		// ������Ϣ
		TCHAR *str_dir;
		switch (sn.direct) {
		case RIGHT:	str_dir = _T("����:  RIGHT");	break;
		case DOWN:	str_dir = _T("����:  DOWN");		break;
		case LEFT:	str_dir = _T("����:  LEFT"); 	break;
		case UP:	str_dir = _T("����:  UP");		break;
		default:	str_dir = _T("����:  None");
		}
		outtextxy(WINDOW_HEIGHT + 20, 270, str_dir);
		// ��Ϸ����˵��
		outtextxy(WINDOW_HEIGHT + 20, 320, _T("����˵��:"));
		outtextxy(WINDOW_HEIGHT + 20, 340, _T("ʹ�÷��������"));
		outtextxy(WINDOW_HEIGHT + 20, 360, _T("�������������"));
		outtextxy(WINDOW_HEIGHT + 20, 380, _T("��  :      ��"));
		outtextxy(WINDOW_HEIGHT + 20, 400, _T("��  :      ��"));
		outtextxy(WINDOW_HEIGHT + 20, 420, _T("��  :      ��"));
		outtextxy(WINDOW_HEIGHT + 20, 440, _T("��  :      ��"));
		outtextxy(WINDOW_HEIGHT + 20, 460, _T("p   :    ��ͣ"));
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





















