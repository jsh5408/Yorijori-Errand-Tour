#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <math.h>
#include <time.h>
#include <conio.h>
#include "npcmodel.h"
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
/*SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 012);//���� �����ϴ� �Լ�->����ǥ �� �� ��� ����
���� 0 ��ο� �Ķ� 1 ��ο� �ʷ� 2 ��ο� �ϴ� 3
��ο� ���� 4 ��ο� ���� 5 ��ο� ��� 6 ȸ�� 7 ��ο� ȸ�� 8
�Ķ� 9 �ʷ� 10 �ϴ� 11 ���� 12 ���� 13 ��� 14 �Ͼ� 15*/

#define UP 72
#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define SPACE 32
//�� ��ü Ʋ ũ�� ����
#define GBOARD_HEIGHT 40
#define GBOARD_WIDTH 140
//���� â ũ�� ����
#define MAP_HEIGHT 40
#define MAP_WIDTH 51//���� x��ǥ�� 102

typedef struct food//���, ������ �´� ������ ����ü
{
	char *ch[6];
	char *name[5];
}food;
typedef struct nofood//���ǿ� ���� �ʴ� ������ ����ü
{
	char *ch[6];
}nofood;
typedef struct myfood
{
	char * ch[10];
}myfood;

food recipe[5];//�´� ������ �迭
nofood norecipe[5];//���� �ʴ� ������ �迭
myfood myrecipe; //PC�� ���� ��� �����ϴ� ����

typedef struct npcArr//npc ������ ������ ����ü
{
	COORD nPos;//��ǥ
	int npc_id;//NPCModel index
	int color;//��
	int direction;//�̵� ����, 0: npc ����, 1: ���� �̵�, 2: ������ �̵�
	int v;//�ӵ�
} npcArr;

npcArr npc[30];//�ִ� 30�� npc�� ������ �迭

COORD curPos;
COORD pc;//pc�� ��ǥ
int gameBoardInfo[MAP_HEIGHT + 1][MAP_WIDTH + 1];
//PC: 123 / 0: ����� / 1: ���� ���� / 100~104: food / -100~-104: nofood / ��: 111 / Ÿ: 222 / ��: 333 / �� : 444 / ���� �� �ִ� ��: 11
int Eatcnt = 0; //PC�� ���� ���! �������� �Ѿ�� ���� 0���� �ʱ�ȭ
int id; //������ ��ȣ
int stage = 1;
int sum_score = 0; //�� ���ھ�
int score = 0;//�������� ���ھ�
int recipe_flag = 1;	// ���������� ������ ��� ���ؼ�
int shinho = 0, shinho2 = 0; // ��ȣ�� ������: 0, �ʷϺ�: 1
int hall_flag = 0;
int hall_first, hall_second;
COORD hallPos;
int item_stop = 0;
char *plname;
int recipe_flag2 = 1;
int show_flag = 1;
//npc �� ����
int npc_color_1 = 14;//�ڵ���(�����)
int npc_color_2 = 4;//����(����)
int npc_color_3 = 12;//������� ����(������)
int npc_color_4 = 15;//�����(���)
int npc_color_5 = 9;//���� (�Ķ���)
int npc_color_6 = 7;//���� ������ ��(ȸ��)

int first = 9, second = 9; //���� �ڸ� /  ���� �ڸ�
int wave_y = 13;
int isEaten = 0;
int stop;
int r_first, r_second;
int temp_clock=100;
int paper = 0;

//������, ���(heart) ����
int timer_item = 0, paper_item = 0, card_item = 0;//���� ī�� ����
int heart = 3; //��� ����

int flag_finish = 0;
int flag_over = 0; // 1: �����ؾ���!
int timelimit; // ���ѽð� ����
int startTime; //���� �ð�

void IItem(int a);
void Item_Info(int a);
void DeleteNPC(char NPCModel[4][9], COORD npc);
//�١ڡ١ڡ١ڡ١ڡ١ڡ١� ���� ���� ���� ��!!!!!!

void center()
{
	HWND hWnd_console; HWND hWnd_desktop;
	RECT rect_console = { 0, }; RECT rect_desktop = { 0, };
	int x, y;

	hWnd_console = GetConsoleWindow();
	hWnd_desktop = GetDesktopWindow();

	GetWindowRect(hWnd_desktop, &rect_desktop);
	GetWindowRect(hWnd_console, &rect_console);

	x = (rect_desktop.left + rect_desktop.right - rect_console.right + rect_console.left) / 2;
	y = (rect_desktop.top + rect_desktop.bottom - rect_console.bottom + rect_console.top) / 2;

	SetWindowPos(hWnd_console, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void setcolor(int color, int bgcolor)// ���� ��(���ڻ�, ���� ����) 
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ((bgcolor & 0xf) << 4) | (color & 0xf));
}

void RemoveCursor(void)   //Ŀ�� ������ ����
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void SetCurrentCursorPos(int x, int y) //Ŀ��������set
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
COORD GetCurrentCursorPos(void) //Ŀ��������get
{
	COORD curPoint;
	CONSOLE_SCREEN_BUFFER_INFO curInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curPoint.X = curInfo.dwCursorPosition.X;
	curPoint.Y = curInfo.dwCursorPosition.Y;

	return curPoint;
}
//�١ڡ١ڡ١ڡ١ڡ١ڡ١� �⺻ ���� �Լ� ��!!!!!!

void Recipe_Set() //������ ����,��� ����� �������ִ� �Լ�
{
	recipe[0].ch[0] = "��"; //����
	recipe[0].name[0] = "����";
	recipe[0].ch[1] = "��"; //����
	recipe[0].name[1] = "����";
	recipe[0].ch[2] = "��"; //��ȣ��
	recipe[0].name[2] = "��ȣ��";
	recipe[0].ch[3] = "��"; //���̹���
	recipe[0].name[3] = "���̹���";
	recipe[0].ch[4] = "��"; //�κ�
	recipe[0].name[4] = "�κ�";
	recipe[0].ch[5] = "�����";//recipe 0 : ����� ��������1

	recipe[1].ch[0] = "��"; //���İ�Ƽ��
	recipe[1].name[0] = "���İ�Ƽ��";
	recipe[1].ch[1] = "��"; //����
	recipe[1].name[1] = "����";
	recipe[1].ch[2] = "��"; //����
	recipe[1].name[2] = "����";
	recipe[1].ch[3] = "��"; //������
	recipe[1].name[3] = "������";
	recipe[1].ch[4] = "��"; //�丶��ҽ�
	recipe[1].name[4] = "�丶��ҽ�";
	recipe[1].ch[5] = "���İ�Ƽ";//recipe 1 : ���İ�Ƽ ��������4

	recipe[2].ch[0] = "��"; //����
	recipe[2].name[0] = "����";
	recipe[2].ch[1] = "��"; //���
	recipe[2].name[1] = "���";
	recipe[2].ch[2] = "��"; //�ñ�ġ
	recipe[2].name[2] = "�ñ�ġ";
	recipe[2].ch[3] = "��"; //���
	recipe[2].name[3] = "���";
	recipe[2].ch[4] = "��"; //���⸧
	recipe[2].name[4] = "���⸧";
	recipe[2].ch[5] = "��ä";//recipe 2 : ��ä �������� 2

	recipe[3].ch[0] = "��"; //��
	recipe[3].name[0] = "��";
	recipe[3].ch[1] = "��"; //������
	recipe[3].name[1] = "������";
	recipe[3].ch[2] = "��"; //�ñ�ġ
	recipe[3].name[2] = "�ñ�ġ";
	recipe[3].ch[3] = "��"; //���⸧
	recipe[3].name[3] = "���⸧";
	recipe[3].ch[4] = "��"; //����Ķ���
	recipe[3].name[4] = "����Ķ���";
	recipe[3].ch[5] = "�����";//recipe 3 : ����� �������� 3
}
void NoRecipe_Set() //�����ǿ� ���� �ʴ� ����� �������ִ� �Լ�
{
	norecipe[1].ch[0] = "��";//1
	
	norecipe[2].ch[0] = "��";//1
	norecipe[2].ch[1] = "��";//2
	
	norecipe[3].ch[0] = "��";//1
	norecipe[3].ch[1] = "��";//2
	norecipe[3].ch[2] = "��";//3
}
//�١ڡ١ڡ١ڡ١ڡ١ڡ١� ������ ����!!!!!!

void Show_sinho()
{
	int x, y;

	SetCurrentCursorPos(2, 8);
	gameBoardInfo[8][1] = 1;
	printf("��");

	if (stage == 1)
	{
		if (first == 9 || first == 7 || first == 5 || first == 3 || first == 1)
		{
			DeleteNPC(NPCModel[1], npc[2].nPos);
			for (x = 2; x <= 98; x += 2)
			{
				for (y = 10; y <= 11; y++)
				{
					gameBoardInfo[y][x / 2] = 78;
				}
			}
			shinho = 0;

			if (second == 0)//90.
			{
				for (y = 10; y <= 11; y++)
				{
					SetCurrentCursorPos(2, y);
					printf("                                                                                                    ");
				}
				SetCurrentCursorPos(28, 11);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
				printf("�� �¸��� �������� �����ϰ� ž�����ּ���..");
				npc[2].nPos.X = 84;
			}
			else
			{

				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
				SetCurrentCursorPos(4, 10);
				printf(" ~~");
				SetCurrentCursorPos(8, 11);
				printf(" ~~");

				SetCurrentCursorPos(12, 10);
				printf(" ~~");
				SetCurrentCursorPos(16, 11);
				printf(" ~~");

				SetCurrentCursorPos(20, 10);
				printf(" ~~");
				SetCurrentCursorPos(24, 11);
				printf(" ~~");

				SetCurrentCursorPos(28, 10);
				printf(" ~~");
				SetCurrentCursorPos(32, 11);
				printf(" ~~");

				SetCurrentCursorPos(36, 10);
				printf(" ~~");
				SetCurrentCursorPos(40, 11);
				printf(" ~~");

				SetCurrentCursorPos(44, 10);
				printf(" ~~");
				SetCurrentCursorPos(48, 11);
				printf(" ~~");

				SetCurrentCursorPos(52, 10);
				printf(" ~~");
				SetCurrentCursorPos(56, 11);
				printf(" ~~");

				SetCurrentCursorPos(60, 10);
				printf(" ~~");
				SetCurrentCursorPos(64, 11);
				printf(" ~~");

				SetCurrentCursorPos(68, 10);
				printf(" ~~");
				SetCurrentCursorPos(72, 11);
				printf(" ~~");

				SetCurrentCursorPos(76, 10);
				printf(" ~~");
				SetCurrentCursorPos(80, 11);
				printf(" ~~");

				SetCurrentCursorPos(84, 10);
				printf(" ~~");
				SetCurrentCursorPos(88, 11);
				printf(" ~~");

				SetCurrentCursorPos(92, 10);
				printf(" ~~");
				SetCurrentCursorPos(96, 11);
				printf(" ~~");
			}
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
			SetCurrentCursorPos(4, 8);
			gameBoardInfo[8][2] = 1;
			printf("��");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
			SetCurrentCursorPos(6, 8);
			gameBoardInfo[8][3] = 1;
			printf("��");
		}
		else
		{
			shinho = 1;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
			SetCurrentCursorPos(4, 8);
			gameBoardInfo[8][2] = 1;
			printf("��");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
			SetCurrentCursorPos(6, 8);
			gameBoardInfo[8][3] = 1;
			printf("��");
		}
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}
void Show_sinho2()
{
	if (first == 2 || first == 5 || first == 8)
	{
		shinho2 = 0;

		if (item_stop == 0)
		{
			curPos.X = 24;
			curPos.Y = 25;
			SetCurrentCursorPos(curPos.X, curPos.Y);
			IItem(444);
			Item_Info(444);
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
		SetCurrentCursorPos(38, 20);
		gameBoardInfo[38 / 2][20] = 1;
		printf("��");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
		SetCurrentCursorPos(40, 20);
		gameBoardInfo[40 / 2][20] = 1;
		printf("��");
	}
	else
	{
		shinho2 = 1;

		curPos.X = 24;
		curPos.Y = 25;
		SetCurrentCursorPos(curPos.X, curPos.Y);
		printf(" ");
		gameBoardInfo[curPos.Y][curPos.X / 2] = 0;

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
		SetCurrentCursorPos(38, 20);
		gameBoardInfo[38 / 2][20] = 1;
		printf("��");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
		SetCurrentCursorPos(40, 20);
		gameBoardInfo[40 / 2][20] = 1;
		printf("��");
	}
	gameBoardInfo[42 / 2][20] = 1;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	printf("��");
}

void Draw_tree(int x, int y)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	SetCurrentCursorPos(x - 1, y);
	gameBoardInfo[y][(x - 1) / 2] = 1;
	gameBoardInfo[y][(x + 1) / 2] = 1;
	printf(" �� ");
	SetCurrentCursorPos(x - 1, y + 1);
	gameBoardInfo[y + 1][(x - 1) / 2] = 1;
	gameBoardInfo[y + 1][(x + 1) / 2] = 1;
	printf("�š�");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
	SetCurrentCursorPos(x - 1, y + 2);
	gameBoardInfo[y + 2][(x - 1) / 2] = 1;
	gameBoardInfo[y + 2][(x + 1) / 2] = 1;
	printf(" ][ ");
}

void Draw_hall(int x, int y)
{
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 55;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
	printf("��");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

void Draw_circle()
{
	int x;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	for (x = 68; x <= 96; x += 2)
	{
		SetCurrentCursorPos(x, 30);
		gameBoardInfo[30][x / 2] = 1;
		printf("��");

		SetCurrentCursorPos(x, 34);
		gameBoardInfo[34][x / 2] = 1;
		printf("��");

		SetCurrentCursorPos(x, 36);
		gameBoardInfo[36][x / 2] = 1;
		printf("��");
	}

	if (second == 5 || second == 4)
	{
		SetCurrentCursorPos(70, 30);
		gameBoardInfo[30][70 / 2] = 0;
		gameBoardInfo[30][(70 + 2) / 2] = 0;
		printf("    ");

		SetCurrentCursorPos(80, 36);
		gameBoardInfo[36][80 / 2] = 0;
		gameBoardInfo[36][(80 + 2) / 2] = 0;
		printf("    ");
	}
	else if (second != 5 && second != 4)
	{
		SetCurrentCursorPos(80, 36);
		gameBoardInfo[36][80 / 2] = 1;
		gameBoardInfo[36][(80 + 2) / 2] = 1;
		printf("�ܡ�");

		SetCurrentCursorPos(70, 30);
		gameBoardInfo[30][70 / 2] = 1;
		gameBoardInfo[30][(70 + 2) / 2] = 1;
		printf("�ܡ�");
	}
	if (second == 8 || second == 9)
	{
		SetCurrentCursorPos(82, 34);
		gameBoardInfo[34][82 / 2] = 0;
		gameBoardInfo[34][(82 + 2) / 2] = 0;
		printf("    ");

		SetCurrentCursorPos(94, 34);
		gameBoardInfo[34][94 / 2] = 0;
		gameBoardInfo[34][(94 + 2) / 2] = 0;
		printf("    ");
	}
	else if (second != 8 && second != 9)
	{
		SetCurrentCursorPos(82, 34);
		gameBoardInfo[34][82 / 2] = 1;
		gameBoardInfo[34][(82 + 2) / 2] = 1;
		printf("�ܡ�");

		SetCurrentCursorPos(94, 34);
		gameBoardInfo[34][94 / 2] = 1;
		gameBoardInfo[34][(94 + 2) / 2] = 1;
		printf("�ܡ�");
	}
}

void maze1()
{
	int x, y;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 03);
	//�̷�
	for (x = 8; x <= 92; x += 4)
	{
		SetCurrentCursorPos(x, 37); //�Ʒ�
		gameBoardInfo[37][x / 2] = 1; //
		gameBoardInfo[37][(x + 2) / 2] = 1; //
		printf("���");

		SetCurrentCursorPos(x, 22); //��
		gameBoardInfo[22][x / 2] = 1; //
		gameBoardInfo[22][(x + 2) / 2] = 1; //
		printf("���");
	}
	SetCurrentCursorPos(92, 22); //��
	gameBoardInfo[22][92 / 2] = 0; //
	gameBoardInfo[22][(92 + 2) / 2] = 0; //
	printf("    ");

	for (y = 22; y <= 37; y++)
	{
		SetCurrentCursorPos(6, y); //����
		gameBoardInfo[y][6 / 2] = 1; //
		printf("��");

		SetCurrentCursorPos(96, y); //������
		gameBoardInfo[y][96 / 2] = 1; //
		printf("��");
	}
	SetCurrentCursorPos(7, 26); //��
	gameBoardInfo[26][7 / 2] = 0; //
	printf("  ");

	SetCurrentCursorPos(7, 26 + 1); //��
	gameBoardInfo[26 + 1][7 / 2] = 0; //
	printf("  ");

	//����
	x = 12; y = 25;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	gameBoardInfo[y][(x + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	printf("��������");

	x = 36; y = 25;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	gameBoardInfo[y][(x + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	printf("�����������������������");

	x = 28; y = 28;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	gameBoardInfo[y][(x + 2) / 2] = 1;
	printf("���");

	x = 26; y = 28;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	gameBoardInfo[y][(x + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2) / 2] = 1;
	printf("������");

	x = 40; y = 28;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	gameBoardInfo[y][(x + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	printf("���������������������");

	x = 84; y = 28;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	gameBoardInfo[y][(x + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2) / 2] = 1;
	printf("�����");

	x = 12; y = 31;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	gameBoardInfo[y][(x + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	printf("�����������");

	x = 36; y = 31;

	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	gameBoardInfo[y][(x + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	printf("�����������������������");

	x = 12; y = 34;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	gameBoardInfo[y][(x + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	printf("��������");

	x = 36; y = 34;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	gameBoardInfo[y][(x + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	printf("�������������");

	x = 76; y = 34;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	gameBoardInfo[y][(x + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	printf("���������");

	//����
	x = 12; y = 32;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");

	x = 12; y = 26;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1);
	gameBoardInfo[y + 1 + 1][x / 2] = 1;
	printf("��");

	x = 30; y = 34;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1);
	gameBoardInfo[y + 1 + 1][x / 2] = 1;
	printf("��");

	x = 36; y = 32;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");

	x = 30; y = 23;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1);
	gameBoardInfo[y + 1 + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1 + 1);
	gameBoardInfo[y + 1 + 1 + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1 + 1 + 1);
	gameBoardInfo[y + 1 + 1 + 1 + 1][x / 2] = 1;
	printf("��");

	x = 18; y = 28;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1);
	gameBoardInfo[y + 1 + 1][x / 2] = 1;
	printf("��");

	x = 24; y = 28;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1);
	gameBoardInfo[y + 1 + 1][x / 2] = 1;
	printf("��");

	x = 56; y = 29;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");

	x = 64; y = 34;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1);
	gameBoardInfo[y + 1 + 1][x / 2] = 1;
	printf("��");

	x = 70; y = 32;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1);
	gameBoardInfo[y + 1 + 1][x / 2] = 1;
	printf("��");

	x = 84; y = 23;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1);
	gameBoardInfo[y + 1 + 1][x / 2] = 1;
	printf("��");

	x = 90; y = 23;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1);
	gameBoardInfo[y + 1 + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1 + 1);
	gameBoardInfo[y + 1 + 1 + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1 + 1 + 1);
	gameBoardInfo[y + 1 + 1 + 1 + 1][x / 2] = 1;
	printf("��");

	x = 44; y = 26;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");

	x = 84; y = 31;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1);
	gameBoardInfo[y + 1 + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1 + 1);
	gameBoardInfo[y + 1 + 1 + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1 + 1 + 1);
	gameBoardInfo[y + 1 + 1 + 1 + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1 + 1 + 1 + 1);
	gameBoardInfo[y + 1 + 1 + 1 + 1 + 1][x / 2] = 1;
	printf("��");


	x = 90; y = 31;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1);
	gameBoardInfo[y + 1 + 1][x / 2] = 1;
	printf("��");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}
void maze2()
{
	int x, y;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
	//�̷�
	for (x = 8; x <= 92; x += 4)
	{
		SetCurrentCursorPos(x, 37); //�Ʒ�
		gameBoardInfo[37][x / 2] = 1; //
		gameBoardInfo[37][(x + 2) / 2] = 1; //
		printf("���");

		SetCurrentCursorPos(x, 22); //��
		gameBoardInfo[22][x / 2] = 1; //
		gameBoardInfo[22][(x + 2) / 2] = 1; //
		printf("���");
	}
	SetCurrentCursorPos(92, 22); //��
	gameBoardInfo[22][92 / 2] = 0; //
	gameBoardInfo[22][(92 + 2) / 2] = 0; //
	printf("    ");

	for (y = 22; y <= 37; y++)
	{
		SetCurrentCursorPos(6, y); //����
		gameBoardInfo[y][6 / 2] = 1; //
		printf("��");

		SetCurrentCursorPos(96, y); //������
		gameBoardInfo[y][96 / 2] = 1; //
		printf("��");
	}
	SetCurrentCursorPos(7, 26); //��
	gameBoardInfo[26][7 / 2] = 0; //
	printf("  ");

	SetCurrentCursorPos(7, 26 + 1); //��
	gameBoardInfo[26 + 1][7 / 2] = 0; //
	printf("  ");

	//����
	x = 24; y = 25;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	gameBoardInfo[y][(x + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	printf("��������");

	x = 48; y = 25;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	gameBoardInfo[y][(x + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	printf("�����������������");

	x = 28; y = 28;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	gameBoardInfo[y][(x + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	printf("���������");

	x = 54; y = 28;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	gameBoardInfo[y][(x + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	printf("��������������������");

	x = 12; y = 31;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	gameBoardInfo[y][(x + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	printf("����������");

	x = 34; y = 31;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	gameBoardInfo[y][(x + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	printf("���������������������������");

	x = 18; y = 34;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	gameBoardInfo[y][(x + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2) / 2] = 1;
	printf("����");

	x = 30; y = 34;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	gameBoardInfo[y][(x + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	printf("�����������");

	x = 54; y = 34;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	gameBoardInfo[y][(x + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2 + 2) / 2] = 1;
	printf("�����������������");

	x = 90; y = 34;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	gameBoardInfo[y][(x + 2) / 2] = 1;
	gameBoardInfo[y][(x + 2 + 2) / 2] = 1;
	printf("����");

	//����
	x = 12; y = 23;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1);
	gameBoardInfo[y + 1 + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1 + 1);
	gameBoardInfo[y + 1 + 1 + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1 + 1 + 1);
	gameBoardInfo[y + 1 + 1 + 1 + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1 + 1 + 1 + 1);
	gameBoardInfo[y + 1 + 1 + 1 + 1 + 1][x / 2] = 1;
	printf("��");

	x = 18; y = 23;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1);
	gameBoardInfo[y + 1 + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1 + 1);
	gameBoardInfo[y + 1 + 1 + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1 + 1 + 1);
	gameBoardInfo[y + 1 + 1 + 1 + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1 + 1 + 1 + 1);
	gameBoardInfo[y + 1 + 1 + 1 + 1 + 1][x / 2] = 1;
	printf("��");

	x = 36; y = 23;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");

	x = 42; y = 23;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1);
	gameBoardInfo[y + 1 + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1 + 1);
	gameBoardInfo[y + 1 + 1 + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1 + 1 + 1);
	gameBoardInfo[y + 1 + 1 + 1 + 1][x / 2] = 1;
	printf("��");

	x = 90; y = 23;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1);
	gameBoardInfo[y + 1 + 1][x / 2] = 1;
	printf("��");

	x = 42; y = 23;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1);
	gameBoardInfo[y + 1 + 1][x / 2] = 1;
	printf("��");

	x = 84; y = 25;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1);
	gameBoardInfo[y + 1 + 1][x / 2] = 1;
	printf("��");

	x = 28; y = 29;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1);
	gameBoardInfo[y + 1 + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1 + 1);
	gameBoardInfo[y + 1 + 1 + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1 + 1 + 1);
	gameBoardInfo[y + 1 + 1 + 1 + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1 + 1 + 1 + 1);
	gameBoardInfo[y + 1 + 1 + 1 + 1 + 1][x / 2] = 1;
	printf("��");

	x = 22; y = 35;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");

	x = 12; y = 32;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1);
	gameBoardInfo[y + 1 + 1][x / 2] = 1;
	printf("��");

	x = 54; y = 35;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");

	x = 90; y = 31;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1);
	gameBoardInfo[y + 1 + 1][x / 2] = 1;
	printf("��");

	x = 84; y = 32;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");

	x = 48; y = 26;
	SetCurrentCursorPos(x, y);
	gameBoardInfo[y][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1);
	gameBoardInfo[y + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1);
	gameBoardInfo[y + 1 + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1 + 1);
	gameBoardInfo[y + 1 + 1 + 1][x / 2] = 1;
	printf("��");
	SetCurrentCursorPos(x, y + 1 + 1 + 1 + 1);
	gameBoardInfo[y + 1 + 1 + 1 + 1][x / 2] = 1;
	printf("��");

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}
void Delete_maze()
{
	int x, y;
	for (y = 23; y <= 36; y++)
	{
		for (x = 8; x <= 92; x += 4)
		{
			if (gameBoardInfo[y][x / 2] == 1)
			{
				SetCurrentCursorPos(x, y);
				gameBoardInfo[y][x / 2] = 0;
				printf("  ");
			}
			if (gameBoardInfo[y][(x + 2) / 2] == 1)
			{
				SetCurrentCursorPos(x + 2, y);
				gameBoardInfo[y][(x + 2) / 2] = 0;
				printf("  ");
			}
		}
	}
}

void Draw_wave()
{
	int x, y;
	if (wave_y < 8)
	{
		for (y = 8; y <= 13; y++)
		{
			for (x = 16; x <= 84; x += 4)
			{
				gameBoardInfo[y][x / 2] = 0;
				gameBoardInfo[y][(x + 2) / 2] = 0;
			}
			SetCurrentCursorPos(16, y);
			printf("                                                                        ");
			if (isEaten == 0)
			{
				SetCurrentCursorPos(56, 12);
				gameBoardInfo[12][56 / 2] = 104;
				printf("%s", recipe[stage - 1].ch[4]);
			}
		}
		wave_y = 13;
	}
	else
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);

		if (second % 2 == 0)
		{
			for (x = 16; x <= 84; x += 4)
			{
				gameBoardInfo[wave_y][x / 2] = 1;
				gameBoardInfo[wave_y][(x + 2) / 2] = 1;
			}
			SetCurrentCursorPos(16, wave_y);
			printf("�ɢɢɢɢɢɢɢɢɢɢɢɢɢɢɢɢɢɢɢɢɢɢɢɢɢɢɢɢɢɢɢɢɢɢɢ�");
			wave_y--;
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	}
}

void ShowRoad1()
{
	int x;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
	for (x = 2; x <= 98; x += 4)
	{
		SetCurrentCursorPos(x, 9);
		gameBoardInfo[9][x / 2] = 11;
		gameBoardInfo[9][(x + 2) / 2] = 11;
		printf(" ~~");

		SetCurrentCursorPos(x, 12);
		gameBoardInfo[12][x / 2] = 11;
		gameBoardInfo[12][(x + 2) / 2] = 11;
		printf(" ~~");
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}
void ShowRoad2()
{
	int x;

	for (x = 2; x <= 98; x += 4)
	{
		if (x <= 88)
		{
			SetCurrentCursorPos(x, 9);
			gameBoardInfo[9][x / 2] = 1;
			gameBoardInfo[9][(x + 2) / 2] = 1;
			printf("����");
		}
		if (x == 30 || x == 66)
			continue;
		SetCurrentCursorPos(x, 3);
		gameBoardInfo[3][x / 2] = 1;
		gameBoardInfo[3][(x + 2) / 2] = 1;
		printf("����");
	}

	for (x = 2; x <= 98; x += 4)
	{
		if (x <= 88)
		{
			if (x == 34)
				continue;
			SetCurrentCursorPos(x, 12);
			gameBoardInfo[12][x / 2] = 1;
			gameBoardInfo[12][(x + 2) / 2] = 1;
			printf("����");
		}
	}

	for (x = 2; x <= 98; x += 4)
	{
		if (x == 66 || (x >= 26 && x <= 34))
			continue;
		SetCurrentCursorPos(x, 18);
		gameBoardInfo[18][x / 2] = 1;
		gameBoardInfo[18][(x + 2) / 2] = 1;
		printf("����");
	}

	for (x = 2; x <= 98; x += 4)
	{
		if (x >= 26 && x <= 34)
			continue;
		SetCurrentCursorPos(x, 21);
		gameBoardInfo[21][x / 2] = 1;
		gameBoardInfo[21][(x + 2) / 2] = 1;
		printf("����");
	}

	for (x = 14; x <= 98; x += 4)
	{
		if (x >= 26 && x <= 34)
			continue;
		SetCurrentCursorPos(x, 27);
		gameBoardInfo[27][x / 2] = 1;
		gameBoardInfo[27][(x + 2) / 2] = 1;
		printf("����");
	}

	for (x = 2; x <= 98; x += 4)
	{
		if (x >= 14)
		{
			SetCurrentCursorPos(x, 30);
			gameBoardInfo[30][x / 2] = 1;
			gameBoardInfo[30][(x + 2) / 2] = 1;
			printf("����");
		}
		if (x >= 80 && x <= 88)
			continue;
		SetCurrentCursorPos(x, 37);
		gameBoardInfo[37][x / 2] = 1;
		gameBoardInfo[37][(x + 2) / 2] = 1;
		printf("����");
	}
}
void ShowRoad4()
{
	int x, y;

	for (x = 2; x <= 100; x += 2)
	{
		if ((x < 90 || x > 96) && (x % 4 == 0))
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
			SetCurrentCursorPos(x, 18);
			gameBoardInfo[18][x / 2] = 1;
			printf("��");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		}

		if ((x >= 6 && x <= 12) || (x >= 90 && x <= 96))
		{
			if (x == 6)
			{
				SetCurrentCursorPos(x, 3);
				gameBoardInfo[3][x / 2] = 1;
				printf("��");

				SetCurrentCursorPos(x, 6);
				gameBoardInfo[6][x / 2] = 1;
				printf("��");

				SetCurrentCursorPos(x, 15);
				gameBoardInfo[15][x / 2] = 1;
				printf("��");
			}
			else if (x == 12)
			{
				SetCurrentCursorPos(x, 3);
				gameBoardInfo[3][x / 2] = 1;
				printf("��");

				SetCurrentCursorPos(x, 6);
				gameBoardInfo[6][x / 2] = 1;
				printf("��");

				SetCurrentCursorPos(x, 15);
				gameBoardInfo[15][x / 2] = 1;
				printf("��");
			}
			else if (x == 90)
			{
				SetCurrentCursorPos(x, 3);
				gameBoardInfo[3][x / 2] = 1;
				printf("��");

				SetCurrentCursorPos(x, 6);
				gameBoardInfo[6][x / 2] = 1;
				printf("��");

				SetCurrentCursorPos(x, 15);
				gameBoardInfo[15][x / 2] = 1;
				printf("��");

			}
			else if (x == 96)
			{
				SetCurrentCursorPos(x, 3);
				gameBoardInfo[3][x / 2] = 1;
				printf("��");

				SetCurrentCursorPos(x, 6);
				gameBoardInfo[6][x / 2] = 1;
				printf("��");

				SetCurrentCursorPos(x, 15);
				gameBoardInfo[15][x / 2] = 1;
				printf("��");
			}
			continue;
		}
		if (x % 4 == 0)
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);

			SetCurrentCursorPos(x, 3);
			gameBoardInfo[3][x / 2] = 1;
			printf("��");

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		}
		SetCurrentCursorPos(x, 6);
		gameBoardInfo[6][x / 2] = 1;
		printf("��");

		SetCurrentCursorPos(x, 15);
		gameBoardInfo[15][x / 2] = 1;
		printf("��");
	}


	for (y = 1; y <= 21; y++)
	{
		if ((y >= 3 && y <= 6)) continue;
		if ((y >= 15 && y <= 18)) continue;

		SetCurrentCursorPos(90, y);
		gameBoardInfo[y][90 / 2] = 1;
		printf("��");
		SetCurrentCursorPos(96, y);
		gameBoardInfo[y][96 / 2] = 1;
		printf("��");

		if (y > 18) continue;

		SetCurrentCursorPos(6, y);
		gameBoardInfo[y][6 / 2] = 1;
		printf("��");
		SetCurrentCursorPos(12, y);
		gameBoardInfo[y][12 / 2] = 1;
		printf("��");
	}

	SetCurrentCursorPos(6, 9);
	gameBoardInfo[9][6 / 2] = 0;
	printf("  ");
	SetCurrentCursorPos(6, 10);
	gameBoardInfo[10][6 / 2] = 0;
	printf("  ");

	SetCurrentCursorPos(20, 6);
	gameBoardInfo[6][20 / 2] = 0;
	printf("  ");
	SetCurrentCursorPos(22, 6);
	gameBoardInfo[6][22 / 2] = 0;
	printf("  ");

	SetCurrentCursorPos(96, 13);
	gameBoardInfo[13][96 / 2] = 0;
	printf("  ");
	SetCurrentCursorPos(96, 12);
	gameBoardInfo[12][96 / 2] = 0;
	printf("  ");

	SetCurrentCursorPos(64, 18);
	gameBoardInfo[18][64 / 2] = 0;
	printf("  ");
	SetCurrentCursorPos(66, 18);
	gameBoardInfo[18][66 / 2] = 0;
	printf("  ");

	SetCurrentCursorPos(48, 3);
	gameBoardInfo[3][48 / 2] = 0;
	printf("  ");
	SetCurrentCursorPos(46, 3);
	gameBoardInfo[3][46 / 2] = 0;
	printf("  ");
}

void ShowMap1()
{
	int x, y;
	// ����/������ ��
	for (y = 1; y <= GBOARD_HEIGHT - 1; y++)
	{
		if (!(y >= 8 && y <= 12) && !(y >= 28 && y <= 34))
		{
			SetCurrentCursorPos(2, y);
			gameBoardInfo[y][1] = 1; //���� ��
			gameBoardInfo[y][2] = 1; //���� ��
			printf("���");
			//printf("%d %d", gameBoardInfo[y][x / 2], gameBoardInfo[y][(x + 2) / 2]);
			SetCurrentCursorPos(98, y);
			gameBoardInfo[y][49] = 1; //������ ��
			gameBoardInfo[y][50] = 1; //������ ��
			printf("���");
			//printf("%d %d", gameBoardInfo[y][(102 - x - 2) / 2], gameBoardInfo[y][(102 - x) / 2]);
		}
		if (y == 8)
		{
			SetCurrentCursorPos(98, y);
			gameBoardInfo[y][49] = 1; //������ ��ȣ�� ������ ��
			gameBoardInfo[y][50] = 1; //������ ��ȣ�� ������ ��
			printf("���");
		}
	}
	// ���� ��
	for (x = 6; x <= 94; x += 4)
	{
		if (x>54 || x<50)
		{
			SetCurrentCursorPos(x, 1); //���� ��
			gameBoardInfo[1][x / 2] = 1; //���� 
			gameBoardInfo[1][(x + 2) / 2] = 1; //������
			printf("���");

			SetCurrentCursorPos(x, 2); //���� ��
			gameBoardInfo[2][x / 2] = 1; //����
			gameBoardInfo[2][(x + 2) / 2] = 1; //������
			printf("���");

			SetCurrentCursorPos(x, 38); //�Ʒ��� ��
			gameBoardInfo[38][x / 2] = 1; //����
			gameBoardInfo[38][(x + 2) / 2] = 1; //������
			printf("���");

			SetCurrentCursorPos(x, 39); //�Ʒ��� ��
			gameBoardInfo[39][x / 2] = 1; //����
			gameBoardInfo[39][(x + 2) / 2] = 1; //������
			printf("���");
		}
	}
	//���� �̷�
	for (x = 6; x <= 22; x += 4)
	{
		if (x != 22)
		{
			SetCurrentCursorPos(x, 13);
			gameBoardInfo[13][x / 2] = 1;
			gameBoardInfo[13][(x + 2) / 2] = 1;
			printf("���");

			SetCurrentCursorPos(x, 14);
			gameBoardInfo[14][x / 2] = 1;
			gameBoardInfo[14][(x + 2) / 2] = 1;
			printf("���");

			SetCurrentCursorPos(x, 16);
			gameBoardInfo[16][x / 2] = 1;
			gameBoardInfo[16][(x + 2) / 2] = 1;
			printf("���");

			SetCurrentCursorPos(x, 20);
			gameBoardInfo[20][x / 2] = 1;
			gameBoardInfo[20][(x + 2) / 2] = 1;
			printf("���");

			SetCurrentCursorPos(x, 26);
			gameBoardInfo[26][x / 2] = 1;
			gameBoardInfo[26][(x + 2) / 2] = 1;
			printf("���");

			SetCurrentCursorPos(x, 27);
			gameBoardInfo[27][x / 2] = 1;
			gameBoardInfo[27][(x + 2) / 2] = 1;
			printf("���");
		}
		if (x != 6)
		{
			SetCurrentCursorPos(x, 18);
			gameBoardInfo[18][x / 2] = 1;
			gameBoardInfo[18][(x + 2) / 2] = 1;
			printf("���");

			SetCurrentCursorPos(x, 22);
			gameBoardInfo[22][x / 2] = 1;
			gameBoardInfo[22][(x + 2) / 2] = 1;
			printf("���");

			SetCurrentCursorPos(x, 24);
			gameBoardInfo[24][x / 2] = 1;
			gameBoardInfo[24][(x + 2) / 2] = 1;
			printf("���");
		}
	}
	//������ �̷�
	for (x = 78; x <= 94; x += 4)
	{
		if (x != 78)
		{
			SetCurrentCursorPos(x, 13);
			gameBoardInfo[13][x / 2] = 1;
			gameBoardInfo[13][(x + 2) / 2] = 1;
			printf("���");

			SetCurrentCursorPos(x, 14);
			gameBoardInfo[14][x / 2] = 1;
			gameBoardInfo[14][(x + 2) / 2] = 1;
			printf("���");

			SetCurrentCursorPos(x, 16);
			gameBoardInfo[16][x / 2] = 1;
			gameBoardInfo[16][(x + 2) / 2] = 1;
			printf("���");

			SetCurrentCursorPos(x, 20);
			gameBoardInfo[20][x / 2] = 1;
			gameBoardInfo[20][(x + 2) / 2] = 1;
			printf("���");

			SetCurrentCursorPos(x, 26);
			gameBoardInfo[26][x / 2] = 1;
			gameBoardInfo[26][(x + 2) / 2] = 1;
			printf("���");

			SetCurrentCursorPos(x, 27);
			gameBoardInfo[27][x / 2] = 1;
			gameBoardInfo[27][(x + 2) / 2] = 1;
			printf("���");
		}
		if (x != 94)
		{
			SetCurrentCursorPos(x, 18);
			gameBoardInfo[18][x / 2] = 1;
			gameBoardInfo[18][(x + 2) / 2] = 1;
			printf("���");

			SetCurrentCursorPos(x, 22);
			gameBoardInfo[22][x / 2] = 1;
			gameBoardInfo[22][(x + 2) / 2] = 1;
			printf("���");

			SetCurrentCursorPos(x, 24);
			gameBoardInfo[24][x / 2] = 1;
			gameBoardInfo[24][(x + 2) / 2] = 1;
			printf("���");
		}
	}

	ShowRoad1();

	for (x = 2; x <= 100; x += 2)
	{
		if (x >= 48 && x <= 56)
			continue;

		SetCurrentCursorPos(x, 34);
		gameBoardInfo[34][x / 2] = 1;
		printf("��");

		if ((x >= 22 && x <= 24) || (x >= 78 && x <= 80))
			continue;

		SetCurrentCursorPos(x, 28);
		gameBoardInfo[28][x / 2] = 1;
		printf("��");
	}
	for (x = 2; x <= 100; x += 4)
	{
		SetCurrentCursorPos(x, 31);
		gameBoardInfo[31][x / 2] = 1;
		printf("��");
	}

	for (x = 28; x <= 74; x += 2)
	{
		SetCurrentCursorPos(x, 13); //
		if (gameBoardInfo[13][x / 2] == 0 &&
			(x != 40 && x != 42 && x != 44 && x != 46 && x != 48) &&
			(x != 70 && x != 72))
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
			gameBoardInfo[13][x / 2] = 1;
			printf("��");
		}

		SetCurrentCursorPos(x, 27);
		if (gameBoardInfo[27][x / 2] == 0 &&
			(x != 48 && x != 50 && x != 52 && x != 54 && x != 56))
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
			gameBoardInfo[27][x / 2] = 1;
			printf("��");
		}
	}

	for (y = 13; y <= 27; y++)
	{
		SetCurrentCursorPos(26, y);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
		gameBoardInfo[y][13] = 1;
		printf("��");
		SetCurrentCursorPos(76, y);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
		gameBoardInfo[y][38] = 1;
		printf("��");
	}
	// ����
	Draw_tree(31, 20);
	Draw_tree(51, 15);
	Draw_tree(71, 19);
	Draw_tree(63, 24);
	Draw_tree(57, 24);
	Draw_tree(45, 23);
	Draw_tree(45, 18);
	Draw_tree(31, 14);
	Draw_tree(31, 20);
	Draw_tree(39, 24);
	Draw_tree(65, 16);
	Draw_tree(59, 20);
	// ��Ȧ
	for (x = 8; x < 96; x += 6)
	{
		Draw_hall(x, 4);
		Draw_hall(x + 2, 5);
		Draw_hall(x + 4, 6);
	}
}
void ShowMap2()
{
	int x;

	for (x = 2; x <= 100; x += 4)
	{
		SetCurrentCursorPos(x, 1); //���� ��
		gameBoardInfo[1][x / 2] = 1; //���� 
		gameBoardInfo[1][(x + 2) / 2] = 1; //������
		printf("���");
		if (x == 30 || x == 66)
			continue;
		SetCurrentCursorPos(x, 2); //���� ��
		gameBoardInfo[2][x / 2] = 1; //����
		gameBoardInfo[2][(x + 2) / 2] = 1; //������
		printf("���");
	}

	for (x = 2; x <= 100; x += 4)
	{
		SetCurrentCursorPos(x, 6);
		gameBoardInfo[6][x / 2] = 1;
		printf("��");
	}

	for (x = 2; x <= 86; x += 4)
	{
		SetCurrentCursorPos(x, 10); //���� ��
		gameBoardInfo[10][x / 2] = 1; //���� 
		gameBoardInfo[10][(x + 2) / 2] = 1; //������
		printf("���");
		if (x == 34)
			continue;
		SetCurrentCursorPos(x, 11); //���� ��
		gameBoardInfo[11][x / 2] = 1; //����
		gameBoardInfo[11][(x + 2) / 2] = 1; //������
		printf("���");
	}

	for (x = 2; x <= 100; x += 4)
	{
		SetCurrentCursorPos(x, 15);
		gameBoardInfo[15][x / 2] = 1;
		printf("��");
	}

	for (x = 2; x <= 100; x += 4)
	{
		if (x >= 26 && x <= 34)
			continue;
		SetCurrentCursorPos(x, 20); //���� ��
		gameBoardInfo[20][x / 2] = 1; //����
		gameBoardInfo[20][(x + 2) / 2] = 1; //������
		printf("���");
		if (x == 66)
			continue;
		SetCurrentCursorPos(x, 19); //���� ��
		gameBoardInfo[19][x / 2] = 1; //���� 
		gameBoardInfo[19][(x + 2) / 2] = 1; //������
		printf("���");
	}

	for (x = 14; x <= 100; x += 4)
	{
		SetCurrentCursorPos(x, 28); //���� ��
		gameBoardInfo[28][x / 2] = 1; //����
		gameBoardInfo[28][(x + 2) / 2] = 1; //������
		printf("���");

		SetCurrentCursorPos(x, 29); //���� ��
		gameBoardInfo[29][x / 2] = 1; //���� 
		gameBoardInfo[29][(x + 2) / 2] = 1; //������
		printf("���");
	}

	for (x = 2; x <= 100; x += 4)
	{
		SetCurrentCursorPos(x, 24);
		gameBoardInfo[24][x / 2] = 1;
		printf("��");
	}

	for (x = 2; x <= 100; x += 4)
	{
		if (x >= 80 && x <= 88)
			continue;
		SetCurrentCursorPos(x, 38); //���� ��
		gameBoardInfo[38][x / 2] = 1; //����
		gameBoardInfo[38][(x + 2) / 2] = 1; //������
		printf("���");

		SetCurrentCursorPos(x, 39); //���� ��
		gameBoardInfo[39][x / 2] = 1; //���� 
		gameBoardInfo[39][(x + 2) / 2] = 1; //������
		printf("���");
	}

	for (x = 2; x <= 100; x += 4)
	{
		SetCurrentCursorPos(x, 34);
		gameBoardInfo[34][x / 2] = 1;
		printf("��");
	}
	Draw_hall(33 * 2, 2);
	Draw_hall(18 * 2, 15);
	Draw_hall(22 * 2, 15);
	Draw_hall(28 * 2, 15);
	Draw_hall(36 * 2, 15);
	Draw_hall(36 * 2, 24);
	Draw_hall(34 * 2, 24);
	Draw_hall(32 * 2, 24);
	Draw_hall(4 * 2, 24);
	Draw_hall(44 * 2, 34);
	Draw_hall(24 * 2, 34);
	Draw_hall(14 * 2, 34);
	Draw_hall(44 * 2, 37);
	Draw_hall(42 * 2, 37);
}
void ShowMap3()
{
	int x, y;
	
	Draw_hall(4, 16);
	Draw_hall(4, 24);
	Draw_hall(20, 16);
	Draw_hall(36, 16);
	Draw_hall(62, 16);
	Draw_hall(78, 16);
	Draw_hall(54, 14);
	Draw_hall(20, 24);
	Draw_hall(36, 24);
	Draw_hall(62, 24);
	Draw_hall(78, 24);
	Draw_hall(48, 26);
	// ����, ������ ��
	for (y = 1; y <= GBOARD_HEIGHT - 1; y++)
	{
		if (y<16 || y>24)
		{
			SetCurrentCursorPos(2, y);
			gameBoardInfo[y][2 / 2] = 1;
			gameBoardInfo[y][(2 + 2) / 2] = 1;
			printf("���");

			SetCurrentCursorPos(98, y);
			gameBoardInfo[y][98 / 2] = 1;
			gameBoardInfo[y][(98 + 2) / 2] = 1;
			printf("���");
		}
	}
	// ����, �Ʒ��� ��
	for (x = 6; x <= 96; x += 4)
	{
		if (x>60 || x<42)
		{
			SetCurrentCursorPos(x, 1);
			gameBoardInfo[1][x / 2] = 1;
			gameBoardInfo[1][(x + 2) / 2] = 1;
			printf("���");

			SetCurrentCursorPos(x, 2);
			gameBoardInfo[2][x / 2] = 1;
			gameBoardInfo[2][(x + 2) / 2] = 1;
			printf("���");

			SetCurrentCursorPos(x, 38);
			gameBoardInfo[38][x / 2] = 1;
			gameBoardInfo[38][(x + 2) / 2] = 1;
			printf("���");

			SetCurrentCursorPos(x, 39);
			gameBoardInfo[39][x / 2] = 1;
			gameBoardInfo[39][(x + 2) / 2] = 1;
			printf("���");
		}
	}

	//���� �� �׸�
	for (x = 6; x<42; x += 4) //���� �� ���� ����
	{
		SetCurrentCursorPos(x, 14);
		gameBoardInfo[14][x / 2] = 1;
		gameBoardInfo[14][(x + 2) / 2] = 1;
		printf("���");

		SetCurrentCursorPos(x, 15);
		gameBoardInfo[15][x / 2] = 1;
		gameBoardInfo[15][(x + 2) / 2] = 1;
		printf("���");

		if (x == 6) continue;

		SetCurrentCursorPos(x, 12); // �ؿ� ���� ����
		gameBoardInfo[12][x / 2] = 1;
		gameBoardInfo[12][(x + 2) / 2] = 1;
		printf("���");
	}

	for (y = 3; y < 11; y++) //���� ����
	{
		SetCurrentCursorPos(38, y);
		gameBoardInfo[y][38 / 2] = 1;
		gameBoardInfo[y][(38 + 2) / 2] = 1;
		printf("���");
	}

	for (y = 4; y < 13; y++)
	{
		SetCurrentCursorPos(8, y); // ���ʿ��� ù��° ���� ���� 
		gameBoardInfo[y][8 / 2] = 1;
		printf("��");
	}

	SetCurrentCursorPos(38, 13); // �����ʿ��� �� ù��° ���� ���� 
	gameBoardInfo[13][38 / 2] = 1;
	gameBoardInfo[13][(38 + 2) / 2] = 1;
	printf("���");

	for (x = 14; x <= 34; x += 4) // �ؿ��� ����° ���� ����
	{
		SetCurrentCursorPos(x, 10);
		gameBoardInfo[10][x / 2] = 1;
		gameBoardInfo[10][(x + 2) / 2] = 1;
		printf("���");

		if (x == 34) continue;

		SetCurrentCursorPos(x, 4); // �ؿ� ���� ����
		gameBoardInfo[4][x / 2] = 1;
		gameBoardInfo[4][(x + 2) / 2] = 1;
		printf("���");
	}

	for (y = 5; y <= 9; y++) //�� �ո� ��� �� ��
	{
		SetCurrentCursorPos(14, y);
		gameBoardInfo[y][14 / 2] = 1;
		printf("��");

		if (y == 5) continue;

		SetCurrentCursorPos(22, y);
		gameBoardInfo[y][22 / 2] = 1;
		printf("��");

		SetCurrentCursorPos(30, y);
		gameBoardInfo[y][30 / 2] = 1;
		printf("��");
	}

	for (y = 4; y <= 8; y++) //�Ʒ� �ո� ��� �� ��
	{
		SetCurrentCursorPos(18, y);
		gameBoardInfo[y][18 / 2] = 1;
		printf("��");

		SetCurrentCursorPos(26, y);
		gameBoardInfo[y][26 / 2] = 1;
		printf("��");

		SetCurrentCursorPos(34, y);
		gameBoardInfo[y][34 / 2] = 1;
		printf("��");
	}

	//���� �Ʒ� �׸�
	for (y = 25; y <= 36; y++)
	{
		SetCurrentCursorPos(6, y); //���� ���� ����
		gameBoardInfo[y][6 / 2] = 1;
		gameBoardInfo[y][(6 + 2) / 2] = 1;
		printf("���");

		SetCurrentCursorPos(34, y); //���� ���� ����
		gameBoardInfo[y][34 / 2] = 1;
		gameBoardInfo[y][(34 + 2) / 2] = 1;
		printf("���");

		if (y != 36)
		{
			SetCurrentCursorPos(32, y); //������ ���� ����
			gameBoardInfo[y][32 / 2] = 1;
			printf("��");
		}

		if (y >= 29 && y <= 35)
		{
			SetCurrentCursorPos(12, y); //�ȿ� �׸� ���� ����
			gameBoardInfo[y][12 / 2] = 1;
			printf("��");

			SetCurrentCursorPos(28, y); //�ȿ� �׸� ���� ����
			gameBoardInfo[y][28 / 2] = 1;
			printf("��");
		}

		if (y <= 26) continue;
		SetCurrentCursorPos(38, y); //������ ���� ����
		gameBoardInfo[y][38 / 2] = 1;
		gameBoardInfo[y][(38 + 2) / 2] = 1;
		printf("���");
	}

	for (x = 12; x <= 28; x += 4)
	{
		SetCurrentCursorPos(x, 25);
		gameBoardInfo[25][x / 2] = 1;
		gameBoardInfo[25][(x + 2) / 2] = 1;
		printf("���");

		SetCurrentCursorPos(x, 26);
		gameBoardInfo[26][x / 2] = 1;
		gameBoardInfo[26][(x + 2) / 2] = 1;
		printf("���");

		if (x <= 14) continue;
		SetCurrentCursorPos(x, 27); //�� ���� ����
		gameBoardInfo[27][x / 2] = 1;
		gameBoardInfo[27][(x + 2) / 2] = 1;
		printf("���");
	}

	for (x = 14; x <= 26; x += 4)
	{
		SetCurrentCursorPos(x, 37);//�Ʒ� ���� ����
		gameBoardInfo[37][x / 2] = 1;
		gameBoardInfo[37][(x + 2) / 2] = 1;
		printf("���");

		SetCurrentCursorPos(x, 35); //�ȿ� �׸� ����
		gameBoardInfo[35][x / 2] = 1;
		gameBoardInfo[35][(x + 2) / 2] = 1;
		printf("���");

		SetCurrentCursorPos(x, 29); //�ȿ� �׸� ����
		gameBoardInfo[29][x / 2] = 1;
		gameBoardInfo[29][(x + 2) / 2] = 1;
		printf("���");
	}

	for (x = 14; x <= 22; x += 4)
	{
		for (y = 30; y <= 34; y++)
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
			SetCurrentCursorPos(x, y);
			printf("��");

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
			SetCurrentCursorPos(x + 2, y);
			printf("��");
		}
	}

	for (y = 30; y <= 34; y++)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
		SetCurrentCursorPos(26, y);
		printf("��");
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

	//������ �� �׸�
	for (y = 3; y <= 6; y++)
	{
		SetCurrentCursorPos(94, y);
		gameBoardInfo[y][94 / 2] = 1;
		gameBoardInfo[y][(94 + 2) / 2] = 1;
		printf("���");
	}

	for (y = 8; y <= 12; y++)
	{
		SetCurrentCursorPos(92, y);
		gameBoardInfo[y][92 / 2] = 1;
		gameBoardInfo[y][(92 + 2) / 2] = 1;
		printf("���");
	}

	for (x = 62; x <= 88; x += 4)
	{
		for (y = 4; y <= 15; y++)
		{
			SetCurrentCursorPos(x, y);
			gameBoardInfo[y][x / 2] = 1;
			gameBoardInfo[y][(x + 2) / 2] = 1;
			printf("���");
		}
	}

	for (x = 86; x <= 96; x += 4)
	{
		SetCurrentCursorPos(x, 13);
		gameBoardInfo[13][x / 2] = 1;
		gameBoardInfo[13][(x + 2) / 2] = 1;
		printf("���");

		SetCurrentCursorPos(x, 14);
		gameBoardInfo[14][x / 2] = 1;
		gameBoardInfo[14][(x + 2) / 2] = 1;
		printf("���");
	}

	//Ʈ��
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	SetCurrentCursorPos(77, 5);
	printf("��");
	SetCurrentCursorPos(75, 6);
	printf("��  ��");
	SetCurrentCursorPos(74, 7);
	printf("��    ��");
	SetCurrentCursorPos(73, 8);
	printf("�� �š� ��");
	SetCurrentCursorPos(71, 9);
	printf("��    ��    ��");
	SetCurrentCursorPos(70, 10);
	printf("��            ��");
	SetCurrentCursorPos(69, 11);
	printf("�� ��  �š�  �� ��");
	SetCurrentCursorPos(68, 12);
	printf("�š� �š�  �š� �š�");

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	SetCurrentCursorPos(77, 7);
	printf("��");
	SetCurrentCursorPos(74, 9);
	printf("��");
	SetCurrentCursorPos(80, 9);
	printf("��");
	SetCurrentCursorPos(73, 10);
	printf("�ڡ�");
	SetCurrentCursorPos(79, 10);
	printf("�ڡ�");

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
	SetCurrentCursorPos(68, 13);
	printf("        ���        ");
	SetCurrentCursorPos(68, 14);
	printf("        ���        ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

	//�Ʒ��� ������
	for (x = 62; x <= 90; x += 4)
	{
		SetCurrentCursorPos(x, 25);
		gameBoardInfo[25][x / 2] = 1;
		gameBoardInfo[25][(x + 2) / 2] = 1;
		printf("���");

		SetCurrentCursorPos(x, 26);
		gameBoardInfo[26][x / 2] = 1;
		gameBoardInfo[26][(x + 2) / 2] = 1;
		printf("���");
	}

	for (y = 27; y <= 39; y++)
	{
		SetCurrentCursorPos(62, y);
		gameBoardInfo[y][62 / 2] = 1;
		gameBoardInfo[y][(62 + 2) / 2] = 1;
		gameBoardInfo[y][(62 + 2 + 2) / 2] = 1;
		printf("����");
	}

	for (y = 1; y <= 39; y += 2)
	{
		if (y >= 17 && y <= 23) continue;
		SetCurrentCursorPos(48, y);
		gameBoardInfo[y][48 / 2] = 1;
		printf("��");

		SetCurrentCursorPos(54, y);
		gameBoardInfo[y][54 / 2] = 1;
		printf("��");
	}
	for (x = 2; x <= 100; x += 4)
	{
		if (x >= 42 && x <= 60) continue;
		SetCurrentCursorPos(x, 20);
		gameBoardInfo[20][x / 2] = 1;
		printf("��");
	}
	SetCurrentCursorPos(48, 20);
	gameBoardInfo[20][48 / 2] = 1;
	printf("��");
	SetCurrentCursorPos(54, 20);
	gameBoardInfo[20][54 / 2] = 1;
	printf("��");

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 03);
	for (x = 2; x <= 42; x += 4)
	{
		if (gameBoardInfo[15][x / 2] == 1)
		{
			SetCurrentCursorPos(x, 16);
			gameBoardInfo[16][x / 2] = 1;
			printf("��");
		}
		if (gameBoardInfo[25][x / 2] == 1)
		{
			SetCurrentCursorPos(x, 24);
			gameBoardInfo[24][x / 2] = 1;
			printf("��");
		}
	}
	for (x = 60; x <= 100; x += 4)
	{
		if (gameBoardInfo[15][x / 2] == 1)
		{
			SetCurrentCursorPos(x, 16);
			gameBoardInfo[16][x / 2] = 1;
			printf("��");
		}
		if (gameBoardInfo[25][x / 2] == 1)
		{
			SetCurrentCursorPos(x, 24);
			gameBoardInfo[24][x / 2] = 1;
			printf("��");
		}
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

	Draw_circle();
}
void ShowMap4()
{
	int x, y;
	// ����/������ ��
	for (y = 1; y <= GBOARD_HEIGHT - 1; y++)
	{
		if ((y >= 3 && y <= 6) || (y >= 15 && y <= 18)) continue;
		SetCurrentCursorPos(2, y);
		gameBoardInfo[y][1] = 1; //���� ��
		gameBoardInfo[y][2] = 1; //���� ��
		printf("���");
		SetCurrentCursorPos(98, y);
		gameBoardInfo[y][49] = 1; //������ ��
		gameBoardInfo[y][50] = 1; //������ ��
		printf("���");
	}
	Draw_hall(8, 3);
	Draw_hall(10, 3);
	Draw_hall(18, 3);
	Draw_hall(34, 3);
	Draw_hall(50, 3);
	Draw_hall(66, 3);
	Draw_hall(86, 3);
	Draw_hall(92, 3);
	Draw_hall(94, 3);
	Draw_hall(14, 18);
	Draw_hall(38, 18);
	Draw_hall(54, 18);
	Draw_hall(70, 18);
	Draw_hall(86, 18);
	Draw_hall(92, 21);
	Draw_hall(94, 19);

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
	SetCurrentCursorPos(8, 7);
	gameBoardInfo[7][8 / 2] = 1; //������ ��
	printf("��");

	SetCurrentCursorPos(10, 9);
	gameBoardInfo[9][10 / 2] = 1; //������ ��
	printf("��");

	SetCurrentCursorPos(8, 11);
	gameBoardInfo[11][8 / 2] = 1; //������ ��
	printf("��");

	SetCurrentCursorPos(10, 13);
	gameBoardInfo[13][10 / 2] = 1; //������ ��
	printf("��");
	///////////////
	SetCurrentCursorPos(92, 7);
	gameBoardInfo[7][92 / 2] = 1; //������ ��
	printf("��");

	SetCurrentCursorPos(94, 9);
	gameBoardInfo[9][94 / 2] = 1; //������ ��
	printf("��");

	SetCurrentCursorPos(92, 11);
	gameBoardInfo[11][92 / 2] = 1; //������ ��
	printf("��");

	SetCurrentCursorPos(94, 13);
	gameBoardInfo[13][94 / 2] = 1; //������ ��
	printf("��");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	// ���� ��
	for (x = 6; x <= 94; x += 4)
	{
		if (x > 10 && x < 90)
		{
			SetCurrentCursorPos(x, 1); //���� ��
			gameBoardInfo[1][x / 2] = 1; //���� 
			gameBoardInfo[1][(x + 2) / 2] = 1; //������
			printf("���");

			SetCurrentCursorPos(x, 2); //���� ��
			gameBoardInfo[2][x / 2] = 1; //����
			gameBoardInfo[2][(x + 2) / 2] = 1; //������
			printf("���");
		}
		SetCurrentCursorPos(x, 38); //�Ʒ��� ��
		gameBoardInfo[38][x / 2] = 1; //����
		gameBoardInfo[38][(x + 2) / 2] = 1; //������
		printf("���");

		SetCurrentCursorPos(x, 39); //�Ʒ��� ��
		gameBoardInfo[39][x / 2] = 1; //����
		gameBoardInfo[39][(x + 2) / 2] = 1; //������
		printf("���");

		if (x >= 90) continue;

		SetCurrentCursorPos(x, 19); //�Ʒ��� ��
		gameBoardInfo[19][x / 2] = 1; //����
		gameBoardInfo[19][(x + 2) / 2] = 1; //������
		printf("���");

		SetCurrentCursorPos(x, 20); //�Ʒ��� ��
		gameBoardInfo[20][x / 2] = 1; //����
		gameBoardInfo[20][(x + 2) / 2] = 1; //������
		printf("���");

		SetCurrentCursorPos(x, 21); //�Ʒ��� ��
		gameBoardInfo[21][x / 2] = 1; //����
		gameBoardInfo[21][(x + 2) / 2] = 1; //������
		printf("���");
	}

	maze1();

	for (x = 14; x < 90; x += 4)//���� �׸�
	{
		SetCurrentCursorPos(x, 7);
		gameBoardInfo[7][x / 2] = 1;
		gameBoardInfo[7][(x + 2) / 2] = 1;
		printf("���");

		SetCurrentCursorPos(x, 14);
		gameBoardInfo[14][x / 2] = 1;
		gameBoardInfo[14][(x + 2) / 2] = 1;
		printf("���");
	}
	for (y = 7; y <= 13; y++)
	{
		SetCurrentCursorPos(14, y);
		gameBoardInfo[y][14 / 2] = 1;
		printf("��");

		SetCurrentCursorPos(88, y);
		gameBoardInfo[y][88 / 2] = 1;
		printf("��");
	}

	SetCurrentCursorPos(20, 7);
	gameBoardInfo[7][20 / 2] = 0;
	printf("  ");
	SetCurrentCursorPos(22, 7);
	gameBoardInfo[7][22 / 2] = 0;
	printf("  ");

	SetCurrentCursorPos(4, 9);
	gameBoardInfo[9][4 / 2] = 0;
	printf("  ");
	SetCurrentCursorPos(4, 10);
	gameBoardInfo[10][4 / 2] = 0;
	printf("  ");

	SetCurrentCursorPos(98, 13);
	gameBoardInfo[13][98 / 2] = 0;
	printf("  ");
	SetCurrentCursorPos(98, 12);
	gameBoardInfo[12][98 / 2] = 0;
	printf("  ");

	SetCurrentCursorPos(64, 19);
	gameBoardInfo[19][64 / 2] = 0;
	printf("  ");
	SetCurrentCursorPos(66, 19);
	gameBoardInfo[19][66 / 2] = 0;
	printf("  ");

	SetCurrentCursorPos(48, 2);
	gameBoardInfo[2][48 / 2] = 0;
	printf("  ");
	SetCurrentCursorPos(46, 2);
	gameBoardInfo[2][46 / 2] = 0;
	printf("  ");
	ShowRoad4();

	Draw_wave();
}
//�١ڡ١ڡ١ڡ١ڡ١ڡ١� �� �׸��� �κ�!!!!!!

void makeInfoEmpty()
{
	int x, y;
	for (y = 1; y < MAP_HEIGHT; y++)
	{
		for (x = 1; x < MAP_WIDTH; x++)
		{
			if (gameBoardInfo[y][x] != 0)
			{
				SetCurrentCursorPos(x * 2, y);
				printf("  ");
				gameBoardInfo[y][x] = 0;
			}
		}
	}
	for (y = 0; y <= MAP_HEIGHT; y++)
	{
		gameBoardInfo[y][0] = 1;
		gameBoardInfo[y][MAP_WIDTH] = 1;
	}
	for (x = 0; x <= MAP_WIDTH; x++)
	{
		gameBoardInfo[0][x] = 1;
		gameBoardInfo[MAP_HEIGHT][x] = 1;
	}
}

void printInfo()
{
	int x, y;
	SetCurrentCursorPos(0, 0);
	for (y = 0; y <= MAP_HEIGHT; y++)
	{
		for (x = 0; x <= MAP_WIDTH; x++)
		{
			if (gameBoardInfo[y][x] == 78 || gameBoardInfo[y][x] == 0)
			{
				printf("  ");
				continue;
			}
			printf("%d ", gameBoardInfo[y][x]);
		}
		printf("\n");
	}
}

void GameMapDelete()//���� â ��ü�� �����ִ� �Լ�.. ���� �ֿ����ּ���
{
	int y;
	for (y = 1; y<MAP_HEIGHT; y++)
	{
		SetCurrentCursorPos(2, y);
		printf("                                                                                                    ");
	}
}

void TimeBoardDelete()//Ÿ�̸�+������ â �����ִ� �Լ�
{
	int y;
	for (y = 18; y<27; y++)
	{
		SetCurrentCursorPos(108, y);
		printf("              ");
	}
}

void ShowRecipe()
{
	int i;
	TimeBoardDelete();
	if (show_flag == 1)
	{
		SetCurrentCursorPos(68, 20);
		printf("10�� ���� �丮�� ���� ������");
		SetCurrentCursorPos(78, 22);
		printf("�� ������ּ���. . .");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
		SetCurrentCursorPos(90, 24);
		printf("�� �� ��");

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
		SetCurrentCursorPos(4, 38);
		printf("��");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		SetCurrentCursorPos(7, 38);
		printf("  ������ ���۵Ǹ� �ؿ� �丮 ����� �̸��� ���ֽ��ϴ�. . .");
		SetCurrentCursorPos(2, 41);
		printf("                                                                                             ");
		show_flag = 0;
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	curPos.X = 108; curPos.Y = 18;
	SetCurrentCursorPos(curPos.X, curPos.Y);
	for (i = 0; i<5; i++)
	{
		printf("%s: %s", recipe[id].ch[i], recipe[id].name[i]);
		curPos.Y += 2;
		SetCurrentCursorPos(curPos.X, curPos.Y);
	}
	if (paper == 0)
	{
		Sleep(10000);
		GameMapDelete();
	}
	else if (paper == 1)
		Sleep(5000);

}

int IsGameOver()
{
	int x, y;
	if (Eatcnt > 4)
	{
		flag_over = 1;
		return 1;//���� ����
	}
	if (heart == 0)
	{
		flag_over = 1;
		stage = 11;
		GameMapDelete();
		x = 18; y = 12;
		SetCurrentCursorPos(x, y++);
		printf("  �����           ����         ��        ��      �������");
		SetCurrentCursorPos(x, y++);
		printf("��        ��       ��      ��       ���    ���      ��");
		SetCurrentCursorPos(x, y++);
		printf("��                ��        ��      �� ��  �� ��      ��");
		SetCurrentCursorPos(x, y++);
		printf("��    ����      �������      ��   ��   ��      �������");
		SetCurrentCursorPos(x, y++);
		printf("��        ��      ��        ��      ��        ��      ��");
		SetCurrentCursorPos(x, y++);
		printf("��        ��      ��        ��      ��        ��      ��");
		SetCurrentCursorPos(x, y++);
		printf("  �����        ��        ��      ��        ��      �������");
		SetCurrentCursorPos(x, y++);
		SetCurrentCursorPos(x, y++);
		SetCurrentCursorPos(x, y++);
		SetCurrentCursorPos(x, y++);
		printf("  �����        ��        ��      �������      ������");
		SetCurrentCursorPos(x, y++);
		printf("��        ��      ��        ��      ��                ��        ��");
		SetCurrentCursorPos(x, y++);
		printf("��        ��      ��        ��      ��                ��        ��");
		SetCurrentCursorPos(x, y++);
		printf("��        ��      ��        ��      �������      ������");
		SetCurrentCursorPos(x, y++);
		printf("��        ��       ��      ��       ��                ��       ��");
		SetCurrentCursorPos(x, y++);
		printf("��        ��        ��    ��        ��                ��        ��");
		SetCurrentCursorPos(x, y++);
		printf("  �����            ���          �������      ��        ��");
		Sleep(2000);
		return 0;
	}
	if (timelimit >= 99)
	{
		flag_over = 1;
		return 1;
	}
	return 0;
}
//�١ڡ١ڡ١ڡ١ڡ١ڡ١� �ֿ� �Լ� ��!!!!!!
void drawBoard() // �ܼ�â �׸���
{//�������� -> ���� �´� �ŷ�!!
	int x, y;

	makeInfoEmpty();//->�̰� �� �ִ°���???
	// �� ����
	for (y = 0; y <= GBOARD_HEIGHT; y++)
	{
		SetCurrentCursorPos(0, y);
		if (y == GBOARD_HEIGHT)
		{
			printf("��");
		}
		else if (y == 0)
		{
			printf("��");
		}
		else
		{
			printf("��");
		}
	}
	// ���� ȭ��� ��Ȳ �ȳ� ȭ����� ���
	for (y = 0; y <= GBOARD_HEIGHT; y++)
	{
		SetCurrentCursorPos(102, y);
		printf("��");
	}
	// �� ������
	for (y = 0; y <= GBOARD_HEIGHT; y++)
	{
		SetCurrentCursorPos(140, y);
		if (y == GBOARD_HEIGHT)
		{
			printf("��");
		}
		else if (y == 0)
		{
			printf("��");
		}
		else
		{
			printf("��");
		}
	}
	// �� ��
	for (x = 2; x <= 138; x += 2)
	{
		SetCurrentCursorPos(x, 0);
		printf("��");
	}
	// �� �Ʒ�
	for (x = 2; x <= 138; x += 2)
	{
		SetCurrentCursorPos(x, 40);
		printf("��");
	}
	/////// ������
	// ����
	for (y = 4; y <= 14; y++)
	{
		SetCurrentCursorPos(106, y);
		if (y == 14)
		{
			printf("��");
		}
		else if (y == 4)
		{
			printf("��");
		}
		else
		{
			printf("��");
		}
	}
	// ������
	for (y = 4; y <= 14; y++)
	{
		SetCurrentCursorPos(136, y);
		if (y == 14)
		{
			printf("��");
		}
		else if (y == 4)
		{
			printf("��");
		}
		else
		{
			printf("��");
		}
	}
	// ��-1
	for (x = 108; x <= 110; x += 2)
	{
		SetCurrentCursorPos(x, 4);
		printf("��");
	}
	// ��-2
	for (x = 132; x <= 134; x += 2)
	{
		SetCurrentCursorPos(x, 4);
		printf("��");
	}
	// �Ʒ�
	for (x = 108; x <= 134; x += 2)
	{
		SetCurrentCursorPos(x, 14);
		printf("��");
	}
	/////// ������ â
	// ��
	for (x = 111; x <= 131; x += 2)
	{
		SetCurrentCursorPos(x, 6);
		printf("��");
	}
	// ����
	for (y = 6; y <= 10; y++)
	{
		SetCurrentCursorPos(109, y);
		if (y == 10)
		{
			printf("��");
		}
		else if (y == 6)
		{
			printf("��");
		}
		else
		{
			printf("��");
		}
	}
	// ������
	for (y = 6; y <= 10; y++)
	{
		SetCurrentCursorPos(133, y);
		if (y == 10)
		{
			printf("��");
		}
		else if (y == 6)
		{
			printf("��");
		}
		else
		{
			printf("��");
		}
	}
	// �Ʒ�
	for (x = 111; x <= 131; x += 2)
	{
		SetCurrentCursorPos(x, 10);
		printf("��");
	}
	// �߰�-1
	for (y = 7; y <= 9; y++)
	{
		SetCurrentCursorPos(117, y);
		printf("��");
	}
	// �߰�-2
	for (y = 7; y <= 9; y++)
	{
		SetCurrentCursorPos(125, y);
		printf("��");
	}
	/////// �ð�â
	// ����
	for (y = 16; y <= 28; y++)
	{
		SetCurrentCursorPos(106, y);
		if (y == 28)
		{
			printf("��");
		}
		else if (y == 16)
		{
			printf("��");
		}
		else
		{
			printf("��");
		}
	}
	// ������
	for (y = 16; y <= 28; y++)
	{
		SetCurrentCursorPos(124, y);
		if (y == 28)
		{
			printf("��");
		}
		else if (y == 16)
		{
			printf("��");
		}
		else
		{
			printf("��");
		}
	}
	// ��
	for (x = 108; x <= 122; x += 2)
	{
		SetCurrentCursorPos(x, 16);
		printf("��");
	}
	// �Ʒ�
	for (x = 108; x <= 122; x += 2)
	{
		SetCurrentCursorPos(x, 28);
		printf("��");
	}
	/////// ��� ȹ�� ��Ȳ
	// ����
	for (y = 16; y <= 36; y++)
	{
		SetCurrentCursorPos(128, y);
		if (y == 36)
		{
			printf("��");
		}
		else if (y == 16)
		{
			printf("��");
		}
		else
		{
			printf("��");
		}
	}
	// ������
	for (y = 16; y <= 36; y++)
	{
		SetCurrentCursorPos(136, y);
		if (y == 36)
		{
			printf("��");
		}
		else if (y == 16)
		{
			printf("��");
		}
		else
		{
			printf("��");
		}
	}
	// ��
	for (x = 130; x <= 134; x += 2)
	{
		SetCurrentCursorPos(x, 16);
		printf("��");
	}
	//�߰�-1
	for (x = 130; x <= 134; x += 2)
	{
		SetCurrentCursorPos(x, 20);
		printf("��");
	}
	// �߰�-2
	for (x = 130; x <= 134; x += 2)
	{
		SetCurrentCursorPos(x, 24);
		printf("��");
	}
	// �߰�-3
	for (x = 130; x <= 134; x += 2)
	{
		SetCurrentCursorPos(x, 28);
		printf("��");
	}
	// �߰�-4
	for (x = 130; x <= 134; x += 2)
	{
		SetCurrentCursorPos(x, 32);
		printf("��");
	}
	// �Ʒ�
	for (x = 130; x <= 134; x += 2)
	{
		SetCurrentCursorPos(x, 36);
		printf("��");
	}
}

void Intro()
{
	int cnt = 1;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
	SetCurrentCursorPos(2, cnt++);
	printf("�̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢�");
	SetCurrentCursorPos(2, cnt++);
	printf("�̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	cnt = 6;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
	SetCurrentCursorPos(2, cnt++);
	printf("                             ��   ��   ��   ��   ��   ��   ��   ��   ��");
	cnt++;
	SetCurrentCursorPos(2, cnt++);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	printf("                               ��      ��     ��      ��    ������");
	SetCurrentCursorPos(2, cnt++);
	printf("                               ��      ��     ��      ��            ��");
	SetCurrentCursorPos(2, cnt++);
	printf("                              ���     ��     ������    ������");
	SetCurrentCursorPos(2, cnt++);
	printf("                             ��  ��    ��     ��      ��    ��");
	SetCurrentCursorPos(2, cnt++);
	printf("                            ��    ��   ��     ������    ������");
	SetCurrentCursorPos(2, cnt++);
	printf("                            ��    ��   ��     ");
	SetCurrentCursorPos(2, cnt++);
	printf("                                             �������  �������");
	SetCurrentCursorPos(2, cnt++);
	printf("                               ������         ��");
	SetCurrentCursorPos(2, cnt++);
	printf("                               ��      ��         ��        ������");
	SetCurrentCursorPos(2, cnt++);
	printf("                               ��      ��         ��        ��      ��");
	SetCurrentCursorPos(2, cnt++);
	printf("                               ������         ��        ������");
	cnt = 21;
	SetCurrentCursorPos(2, cnt++);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
	printf("                           �ââââ�   �ââ�   ��    ��  �âââ�   �â�");
	SetCurrentCursorPos(2, cnt++);
	printf("                               ��      ��    ��  ��    ��  ��     ��  �â�");
	SetCurrentCursorPos(2, cnt++);
	printf("                               ��      ��    ��  ��    ��  �âââ�    ��");
	SetCurrentCursorPos(2, cnt++);
	printf("                               ��      ��    ��  ��    ��  ��   �� ");
	SetCurrentCursorPos(2, cnt++);
	printf("                               ��       �ââ�    �ââ�   ��     ��   ��");

	cnt = 38;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 013);
	SetCurrentCursorPos(2, cnt++);
	printf("�̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�");
	SetCurrentCursorPos(2, cnt++);
	printf("�̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢�");

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

	SetCurrentCursorPos(36, 33);
	printf("  ����������START����������  ");
	Sleep(500);
	SetCurrentCursorPos(36, 33);
	printf("  ����������START����������  ");
	Sleep(500);
	SetCurrentCursorPos(36, 33);
	printf("  ����������START����������  ");
	Sleep(500);
	SetCurrentCursorPos(36, 33);
	printf("  ����������START����������  ");
	Sleep(500);
	SetCurrentCursorPos(36, 33);
	printf("  ����������START����������  ");
	RemoveCursor();
	/*
	while (1)
	{
		if (_kbhit() != 0)
		{
			return;
		}
		Sleep(100);
	}
	*/
	Sleep(500);
}

void Ending1()
{
	int x, y;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

	SetCurrentCursorPos(30, 3);
	printf("�����");

	SetCurrentCursorPos(46, 3);
	printf("��");

	SetCurrentCursorPos(36, 2);
	printf("������");

	for (y = 1; y <= 39; y++)
	{
		if (y >= 4 && y<8)
		{
			SetCurrentCursorPos(28, y);
			printf("��");
		}
		if (y == 8)
		{
			SetCurrentCursorPos(24, y);
			printf("����");
		}
		if (y>8 && y<13)
		{
			SetCurrentCursorPos(24, y);
			printf("��");
		}
		if (y == 13)
		{
			SetCurrentCursorPos(22, y);
			printf("���");
		}
		if (y>13 && y <= 19)
		{
			SetCurrentCursorPos(22, y);
			printf("��");
		}
		if (y >= 20 && y <= 21)
		{
			SetCurrentCursorPos(24, y);
			printf("��");
		}
		if (y >= 22 && y <= 23)
		{
			SetCurrentCursorPos(26, y);
			printf("��");
		}
		if (y == 24)
		{
			SetCurrentCursorPos(28, y);
			printf("��");
		}
	}

	SetCurrentCursorPos(62, 3);
	printf("�����");

	SetCurrentCursorPos(54, 2);
	printf("������");

	SetCurrentCursorPos(52, 3);
	printf("��");

	SetCurrentCursorPos(48, 4);
	printf("���");

	for (y = 1; y <= 39; y++)
	{
		if (y >= 4 && y<8)
		{
			SetCurrentCursorPos(70, y);
			printf("��");
		}
		if (y == 8)
		{
			SetCurrentCursorPos(24, y);
			printf("����");

			SetCurrentCursorPos(70, y);
			printf("����");
		}
		if (y>8 && y<13)
		{
			SetCurrentCursorPos(24, y);
			printf("��");
			SetCurrentCursorPos(74, y);
			printf("��");
		}
		if (y == 13)
		{
			SetCurrentCursorPos(22, y);
			printf("���");

			SetCurrentCursorPos(74, y);
			printf("���");
		}
		if (y>13 && y <= 19)
		{
			SetCurrentCursorPos(22, y);
			printf("��");
			SetCurrentCursorPos(76, y);
			printf("��");
		}
		if (y >= 20 && y <= 21)
		{
			SetCurrentCursorPos(24, y);
			printf("��");
			SetCurrentCursorPos(74, y);
			printf("��");
		}
		if (y >= 22 && y <= 23)
		{
			SetCurrentCursorPos(26, y);
			printf("��");
			SetCurrentCursorPos(72, y);
			printf("��");
		}
		if (y == 24)
		{
			SetCurrentCursorPos(28, y);
			printf("��");
			SetCurrentCursorPos(70, y);
			printf("��");
		}
	}
	//�Ӹ�ī�� ��
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	for (x = 30; x <= 68; x += 2)
	{
		SetCurrentCursorPos(x, 10);
		printf("��");
	}
	for (y = 10; y <= 27; y++)
	{
		SetCurrentCursorPos(30, y);
		printf("��");
		SetCurrentCursorPos(68, y);
		printf("��");
	}

	SetCurrentCursorPos(30, 27);
	printf("����");
	SetCurrentCursorPos(64, 27);
	printf("����");

	SetCurrentCursorPos(34, 28);
	printf("����");
	SetCurrentCursorPos(60, 28);
	printf("����");

	SetCurrentCursorPos(38, 29);
	printf("����");
	SetCurrentCursorPos(56, 29);
	printf("����");

	SetCurrentCursorPos(42, 30);
	printf("���������");
	//��
	/////////////////////////////////////////////�� ����,��
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	x = 36;
	y = 13;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2; y++;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2; y++;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2; y++;
	SetCurrentCursorPos(x, y);
	printf("��");
	x = 62;
	y = 13;
	SetCurrentCursorPos(x, y);
	printf("��");
	x -= 2; y++;
	SetCurrentCursorPos(x, y);
	printf("��");
	x -= 2; y++;
	SetCurrentCursorPos(x, y);
	printf("��");
	x -= 2; y++;
	SetCurrentCursorPos(x, y);
	printf("��");
	x -= 2; y++;
	x = 40;
	y = 17;
	SetCurrentCursorPos(x, y);
	printf("��");
	x = 58;
	y = 17;
	SetCurrentCursorPos(x, y);
	printf("��");
	////////////////////////////////////////////////��
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	for (y = 18; y <= 21; y++)
	{
		SetCurrentCursorPos(49, y);
		printf("��");
	}
	///////////////////////////////////////////////����
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	for (y = 20; y <= 22; y++)
	{
		SetCurrentCursorPos(34, y);
		printf("��");
	}

	SetCurrentCursorPos(36, 22);
	printf("��");

	for (y = 19; y <= 23; y++)
	{
		SetCurrentCursorPos(40, y);
		printf("��");
		if (y == 21)
		{
			SetCurrentCursorPos(42, y);
			printf("��");
		}
	}
	for (y = 20; y <= 22; y++)
	{
		SetCurrentCursorPos(58, y);
		printf("��");
	}

	SetCurrentCursorPos(56, 20);
	printf("���");

	for (y = 19; y <= 23; y++)
	{
		SetCurrentCursorPos(62, y);
		printf("��");
		if (y == 21)
		{
			SetCurrentCursorPos(64, y);
			printf("��");
		}
	}
	//////////////////////////////////////////////�Ժκ�
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);

	SetCurrentCursorPos(48, 24);
	printf("���");

	SetCurrentCursorPos(42, 27);
	printf("��");
	SetCurrentCursorPos(56, 27);
	printf("��");

	SetCurrentCursorPos(46, 25);
	printf("��");
	SetCurrentCursorPos(52, 25);
	printf("��");

	SetCurrentCursorPos(44, 26);
	printf("��");
	SetCurrentCursorPos(54, 26);
	printf("��");
	/////////////////////////////////////////////
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	for (y = 30; y <= 33; y++)
	{
		SetCurrentCursorPos(42, y);
		printf("��");
		SetCurrentCursorPos(56, y);
		printf("��");
	}
	for (x = 18; x <= 42; x += 2)
	{
		SetCurrentCursorPos(x, 34);
		printf("��");
		SetCurrentCursorPos(38 + x, 34);
		printf("��");
	}
	for (y = 35; y <= 39; y++)
	{
		SetCurrentCursorPos(18, y);
		printf("��");
	}//��, ���� ��,������ ���
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	for (y = 34; y >= 30; y--)
	{
		SetCurrentCursorPos(80, y);
		printf("��");
	}
	for (y = 25; y <= 29; y++)
	{
		SetCurrentCursorPos(78, y);
		printf("��");
		SetCurrentCursorPos(92, y);
		printf("��");
	}

	SetCurrentCursorPos(78, 25);
	printf("���������");

	SetCurrentCursorPos(80, 30);
	printf("�������");

	for (y = 30; y <= 39; y++)
	{
		SetCurrentCursorPos(90, y);
		printf("��");
	}
	for (y = 37; y <= 39; y++)
	{
		SetCurrentCursorPos(80, y);
		printf("��");
		if (y == 38)
		{
			SetCurrentCursorPos(78, y);
			printf("��");
		}
		if (y == 39)
		{
			SetCurrentCursorPos(76, y);
			printf("��");
		}
	} //��
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
	for (y = 24; y >= 14; y--)
	{
		SetCurrentCursorPos(90, y);
		printf("��");
		SetCurrentCursorPos(86, y);
		printf("��");
	}
	for (y = 14; y >= 4; y--)
	{
		SetCurrentCursorPos(96, y);
		printf("��");
		SetCurrentCursorPos(80, y);
		printf("��");
	}
	for (x = 80; x <= 96; x += 2)
	{
		SetCurrentCursorPos(x, 14);
		printf("��");
		SetCurrentCursorPos(x, 4);
		printf("��");
		if (x == 84 || x == 88 || x == 92)
		{
			SetCurrentCursorPos(x, 7);
			printf("��");
			SetCurrentCursorPos(x, 8);
			printf("��");
			SetCurrentCursorPos(x, 9);
			printf("��");
			SetCurrentCursorPos(x, 10);
			printf("��");
			SetCurrentCursorPos(x, 11);
			printf("��");
		}
	}//������
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	for (y = 6; y <= 15; y++)
	{
		if (y != 10 && y != 11)
		{
			SetCurrentCursorPos(10, y);
			printf("��");
			SetCurrentCursorPos(14, y);
			printf("��");
		}
	}
	for (x = 4; x <= 20; x += 2)
	{
		if (x != 12)
		{
			SetCurrentCursorPos(x, 9);
			printf("��");
			SetCurrentCursorPos(x, 12);
			printf("��");
		}
	}//����
}
void Ending2()
{
	int x, y;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

	SetCurrentCursorPos(30, 3);
	printf("�����");

	SetCurrentCursorPos(46, 3);
	printf("��");

	SetCurrentCursorPos(36, 2);
	printf("������");

	for (y = 1; y <= 39; y++)
	{
		if (y >= 4 && y<8)
		{
			SetCurrentCursorPos(28, y);
			printf("��");
		}
		if (y == 8)
		{
			for (x = 24; x <= 28; x += 2)
			{
				SetCurrentCursorPos(x, y);
				printf("��");
			}
		}
		if (y>8 && y<13)
		{
			SetCurrentCursorPos(24, y);
			printf("��");
		}
		if (y == 13)
		{
			SetCurrentCursorPos(22, y);
			printf("����");
		}
		if (y>13 && y <= 19)
		{
			SetCurrentCursorPos(22, y);
			printf("��");
		}
		if (y >= 20 && y <= 21)
		{
			SetCurrentCursorPos(24, y);
			printf("��");
		}
		if (y >= 22 && y <= 23)
		{
			SetCurrentCursorPos(26, y);
			printf("��");
		}
		if (y == 24)
		{
			SetCurrentCursorPos(28, y);
			printf("��");
		}
	}

	SetCurrentCursorPos(62, 3);
	printf("�����");

	SetCurrentCursorPos(54, 2);
	printf("������");

	SetCurrentCursorPos(52, 3);
	printf("��");

	SetCurrentCursorPos(48, 4);
	printf("���");

	for (y = 1; y <= 39; y++)
	{
		if (y >= 4 && y<8)
		{
			SetCurrentCursorPos(70, y);
			printf("��");
		}
		if (y == 8)
		{
			for (x = 24; x <= 28; x += 2)
			{
				SetCurrentCursorPos(x, y);
				printf("��");
			}

			SetCurrentCursorPos(70, y);
			printf("����");
		}
		if (y>8 && y<13)
		{
			SetCurrentCursorPos(24, y);
			printf("��");
			SetCurrentCursorPos(74, y);
			printf("��");
		}
		if (y == 13)
		{
			SetCurrentCursorPos(22, y);
			printf("���");

			SetCurrentCursorPos(74, y);
			printf("���");
		}
		if (y>13 && y <= 19)
		{
			SetCurrentCursorPos(22, y);
			printf("��");
			SetCurrentCursorPos(76, y);
			printf("��");
		}
		if (y >= 20 && y <= 21)
		{
			SetCurrentCursorPos(24, y);
			printf("��");
			SetCurrentCursorPos(74, y);
			printf("��");
		}
		if (y >= 22 && y <= 23)
		{
			SetCurrentCursorPos(26, y);
			printf("��");
			SetCurrentCursorPos(72, y);
			printf("��");
		}
		if (y == 24)
		{
			SetCurrentCursorPos(28, y);
			printf("��");
			SetCurrentCursorPos(70, y);
			printf("��");
		}
	}
	//�Ӹ�ī�� ��
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	for (x = 30; x <= 68; x += 2)
	{
		SetCurrentCursorPos(x, 10);
		printf("��");
	}
	for (y = 10; y <= 27; y++)
	{
		SetCurrentCursorPos(30, y);
		printf("��");
		SetCurrentCursorPos(68, y);
		printf("��");
	}

	SetCurrentCursorPos(30, 27);
	printf("����");
	SetCurrentCursorPos(64, 27);
	printf("����");

	SetCurrentCursorPos(34, 28);
	printf("����");
	SetCurrentCursorPos(60, 28);
	printf("����");

	SetCurrentCursorPos(38, 29);
	printf("����");
	SetCurrentCursorPos(56, 29);
	printf("����");

	SetCurrentCursorPos(42, 30);
	printf("���������");
	//��
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	for (y = 18; y <= 21; y++)
	{
		SetCurrentCursorPos(49, y);
		printf("��");
	}//��
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	for (y = 30; y <= 33; y++)
	{
		SetCurrentCursorPos(42, y);
		printf("��");
		SetCurrentCursorPos(56, y);
		printf("��");
	}
	for (x = 28; x <= 42; x += 2)
	{
		SetCurrentCursorPos(x, 34);
		printf("��");
		SetCurrentCursorPos(28 + x, 34);
		printf("��");
	}

	SetCurrentCursorPos(24, 33);
	printf("����");
	SetCurrentCursorPos(70, 33);
	printf("����");

	for (y = 38; y <= 39; y++)
	{
		SetCurrentCursorPos(24, y);
		printf("��");
		SetCurrentCursorPos(76, y);
		printf("��");
	}
	for (x = 20; x <= 24; x += 2)
	{
		SetCurrentCursorPos(x, 32);
		printf("��");
		SetCurrentCursorPos(x - 6, 37);
		printf("��");
		SetCurrentCursorPos(x - 2, 38);
		printf("��");
		SetCurrentCursorPos(54 + x, 32);
		printf("��");
		SetCurrentCursorPos(62 + x, 37);
		printf("��");
		SetCurrentCursorPos(58 + x, 38);
		printf("��");
	}
	for (y = 36; y >= 30; y--)
	{
		SetCurrentCursorPos(12, y);
		printf("��");
		SetCurrentCursorPos(88, y);
		printf("��");
	}
	for (y = 32; y >= 29; y--)
	{
		if (y != 29)
		{
			SetCurrentCursorPos(20, y);
			printf("��");
			SetCurrentCursorPos(80, y);
			printf("��");
		}
		if (y == 29)
		{
			SetCurrentCursorPos(18, y);
			printf("��");
			SetCurrentCursorPos(10, y);
			printf("��");
			SetCurrentCursorPos(82, y);
			printf("��");
			SetCurrentCursorPos(90, y);
			printf("��");
		}
	}
	for (y = 28; y >= 25; y--)
	{
		SetCurrentCursorPos(16, y);
		printf("��");
		SetCurrentCursorPos(8, y);
		printf("��");
		SetCurrentCursorPos(84, y);
		printf("��");
		SetCurrentCursorPos(92, y);
		printf("��");
	}
	for (y = 24; y >= 21; y--)
	{
		SetCurrentCursorPos(14, y);
		printf("��");
		SetCurrentCursorPos(6, y);
		printf("��");
		SetCurrentCursorPos(86, y);
		printf("��");
		SetCurrentCursorPos(94, y);
		printf("��");
	}
	for (y = 20; y >= 18; y--)
	{
		SetCurrentCursorPos(12, y);
		printf("��");
		SetCurrentCursorPos(4, y);
		printf("��");
		SetCurrentCursorPos(88, y);
		printf("��");
		SetCurrentCursorPos(96, y);
		printf("��");
	}
	for (x = 4; x <= 12; x += 2)
	{
		SetCurrentCursorPos(x, 17);
		printf("��");
		SetCurrentCursorPos(84 + x, 17);
		printf("��");
	}
	//��,��,��
	x = 36;
	y = 16;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y--;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y--;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y++;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y++;
	SetCurrentCursorPos(x, y);
	printf("��");
	x = 54; y = 16;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y--;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y--;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y++;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y++;
	SetCurrentCursorPos(x, y);
	printf("��");

	x = 40; y = 22;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y++;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y++;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y++;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y++;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y--;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y--;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y--;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y--;
	SetCurrentCursorPos(x, y);
	printf("��");
	//��
	///////////////////////
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	x = 2; y = 13;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y++;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y++;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y++;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y--;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y--;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y--;
	SetCurrentCursorPos(x, y);
	printf("��");
	////////////////////////
	x = 86;
	y = 13;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y++;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y++;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y++;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y--;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y--;
	SetCurrentCursorPos(x, y);
	printf("��");
	x += 2;
	y--;
	SetCurrentCursorPos(x, y);
	printf("��");
	for (y = 12; y <= 12; y++)
	{
		SetCurrentCursorPos(2, y);
		printf("��");
		SetCurrentCursorPos(14, y);
		printf("��");
		SetCurrentCursorPos(86, y);
		printf("��");
		SetCurrentCursorPos(98, y);
		printf("��");
	}
	for (x = 4; x <= 14; x += 2)
	{
		if (x == 4 || x == 6 || x == 10 || x == 12)
		{
			SetCurrentCursorPos(x, 11);
			printf("��");
		}
		if (x == 8)
		{
			SetCurrentCursorPos(x, 12);
			printf("��");
		}
	}
	for (x = 88; x <= 100; x += 2)
	{
		if (x == 88 || x == 90 || x == 94 || x == 96)
		{
			SetCurrentCursorPos(x, 11);
			printf("��");
		}
		if (x == 92)
		{
			SetCurrentCursorPos(x, 12);
			printf("��");
		}
	}
}

void GameRule()//���� ����� �����ִ� �Լ�
{
	SetCurrentCursorPos(40, 4);
	printf("������ ���� ��� ������");
	Sleep(500);

	SetCurrentCursorPos(4, 10);
	printf("1. ����Ű�� ���� ������ �� �־��! ��, ��Ȧ(��)�� �ε����� 10�ʵ��� ����Ű�� �ݴ�� �۵��մϴ�.");
	Sleep(500);

	SetCurrentCursorPos(4, 13);
	printf("2. ���� �ð�(99��)���� ȭ�� �������� �����Ǹ� �ܿ�� ������� �丮 ��Ḧ ȹ���� ������ ����!");
	Sleep(500);

	SetCurrentCursorPos(4, 16);
	printf("3. �������� ȹ���Ͽ� ����غ�����!");
	Sleep(500);

	SetCurrentCursorPos(7, 18);
	printf("- Ÿ�̸� ������(��): ���� 1 Ű�� ���� ��� ����, ���� �ð� +10��");
	SetCurrentCursorPos(7, 20);
	printf("- ���� ������ ������(��): ���� 2 Ű�� ���� ��� ����, ������ ������ 5�� ���� �ٽ� ������");
	SetCurrentCursorPos(7, 22);
	printf("- ���� ������(��): ���� 3 Ű�� ���� ��� ����, �Ÿ��� �ִ� ��� �غ��̵��� 10�� �� ����");
	SetCurrentCursorPos(7, 24);
	printf("- ��Ʈ ������(��): ȹ�� ��� �ڵ� ���, ��� +1��");
	Sleep(500);

	SetCurrentCursorPos(4, 27);
	printf("4. �غ��̵鿡 �ε����� ����� �پ��� ó�� ��ġ�� ���ư���..");
	Sleep(500);

	SetCurrentCursorPos(4, 30);
	printf("5. ��� 3���� ��� ������ ��ü ���������� ����ǰ�, ���� ������ ���� ������ �� �� �־��!");
	Sleep(500);

	SetCurrentCursorPos(4, 33);
	printf("6. ����� �̸��� �ϴܿ� �����ְ�, Ʋ�� ��Ḧ ������ ������ �پ��� �����սô�!");
	Sleep(500);

	SetCurrentCursorPos(40, 36);
	printf("=> ��, ���� ������ �ɺθ��� �Ϸ� ��� (@ '-'@)/~!");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	printf("��");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	SetCurrentCursorPos(40, 38);
	printf("   SPACEŰ�� ������ ������ ���۵˴ϴ�. . .");


	while (1)
	{
		if (_kbhit() != 0)
		{
			return;
		}
		Sleep(100);
	}
}

void SmallRule1()
{
	int y;
	
	SetCurrentCursorPos(12, 12);
	printf("* ������ Tip *");

	SetCurrentCursorPos(42, 8);
	printf("���� �������� 1 ����");
	SetCurrentCursorPos(20, 16);
	printf("1. ��ȣ���� �������� �� ���� �ǳʸ� �� ��!");

	SetCurrentCursorPos(64, 16);
	printf("��");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	SetCurrentCursorPos(66, 16);

	printf("��");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	SetCurrentCursorPos(68, 16);

	printf("��");

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	SetCurrentCursorPos(20, 19);
	printf("2. �Ķ����� �Ǹ� �¸� ��ΰ� �ö�Ÿ�� �ǳʾ� ��! (����Ű ��Ÿ)");

	SetCurrentCursorPos(86, 19);
	printf("��");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	SetCurrentCursorPos(88, 19);
	printf("��");

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	SetCurrentCursorPos(90, 19);
	printf("��");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
	SetCurrentCursorPos(24, 21);
	printf("����������");
	SetCurrentCursorPos(44, 21);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	printf("<-�������� �¸�");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
	SetCurrentCursorPos(24, 22);
	printf("����������");

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	SetCurrentCursorPos(10, 13);
	printf("��");
	SetCurrentCursorPos(12, 13);
	printf("����������������������������������������������������������������������������������");
	SetCurrentCursorPos(94, 13);
	printf("��");

	for (y = 14; y <= 24; y++)
	{
		SetCurrentCursorPos(94, y);
		if (y == 24)
			printf("��");
		else
			printf("��");
	}

	for (y = 14; y <= 24; y++)
	{
		SetCurrentCursorPos(10, y);
		if (y == 24)
			printf("��");
		else
			printf("��");
	}

	SetCurrentCursorPos(12, 24);
	printf("����������������������������������������������������������������������������������");
	SetCurrentCursorPos(16, 24);
	printf("��");
	SetCurrentCursorPos(16, 25);
	printf("��");


	SetCurrentCursorPos(14, 26);
	printf("������");
	SetCurrentCursorPos(13, 27);
	printf("��������");
	SetCurrentCursorPos(15, 28);
	printf(" ��");
}
void SmallRule2()
{
	int y;

	SetCurrentCursorPos(12, 12);
	printf("* ������ Tip *");

	SetCurrentCursorPos(42, 8);
	printf("���� �������� 2 ����");
	SetCurrentCursorPos(20, 16);
	printf("1. 10�ʸ��� �̷ΰ� �ٲ�ϱ� ���� ���� ������!");

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	SetCurrentCursorPos(20, 19);
	printf("2. ȣ���� �ȿ� ���� ��Ḧ �Ծ�� ��!");

	SetCurrentCursorPos(24, 21);
	printf("-> ȣ������ ���ö� ������ �� �����ϱ� ��Ȳ���� ����!");
	SetCurrentCursorPos(44, 21);

	SetCurrentCursorPos(10, 13);
	printf("��");
	SetCurrentCursorPos(12, 13);
	printf("����������������������������������������������������������������������������������");
	SetCurrentCursorPos(94, 13);
	printf("��");

	for (y = 14; y <= 24; y++)
	{
		SetCurrentCursorPos(94, y);
		if (y == 24)
			printf("��");
		else
			printf("��");
	}

	for (y = 14; y <= 24; y++)
	{
		SetCurrentCursorPos(10, y);
		if (y == 24)
			printf("��");
		else
			printf("��");
	}

	SetCurrentCursorPos(12, 24);
	printf("����������������������������������������������������������������������������������");
	SetCurrentCursorPos(16, 24);
	printf("��");
	SetCurrentCursorPos(16, 25);
	printf("��");

	SetCurrentCursorPos(14, 26);
	printf("������");
	SetCurrentCursorPos(13, 27);
	printf("��������");
	SetCurrentCursorPos(15, 28);
	printf(" ��");
}
void SmallRule3()
{
	int y;

	SetCurrentCursorPos(12, 12);
	printf("* ������ Tip *");

	SetCurrentCursorPos(42, 8);
	printf("���� �������� 3 ����");
	SetCurrentCursorPos(20, 17);
	printf("1. �̹����� �ð��� ������ �� ������ Ÿ�̸� ������(��)�� ���� Ȱ������!");

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	SetCurrentCursorPos(20, 20);
	printf("2. ���׶��(��)�� �̷���� �κ��� �ð��� ���� �����ž�~");



	SetCurrentCursorPos(10, 13);
	printf("��");
	SetCurrentCursorPos(12, 13);
	printf("����������������������������������������������������������������������������������");
	SetCurrentCursorPos(94, 13);
	printf("��");

	for (y = 14; y <= 24; y++)
	{
		SetCurrentCursorPos(94, y);
		if (y == 24)
			printf("��");
		else
			printf("��");
	}

	for (y = 14; y <= 24; y++)
	{
		SetCurrentCursorPos(10, y);
		if (y == 24)
			printf("��");
		else
			printf("��");
	}

	SetCurrentCursorPos(12, 24);
	printf("����������������������������������������������������������������������������������");
	SetCurrentCursorPos(16, 24);
	printf("��");
	SetCurrentCursorPos(16, 25);
	printf("��");

	SetCurrentCursorPos(14, 26);
	printf("������");
	SetCurrentCursorPos(13, 27);
	printf("��������");
	SetCurrentCursorPos(15, 28);
	printf(" ��");
}
void SmallRule4()
{
	int y;

	SetCurrentCursorPos(42, 8);
	printf("���� �������� 4 ����");

	SetCurrentCursorPos(12, 12);
	printf("* ������ Tip *");

	SetCurrentCursorPos(20, 16);
	printf("1. �̹��� �Ÿ��� �غ��̵��� �����ϱ� ���� ������(��)�� ���� Ȱ������!");

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	SetCurrentCursorPos(20, 18);
	printf("2. ��ȣ���� �ִ� ���ο����� �������� �Ǹ� �� ������ �غ��̵鸸 ����!");
	SetCurrentCursorPos(24, 20);
	printf("-> �� �� ���� ������(��)�� ��� �����ϱ� �� �Ծ����!");

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	SetCurrentCursorPos(10, 13);
	printf("��");
	SetCurrentCursorPos(12, 13);
	printf("����������������������������������������������������������������������������������");
	SetCurrentCursorPos(94, 13);
	printf("��");

	for (y = 14; y <= 24; y++)
	{
		SetCurrentCursorPos(94, y);
		if (y == 24)
			printf("��");
		else
			printf("��");
	}

	for (y = 14; y <= 24; y++)
	{
		SetCurrentCursorPos(10, y);
		if (y == 24)
			printf("��");
		else
			printf("��");
	}

	SetCurrentCursorPos(12, 24);
	printf("����������������������������������������������������������������������������������");
	SetCurrentCursorPos(16, 24);
	printf("��");
	SetCurrentCursorPos(16, 25);
	printf("��");

	SetCurrentCursorPos(14, 26);
	printf("������");
	SetCurrentCursorPos(13, 27);
	printf("��������");
	SetCurrentCursorPos(15, 28);
	printf(" ��");
}

void Drama()
{
	SetCurrentCursorPos(14, 7);
	printf("������");
	SetCurrentCursorPos(13, 8);
	printf("��������");
	SetCurrentCursorPos(15, 9);
	printf(" ��");
	Sleep(500);

	SetCurrentCursorPos(24, 8);
	printf("%s~ ������ �������� �ɺθ� �� �ٳ��", plname);
	Sleep(1500);

	SetCurrentCursorPos(14, 11);
	printf("������");
	SetCurrentCursorPos(13, 12);
	printf("��������");
	SetCurrentCursorPos(15, 13);
	printf(" ��");
	Sleep(500);

	SetCurrentCursorPos(24, 12);
	printf("�˰ھ�� ����");
	Sleep(1500);


	SetCurrentCursorPos(14, 15);
	printf("������");
	SetCurrentCursorPos(13, 16);
	printf("��������");
	SetCurrentCursorPos(15, 17);
	printf(" ��");
	Sleep(500);

	SetCurrentCursorPos(24, 16);
	printf("�ڵ���, ����, �����, �������, ��Ȧ, �� �����ϰ�");
	SetCurrentCursorPos(24, 18);
	printf("�� ��Ƽ� ���ƿ��Ŷ�...");
	Sleep(1500);

	SetCurrentCursorPos(14, 21);
	printf("������");
	SetCurrentCursorPos(13, 22);
	printf("��������");
	SetCurrentCursorPos(15, 23);
	printf(" ��");
	Sleep(500);

	SetCurrentCursorPos(24, 22);
	printf("�˰ھ�� ����.. ���� ���� �� �Ǵٱ���");
	Sleep(1500);


	SetCurrentCursorPos(14, 30);
	printf("������");
	SetCurrentCursorPos(13, 31);
	printf("��������");
	SetCurrentCursorPos(15, 32);
	printf(" ��");
	Sleep(500);

	SetCurrentCursorPos(24, 31);
	printf("���̻� �̷� ����� ���� �ʰھ�!");
	SetCurrentCursorPos(24, 33);
	printf("���� ȭ���� �ɺθ� �Ƿ��� �����ְ� ���׾�!");
	Sleep(2000);
}
void Drama2()
{
	SetCurrentCursorPos(14, 8);
	printf("������");
	SetCurrentCursorPos(13, 9);
	printf("��������");
	SetCurrentCursorPos(15, 10);
	printf(" ��");
	Sleep(500);

	SetCurrentCursorPos(24, 9);
	printf("��...�ʹ� ����߾�...");
	SetCurrentCursorPos(24, 11);
	printf("�ٳ�Խ��ϴ�~");
	Sleep(1500);

	SetCurrentCursorPos(14, 14);
	printf("������");
	SetCurrentCursorPos(13, 15);
	printf("��������");
	SetCurrentCursorPos(15, 16);
	printf(" ��");
	Sleep(500);

	SetCurrentCursorPos(24, 15);
	printf("�����߾� �츮 �ֱ� %s~", plname);
	SetCurrentCursorPos(24, 17);
	printf("�󸶳� �� �����Գ� ����?");
	Sleep(1500);

	SetCurrentCursorPos(14, 20);
	printf("������");
	SetCurrentCursorPos(13, 21);
	printf("��������");
	SetCurrentCursorPos(15, 22);
	printf(" ��");
	Sleep(500);

	SetCurrentCursorPos(24, 21);
	printf(".....����..��...");
	Sleep(1500);

	SetCurrentCursorPos(14, 30);
	printf("������");
	SetCurrentCursorPos(13, 31);
	printf("��������");
	SetCurrentCursorPos(15, 32);
	printf(" ��");
	Sleep(500);

	SetCurrentCursorPos(24, 31);
	printf(".................!!!!!!!!!!!!!!!!!!!");
	Sleep(2000);
}

void InputPlayer()
{
	SetCurrentCursorPos(38, 10);
	printf("������ ���� �̸��� ������ ������");
	SetCurrentCursorPos(44, 14);
	printf("�� �̸�: ");
	SetCurrentCursorPos(56, 14);
	scanf("%s", plname);
	getchar();
}
void Bottari_board()	// ������ �̸� �����ֱ� ���ؼ�!
{
	SetCurrentCursorPos(116, 4);
	printf("%s ������", plname);
}

void Item_board()	// ������ â ǥ��
{
	// ������1-Ÿ
	SetCurrentCursorPos(113, 7);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
	printf("��");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	SetCurrentCursorPos(113, 9);
	printf("X %d", timer_item);

	// ������2-��
	SetCurrentCursorPos(121, 7);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
	printf("��");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	SetCurrentCursorPos(121, 9);
	printf("X %d", paper_item);

	// ������3-���
	SetCurrentCursorPos(129, 7);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
	printf("��");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	SetCurrentCursorPos(129, 9);
	printf("X %d", card_item);
}
void Heart_board()	// ���� â ǥ��
{
	int i;

	//IsGameOver();
	SetCurrentCursorPos(108, 12);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	printf("����: ");
	SetCurrentCursorPos(114, 12);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	for (i = 0; i<3; i++)
		printf("�� ");
	SetCurrentCursorPos(114, 12);
	for (i = 0; i<heart; i++)
		printf("�� ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}
void Score_board()
{
	SetCurrentCursorPos(122, 12);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	printf(" �� %d%c �޼�!", sum_score, '%');
}

void IItem(int f)	// ������ ���ϰ� �׸��� ����
{
	if (f == 111)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
		printf("��");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	}
	else if (f == 222)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
		printf("��");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	}
	else if (f == 333)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
		printf("��");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	}
	else if (f == 444)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
		printf("��");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	}
}

void ScorePrint()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	SetCurrentCursorPos(108, 30); //-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10
	printf("                   ");
	SetCurrentCursorPos(108, 31);
	printf("                   ");
	SetCurrentCursorPos(108, 32);
	printf("                   ");
	SetCurrentCursorPos(108, 33);
	printf("                   ");
	SetCurrentCursorPos(108, 34);
	printf("                   ");
	SetCurrentCursorPos(108, 35);
	printf("                   ");
	//printf("%2d",score);3�ٴ�
	SetCurrentCursorPos(108, 30);
	switch (score)
	{
	case -10:
		printf("      ��   ����");
		SetCurrentCursorPos(108, 31);
		printf("    ���   ��  ��");
		SetCurrentCursorPos(108, 32);
		printf("���  ��   ��  ��");
		SetCurrentCursorPos(108, 33);
		printf("      ��   ��  ��");
		SetCurrentCursorPos(108, 34);
		printf("      ��   ��  ��");
		SetCurrentCursorPos(108, 35);
		printf("    ���� ����");
		break;
	case 10:
		printf("     ��   ����");
		SetCurrentCursorPos(108, 31);
		printf("   ���   ��  ��");
		SetCurrentCursorPos(108, 32);
		printf("     ��   ��  ��");
		SetCurrentCursorPos(108, 33);
		printf("     ��   ��  ��");
		SetCurrentCursorPos(108, 34);
		printf("     ��   ��  ��");
		SetCurrentCursorPos(108, 35);
		printf("   ���� ����");
		break;
	case -9:
		printf("      ������");
		SetCurrentCursorPos(108, 31);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 32);
		printf("���  ������");
		SetCurrentCursorPos(108, 33);
		printf("              ��");
		SetCurrentCursorPos(108, 34);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 35);
		printf("      ������");
		break;
	case 9:
		printf("      ������");
		SetCurrentCursorPos(108, 31);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 32);
		printf("      ������");
		SetCurrentCursorPos(108, 33);
		printf("              ��");
		SetCurrentCursorPos(108, 34);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 35);
		printf("      ������");
		break;
	case -8:
		printf("      ������");
		SetCurrentCursorPos(108, 31);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 32);
		printf("���  ������");
		SetCurrentCursorPos(108, 33);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 34);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 35);
		printf("      ������");
		break;
	case 8:
		printf("      ������");
		SetCurrentCursorPos(108, 31);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 32);
		printf("      ������");
		SetCurrentCursorPos(108, 33);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 34);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 35);
		printf("      ������");
		break;
	case -7:
		printf("      ������");
		SetCurrentCursorPos(108, 31);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 32);
		printf("���         ��");
		SetCurrentCursorPos(108, 33);
		printf("            ��");
		SetCurrentCursorPos(108, 34);
		printf("           ��");
		SetCurrentCursorPos(108, 35);
		printf("          ��");
		break;
	case 7:
		printf("      ������");
		SetCurrentCursorPos(108, 31);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 32);
		printf("             ��");
		SetCurrentCursorPos(108, 33);
		printf("            ��");
		SetCurrentCursorPos(108, 34);
		printf("           ��");
		SetCurrentCursorPos(108, 35);
		printf("          ��");
		break;
	case -6:
		printf("       ������");
		SetCurrentCursorPos(108, 31);
		printf("       ��      ��");
		SetCurrentCursorPos(108, 32);
		printf("���   ��");
		SetCurrentCursorPos(108, 33);
		printf("       ������");
		SetCurrentCursorPos(108, 34);
		printf("       ��      ��");
		SetCurrentCursorPos(108, 35);
		printf("       ������");
		break;
	case 6:
		printf("      ������");
		SetCurrentCursorPos(108, 31);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 32);
		printf("      ��");
		SetCurrentCursorPos(108, 33);
		printf("      ������");
		SetCurrentCursorPos(108, 34);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 35);
		printf("      ������");
		break;
	case -5:
		printf("      ������");
		SetCurrentCursorPos(108, 31);
		printf("      ��");
		SetCurrentCursorPos(108, 32);
		printf("���  ������");
		SetCurrentCursorPos(108, 33);
		printf("              ��");
		SetCurrentCursorPos(108, 34);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 35);
		printf("      ������");
		break;
	case 5:
		printf("      ������");
		SetCurrentCursorPos(108, 31);
		printf("      ��");
		SetCurrentCursorPos(108, 32);
		printf("      ������");
		SetCurrentCursorPos(108, 33);
		printf("              ��");
		SetCurrentCursorPos(108, 34);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 35);
		printf("      ������");
		break;
	case -4:
		printf("           ���");
		SetCurrentCursorPos(108, 31);
		printf("         ��  ��");
		SetCurrentCursorPos(108, 32);
		printf("���   ��    ��");
		SetCurrentCursorPos(108, 33);
		printf("     �������");
		SetCurrentCursorPos(108, 34);
		printf("             ��");
		SetCurrentCursorPos(108, 35);
		printf("           ����");
		break;
	case 4:
		printf("           ���");
		SetCurrentCursorPos(108, 31);
		printf("         ��  ��");
		SetCurrentCursorPos(108, 32);
		printf("       ��    ��");
		SetCurrentCursorPos(108, 33);
		printf("     �������");
		SetCurrentCursorPos(108, 34);
		printf("             ��");
		SetCurrentCursorPos(108, 35);
		printf("           ����");
		break;
	case -3:
		printf("      ������");
		SetCurrentCursorPos(108, 31);
		printf("              ��");
		SetCurrentCursorPos(108, 32);
		printf("���    �����");
		SetCurrentCursorPos(108, 33);
		printf("              ��");
		SetCurrentCursorPos(108, 34);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 35);
		printf("      ������");
		break;
	case 3:
		printf("      ������");
		SetCurrentCursorPos(108, 31);
		printf("              ��");
		SetCurrentCursorPos(108, 32);
		printf("        �����");
		SetCurrentCursorPos(108, 33);
		printf("              ��");
		SetCurrentCursorPos(108, 34);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 35);
		printf("      ������");
		break;
	case -2:
		printf("       ������");
		SetCurrentCursorPos(108, 31);
		printf("               ��");
		SetCurrentCursorPos(108, 32);
		printf("���   ������");
		SetCurrentCursorPos(108, 33);
		printf("       ��      ");
		SetCurrentCursorPos(108, 34);
		printf("       ��      ��");
		SetCurrentCursorPos(108, 35);
		printf("       ������");
		break;
	case 2:
		printf("      ������");
		SetCurrentCursorPos(108, 31);
		printf("              ��");
		SetCurrentCursorPos(108, 32);
		printf("      ������");
		SetCurrentCursorPos(108, 33);
		printf("      ��      ");
		SetCurrentCursorPos(108, 34);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 35);
		printf("      ������");
		break;
	case -1:
		printf("           ��");
		SetCurrentCursorPos(108, 31);
		printf("       ����");
		SetCurrentCursorPos(108, 32);
		printf("���       ��");
		SetCurrentCursorPos(108, 33);
		printf("           �� ");
		SetCurrentCursorPos(108, 34);
		printf("           �� ");
		SetCurrentCursorPos(108, 35);
		printf("       �����");
		break;
	case 1:
		printf("            ��");
		SetCurrentCursorPos(108, 31);
		printf("        ����");
		SetCurrentCursorPos(108, 32);
		printf("            ��");
		SetCurrentCursorPos(108, 33);
		printf("            �� ");
		SetCurrentCursorPos(108, 34);
		printf("            �� ");
		SetCurrentCursorPos(108, 35);
		printf("        �����");
		break;
	case 0:
		printf("      ������");
		SetCurrentCursorPos(108, 31);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 32);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 33);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 34);
		printf("      ��      ��");
		SetCurrentCursorPos(108, 35);
		printf("      ������");
		break;
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

void TicTocTicToc()
{
	int y;
	timelimit = (int)time(NULL) - startTime;
	if (second == ((99 - timelimit) % 10))
	{
		return;
	}
	second = (99 - timelimit) % 10; //���� �ڸ�

	for (y = 18; y<26; y++)
	{
		SetCurrentCursorPos(117, y);
		printf("      ");
	}

	SetCurrentCursorPos(117, 18);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	switch (second)
	{
	case 0:
		printf("����");
		SetCurrentCursorPos(117, 19);
		printf("��  ��");
		SetCurrentCursorPos(117, 20);
		printf("��  ��");
		SetCurrentCursorPos(117, 21);
		printf("��  ��");
		SetCurrentCursorPos(117, 22);
		printf("��  ��");
		SetCurrentCursorPos(117, 23);
		printf("��  ��");
		SetCurrentCursorPos(117, 24);
		printf("��  ��");
		SetCurrentCursorPos(117, 25);
		printf("����");
		break;
	case 1:
		printf("  ��  ");
		SetCurrentCursorPos(117, 19);
		printf("���");
		SetCurrentCursorPos(117, 20);
		printf("  ��");
		SetCurrentCursorPos(117, 21);
		printf("  ��");
		SetCurrentCursorPos(117, 22);
		printf("  ��");
		SetCurrentCursorPos(117, 23);
		printf("  ��");
		SetCurrentCursorPos(117, 24);
		printf("  ��");
		SetCurrentCursorPos(117, 25);
		printf("����");
		break;
	case 2:
		printf("����");
		SetCurrentCursorPos(117, 19);
		printf("    ��");
		SetCurrentCursorPos(117, 20);
		printf("    ��");
		SetCurrentCursorPos(117, 21);
		printf("����");
		SetCurrentCursorPos(117, 22);
		printf("��");
		SetCurrentCursorPos(117, 23);
		printf("��");
		SetCurrentCursorPos(117, 24);
		printf("��");
		SetCurrentCursorPos(117, 25);
		printf("����");
		break;
	case 3:
		printf("����");
		SetCurrentCursorPos(117, 19);
		printf("    ��");
		SetCurrentCursorPos(117, 20);
		printf("    ��");
		SetCurrentCursorPos(117, 21);
		printf("����");
		SetCurrentCursorPos(117, 22);
		printf("    ��");
		SetCurrentCursorPos(117, 23);
		printf("    ��");
		SetCurrentCursorPos(117, 24);
		printf("    ��");
		SetCurrentCursorPos(117, 25);
		printf("����");
		break;
	case 4:
		printf("��  ��");
		SetCurrentCursorPos(117, 19);
		printf("��  ��");
		SetCurrentCursorPos(117, 20);
		printf("��  ��");
		SetCurrentCursorPos(117, 21);
		printf("����");
		SetCurrentCursorPos(117, 22);
		printf("    ��");
		SetCurrentCursorPos(117, 23);
		printf("    ��");
		SetCurrentCursorPos(117, 24);
		printf("    ��");
		SetCurrentCursorPos(117, 25);
		printf("    ��");
		break;
	case 5:
		printf("����");
		SetCurrentCursorPos(117, 19);
		printf("��");
		SetCurrentCursorPos(117, 20);
		printf("��");
		SetCurrentCursorPos(117, 21);
		printf("����");
		SetCurrentCursorPos(117, 22);
		printf("    ��");
		SetCurrentCursorPos(117, 23);
		printf("    ��");
		SetCurrentCursorPos(117, 24);
		printf("    ��");
		SetCurrentCursorPos(117, 25);
		printf("����");
		break;
	case 6:
		printf("����");
		SetCurrentCursorPos(117, 19);
		printf("��");
		SetCurrentCursorPos(117, 20);
		printf("��");
		SetCurrentCursorPos(117, 21);
		printf("��");
		SetCurrentCursorPos(117, 22);
		printf("����");
		SetCurrentCursorPos(117, 23);
		printf("��  ��");
		SetCurrentCursorPos(117, 24);
		printf("��  ��");
		SetCurrentCursorPos(117, 25);
		printf("����");
		break;
	case 7:
		printf("����");
		SetCurrentCursorPos(117, 19);
		printf("    ��");
		SetCurrentCursorPos(117, 20);
		printf("    ��");
		SetCurrentCursorPos(117, 21);
		printf("    ��");
		SetCurrentCursorPos(117, 22);
		printf("    ��");
		SetCurrentCursorPos(117, 23);
		printf("    ��");
		SetCurrentCursorPos(117, 24);
		printf("    ��");
		SetCurrentCursorPos(117, 25);
		printf("    ��");
		break;
	case 8:
		printf("����");
		SetCurrentCursorPos(117, 19);
		printf("��  ��");
		SetCurrentCursorPos(117, 20);
		printf("��  ��");
		SetCurrentCursorPos(117, 21);
		printf("����");
		SetCurrentCursorPos(117, 22);
		printf("��  ��");
		SetCurrentCursorPos(117, 23);
		printf("��  ��");
		SetCurrentCursorPos(117, 24);
		printf("��  ��");
		SetCurrentCursorPos(117, 25);
		printf("����");
		break;
	case 9:
		printf("����");
		SetCurrentCursorPos(117, 19);
		printf("��  ��");
		SetCurrentCursorPos(117, 20);
		printf("��  ��");
		SetCurrentCursorPos(117, 21);
		printf("����");
		SetCurrentCursorPos(117, 22);
		printf("    ��");
		SetCurrentCursorPos(117, 23);
		printf("    ��");
		SetCurrentCursorPos(117, 24);
		printf("    ��");
		SetCurrentCursorPos(117, 25);
		printf("����");
		break;
	default:
		break;
	}

	if (first == (99 - (timelimit) / 10))
	{
		return;
	}
	first = (99 - timelimit) / 10; //���� �ڸ�

	for (y = 18; y<26; y++)
	{
		SetCurrentCursorPos(108, y);
		printf("      ");
	}
	SetCurrentCursorPos(108, 18);
	switch (first)
	{
	case 0:
		printf("����");
		SetCurrentCursorPos(108, 19);
		printf("��  ��");
		SetCurrentCursorPos(108, 20);
		printf("��  ��");
		SetCurrentCursorPos(108, 21);
		printf("��  ��");
		SetCurrentCursorPos(108, 22);
		printf("��  ��");
		SetCurrentCursorPos(108, 23);
		printf("��  ��");
		SetCurrentCursorPos(108, 24);
		printf("��  ��");
		SetCurrentCursorPos(108, 25);
		printf("����");
		break;
	case 1:
		printf("  ��  ");
		SetCurrentCursorPos(108, 19);
		printf("���");
		SetCurrentCursorPos(108, 20);
		printf("  ��");
		SetCurrentCursorPos(108, 21);
		printf("  ��");
		SetCurrentCursorPos(108, 22);
		printf("  ��");
		SetCurrentCursorPos(108, 23);
		printf("  ��");
		SetCurrentCursorPos(108, 24);
		printf("  ��");
		SetCurrentCursorPos(108, 25);
		printf("����");
		break;
	case 2:
		printf("����");
		SetCurrentCursorPos(108, 19);
		printf("    ��");
		SetCurrentCursorPos(108, 20);
		printf("    ��");
		SetCurrentCursorPos(108, 21);
		printf("����");
		SetCurrentCursorPos(108, 22);
		printf("��");
		SetCurrentCursorPos(108, 23);
		printf("��");
		SetCurrentCursorPos(108, 24);
		printf("��");
		SetCurrentCursorPos(108, 25);
		printf("����");
		break;
	case 3:
		printf("����");
		SetCurrentCursorPos(108, 19);
		printf("    ��");
		SetCurrentCursorPos(108, 20);
		printf("    ��");
		SetCurrentCursorPos(108, 21);
		printf("����");
		SetCurrentCursorPos(108, 22);
		printf("    ��");
		SetCurrentCursorPos(108, 23);
		printf("    ��");
		SetCurrentCursorPos(108, 24);
		printf("    ��");
		SetCurrentCursorPos(108, 25);
		printf("����");
		break;
	case 4:
		printf("��  ��");
		SetCurrentCursorPos(108, 19);
		printf("��  ��");
		SetCurrentCursorPos(108, 20);
		printf("��  ��");
		SetCurrentCursorPos(108, 21);
		printf("����");
		SetCurrentCursorPos(108, 22);
		printf("    ��");
		SetCurrentCursorPos(108, 23);
		printf("    ��");
		SetCurrentCursorPos(108, 24);
		printf("    ��");
		SetCurrentCursorPos(108, 25);
		printf("    ��");
		break;
	case 5:
		printf("����");
		SetCurrentCursorPos(108, 19);
		printf("��");
		SetCurrentCursorPos(108, 20);
		printf("��");
		SetCurrentCursorPos(108, 21);
		printf("����");
		SetCurrentCursorPos(108, 22);
		printf("    ��");
		SetCurrentCursorPos(108, 23);
		printf("    ��");
		SetCurrentCursorPos(108, 24);
		printf("    ��");
		SetCurrentCursorPos(108, 25);
		printf("����");
		break;
	case 6:
		printf("����");
		SetCurrentCursorPos(108, 19);
		printf("��");
		SetCurrentCursorPos(108, 20);
		printf("��");
		SetCurrentCursorPos(108, 21);
		printf("��");
		SetCurrentCursorPos(108, 22);
		printf("����");
		SetCurrentCursorPos(108, 23);
		printf("��  ��");
		SetCurrentCursorPos(108, 24);
		printf("��  ��");
		SetCurrentCursorPos(108, 25);
		printf("����");
		break;
	case 7:
		printf("����");
		SetCurrentCursorPos(108, 19);
		printf("    ��");
		SetCurrentCursorPos(108, 20);
		printf("    ��");
		SetCurrentCursorPos(108, 21);
		printf("    ��");
		SetCurrentCursorPos(108, 22);
		printf("    ��");
		SetCurrentCursorPos(108, 23);
		printf("    ��");
		SetCurrentCursorPos(108, 24);
		printf("    ��");
		SetCurrentCursorPos(108, 25);
		printf("    ��");
		break;
	case 8:
		printf("����");
		SetCurrentCursorPos(108, 19);
		printf("��  ��");
		SetCurrentCursorPos(108, 20);
		printf("��  ��");
		SetCurrentCursorPos(108, 21);
		printf("����");
		SetCurrentCursorPos(108, 22);
		printf("��  ��");
		SetCurrentCursorPos(108, 23);
		printf("��  ��");
		SetCurrentCursorPos(108, 24);
		printf("��  ��");
		SetCurrentCursorPos(108, 25);
		printf("����");
		break;
	case 9:
		printf("����");
		SetCurrentCursorPos(108, 19);
		printf("��  ��");
		SetCurrentCursorPos(108, 20);
		printf("��  ��");
		SetCurrentCursorPos(108, 21);
		printf("����");
		SetCurrentCursorPos(108, 22);
		printf("    ��");
		SetCurrentCursorPos(108, 23);
		printf("    ��");
		SetCurrentCursorPos(108, 24);
		printf("    ��");
		SetCurrentCursorPos(108, 25);
		printf("����");
		break;
	default:
		break;
	}
	IsGameOver();
}

void Recipe_Set2() //�����Ǹ� ��Ÿ���ִ� �Լ�
{
	int i, k = 1, j = 0;
	clock_t Start;
	RemoveCursor();

	Recipe_Set();
	NoRecipe_Set();

	srand((unsigned int)time(NULL));
	Start = clock();
	if (recipe_flag == 0) //���������� �Ѿ ��
	{
		id = stage - 1;
		//��Ȳ �ȳ� ȭ��
		SetCurrentCursorPos(113, 2);
		printf("                          ");
		SetCurrentCursorPos(113, 2);
		printf("STAGE %d - %s", stage, recipe[id].ch[5]);
		recipe_flag = 2;
	}
	//������ ����
	if (recipe_flag2 == 1)
		ShowRecipe();

	TimeBoardDelete();

	for (i = 0; i < 5; i++)
	{
		switch (stage)
		{
		case 1:
			curPos.X = 30 * 2;
			curPos.Y = 26;
			SetCurrentCursorPos(curPos.X, curPos.Y);
			printf("%s", recipe[id].ch[0]);
			gameBoardInfo[curPos.Y][curPos.X / 2] = 100;

			curPos.X = 3 * 2;
			curPos.Y = 15;
			SetCurrentCursorPos(curPos.X, curPos.Y);
			printf("%s", recipe[id].ch[1]);
			gameBoardInfo[curPos.Y][curPos.X / 2] = 101;

			curPos.X = 12 * 2;
			curPos.Y = 23;
			SetCurrentCursorPos(curPos.X, curPos.Y);
			printf("%s", recipe[id].ch[2]);
			gameBoardInfo[curPos.Y][curPos.X / 2] = 102;

			curPos.X = 48 * 2;
			curPos.Y = 15;
			SetCurrentCursorPos(curPos.X, curPos.Y);
			printf("%s", recipe[id].ch[3]);
			gameBoardInfo[curPos.Y][curPos.X / 2] = 103;

			curPos.X = 27 * 2;
			curPos.Y = 1;
			SetCurrentCursorPos(curPos.X, curPos.Y);
			printf("%s", recipe[id].ch[4]);
			gameBoardInfo[curPos.Y][curPos.X / 2] = 104;
			break;
		case 4:
			curPos.X = 15 * 2;
			curPos.Y = 2;
			SetCurrentCursorPos(curPos.X, curPos.Y);
			printf("%s", recipe[id].ch[0]);
			gameBoardInfo[curPos.Y][curPos.X / 2] = 100;

			curPos.X = 18 * 2;
			curPos.Y = 11;
			SetCurrentCursorPos(curPos.X, curPos.Y);
			printf("%s", recipe[id].ch[1]);
			gameBoardInfo[curPos.Y][curPos.X / 2] = 101;

			curPos.X = 34 * 2;
			curPos.Y = 19;
			SetCurrentCursorPos(curPos.X, curPos.Y);
			printf("%s", recipe[id].ch[2]);
			gameBoardInfo[curPos.Y][curPos.X / 2] = 102;

			curPos.X = 44 * 2;
			curPos.Y = 24;
			SetCurrentCursorPos(curPos.X, curPos.Y);
			printf("%s", recipe[id].ch[3]);
			gameBoardInfo[curPos.Y][curPos.X / 2] = 103;

			curPos.X = 44 * 2;
			curPos.Y = 38;
			SetCurrentCursorPos(curPos.X, curPos.Y);
			printf("%s", recipe[id].ch[4]);
			gameBoardInfo[curPos.Y][curPos.X / 2] = 104;
			break;
		case 3:
			//����
			curPos.X = 3 * 2;
			curPos.Y = 37;
			SetCurrentCursorPos(curPos.X, curPos.Y);
			printf("%s", recipe[id].ch[0]);
			gameBoardInfo[curPos.Y][curPos.X / 2] = 100;

			//���
			curPos.X = 8 * 2;
			curPos.Y = 5;
			SetCurrentCursorPos(curPos.X, curPos.Y);
			printf("%s", recipe[id].ch[1]);
			gameBoardInfo[curPos.Y][curPos.X / 2] = 101;
			
			curPos.X = 48 * 2;
			curPos.Y = 12;
			SetCurrentCursorPos(curPos.X, curPos.Y);
			printf("%s", recipe[id].ch[2]);
			gameBoardInfo[curPos.Y][curPos.X / 2] = 102;

			//���
			curPos.X = 48 * 2;
			curPos.Y = 31;
			SetCurrentCursorPos(curPos.X, curPos.Y);
			printf("%s", recipe[id].ch[3]);
			gameBoardInfo[curPos.Y][curPos.X / 2] = 103;

			//���⸧
			curPos.X = 48 * 2;
			curPos.Y = 37;
			SetCurrentCursorPos(curPos.X, curPos.Y);
			printf("%s", recipe[id].ch[4]);
			gameBoardInfo[curPos.Y][curPos.X / 2] = 104;
			break;
		case 2:
			curPos.X = 19 * 2;
			curPos.Y = 23;
			SetCurrentCursorPos(curPos.X, curPos.Y);
			printf("%s", recipe[id].ch[0]);
			gameBoardInfo[curPos.Y][curPos.X / 2] = 100;

			curPos.X = 28 * 2;
			curPos.Y = 32;
			SetCurrentCursorPos(curPos.X, curPos.Y);
			printf("%s", recipe[id].ch[1]);
			gameBoardInfo[curPos.Y][curPos.X / 2] = 101;

			curPos.X = 49 * 2;
			curPos.Y = 12;
			SetCurrentCursorPos(curPos.X, curPos.Y);
			printf("%s", recipe[id].ch[2]);
			gameBoardInfo[curPos.Y][curPos.X / 2] = 102;

			curPos.X = 23 * 2;
			curPos.Y = 2;
			SetCurrentCursorPos(curPos.X, curPos.Y);
			printf("%s", recipe[id].ch[3]);
			gameBoardInfo[curPos.Y][curPos.X / 2] = 103;

			curPos.X = 28 * 2;
			curPos.Y = 12;
			SetCurrentCursorPos(curPos.X, curPos.Y);
			printf("%s", recipe[id].ch[4]);
			gameBoardInfo[curPos.Y][curPos.X / 2] = 104;
			break;
		default:
			break;
		}
	}
	recipe_flag = 1;
}
void NoRecipe_Set2()
{
	if (stage == 4)
	{
		GetCurrentCursorPos();
		curPos.X = 16 * 2;
		curPos.Y = 2;
		SetCurrentCursorPos(curPos.X, curPos.Y);
		printf("%s", norecipe[3].ch[0]);
		gameBoardInfo[curPos.Y][curPos.X / 2] = -100;
		GetCurrentCursorPos();
		curPos.X = 20 * 2;
		curPos.Y = 15;
		SetCurrentCursorPos(curPos.X, curPos.Y);
		printf("%s", norecipe[3].ch[1]);
		gameBoardInfo[curPos.Y][curPos.X / 2] = -101;
		GetCurrentCursorPos();
		curPos.X = 36 * 2;
		curPos.Y = 34;
		SetCurrentCursorPos(curPos.X, curPos.Y);
		printf("%s", norecipe[3].ch[2]);
		gameBoardInfo[curPos.Y][curPos.X / 2] = -102;
	}
	if (stage == 3)
	{
		GetCurrentCursorPos();
		curPos.X = 30;
		curPos.Y = 31;
		SetCurrentCursorPos(curPos.X, curPos.Y);
		printf("%s", norecipe[2].ch[0]);
		gameBoardInfo[curPos.Y][curPos.X / 2] = -100;
		GetCurrentCursorPos();
		curPos.X = 80;
		curPos.Y = 31;
		SetCurrentCursorPos(curPos.X, curPos.Y);
		gameBoardInfo[curPos.Y][curPos.X / 2] = -101;
		printf("%s", norecipe[2].ch[1]);
	}
	if (stage == 2)
	{
		GetCurrentCursorPos();
		curPos.X = 98;
		curPos.Y = 13;
		SetCurrentCursorPos(curPos.X, curPos.Y);
		printf("%s", norecipe[1].ch[0]);
		gameBoardInfo[curPos.Y][curPos.X / 2] = -100;
	}
}

void finishStage()
{
	//FINISH
	int x, y;
	x = 20; y = 17;
	SetCurrentCursorPos(x, y++);
	printf("�����  �����  ��    ��  �����    ����  ��    ��  ���");
	SetCurrentCursorPos(x, y++);
	printf("��           ��     ���  ��     ��     ��        ��    ��  ���");
	SetCurrentCursorPos(x, y++);
	printf("�����     ��     �� �� ��     ��     �����  �����   ��");
	SetCurrentCursorPos(x, y++);
	printf("��           ��     ��  ���     ��           ��  ��    ��");
	SetCurrentCursorPos(x, y);
	printf("��        �����  ��    ��  �����  ����    ��    ��   ��");

	Sleep(1000);

	x = 20; y = 17;
	SetCurrentCursorPos(x, y++);
	printf("                                                                         ");
	SetCurrentCursorPos(x, y++);
	printf("                                                                         ");
	SetCurrentCursorPos(x, y++);
	printf("                                                                         ");
	SetCurrentCursorPos(x, y++);
	printf("                                                                         ");
	SetCurrentCursorPos(x, y);
	printf("                                                                         ");
}
//�١ڡ١ڡ١ڡ١ڡ١ڡ١� �׸��� �Լ�

void EatMyRecipe(int posX, int posY)//PC�� ���� ��Ḧ �迭�� �����ϴ� �Լ� -> �浹�˻��ؼ� true�� ������ �� ���� ������ 100�̻����� ���� -> �̻��̸� �� �Լ� ȣ��
{
	int eat = gameBoardInfo[posY][posX];

	if (stage == 4 && eat == 104) isEaten = 1;

	IsGameOver();

	if (eat >= 100 && eat <= 104) // 100, 101, 102, 103, 104������� Info�� ����
	{
		myrecipe.ch[Eatcnt] = recipe[id].ch[eat - 100];
		gameBoardInfo[posY][posX] = 0;

		if (Eatcnt == eat - 100)
		{
			sum_score += 2;
			Score_board();
			score += 2;
			ScorePrint();
		}
		else if (Eatcnt != eat - 100)
		{
			sum_score += 1;
			Score_board();
			score += 1;
			ScorePrint();
		}
		Eatcnt++;
	}
	else if (eat <= -100 && eat >= -104)//-100, -101, -102, -103, -104������� Info�� ����
	{
		myrecipe.ch[Eatcnt] = norecipe[id].ch[(eat*(-1)) - 100];
		gameBoardInfo[posY][posX] = 0;
		sum_score -= 2;
		Score_board();
		score -= 2;
		ScorePrint();
		Eatcnt++;
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	SetCurrentCursorPos(132, 18);
	printf("%s", myrecipe.ch[0]);

	SetCurrentCursorPos(132, 22);
	printf("%s", myrecipe.ch[1]);

	SetCurrentCursorPos(132, 26);
	printf("%s", myrecipe.ch[2]);

	SetCurrentCursorPos(132, 30);
	printf("%s", myrecipe.ch[3]);

	SetCurrentCursorPos(132, 34);
	printf("%s", myrecipe.ch[4]);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 012);

	return;
}

void ShowPC()//PC �����ִ� �Լ�
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 012);//PC ���� ���λ����� ����

	SetCurrentCursorPos(pc.X, pc.Y);
	gameBoardInfo[pc.Y][pc.X / 2] = 123;
	printf("��");
}
void DeletePC()
{
	SetCurrentCursorPos(pc.X, pc.Y);
	gameBoardInfo[pc.Y][pc.X / 2] = 0;
	printf(" ");
}

int DetectCollisionPC(int posX, int posY)//->PC �浹 ��
{
	int arrX = posX / 2;
	int arrY = posY;

	if (gameBoardInfo[arrY][arrX] == 0 || gameBoardInfo[arrY][arrX] == 123)
		return 1;
	else if (gameBoardInfo[arrY][arrX] == 1)//��ֹ� �浹 ��
		return 0;
	else if ((gameBoardInfo[arrY][arrX] >= 100 && gameBoardInfo[arrY][arrX] < 105) || (gameBoardInfo[arrY][arrX] <= -100 && gameBoardInfo[arrY][arrX] > -105))
	{
		EatMyRecipe(arrX, arrY);
		return 1;
	}
	else if (gameBoardInfo[arrY][arrX] == 111)	// ��Ʈ ������
	{
		gameBoardInfo[arrY][arrX] = 0;
		if (heart == 3)
		{
			return 1;
		}
		else
		{
			heart++;
			Heart_board();
			return 1;
		}
	}
	else if (gameBoardInfo[arrY][arrX] == 222)	// Ÿ�̸� ������
	{
		gameBoardInfo[arrY][arrX] = 0;
		timer_item++;
		Item_board();
		return 1;
	}
	else if (gameBoardInfo[arrY][arrX] == 333)	// ���������� ������
	{
		gameBoardInfo[arrY][arrX] = 0;
		paper_item++;
		Item_board();
		return 1;
	}
	else if (gameBoardInfo[arrY][arrX] == 444)	// ��ī ������
	{
		if (stage == 4)
			item_stop = 1;
		gameBoardInfo[arrY][arrX] = 0;
		card_item++;
		Item_board();
		return 1;
	}
	else if (gameBoardInfo[arrY][arrX] == 11)
	{
		return 1;
	}
	else if (gameBoardInfo[arrY][arrX] == 55)	// ��Ȧ �浹��
	{
		gameBoardInfo[arrY][arrX] = 0;

		hall_flag = 1;
		hall_first = first;
		hall_second = second;

		return 1;
	}
	else if (gameBoardInfo[arrY][arrX] == 78)// ���� �浹 ��
	{
		heart--;
		Heart_board();
		SetCurrentCursorPos(pc.X, pc.Y);
		DeletePC();
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
		SetCurrentCursorPos(pc.X, pc.Y);
		printf("��");
		Sleep(800);

		SetCurrentCursorPos(pc.X, pc.Y);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
		printf("��");
		Sleep(800);

		SetCurrentCursorPos(pc.X, pc.Y);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
		printf("��");
		Sleep(800);

		SetCurrentCursorPos(pc.X, pc.Y);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
		printf("��");
		Sleep(800);
		pc.X = 52;
		pc.Y = 38;

		SetCurrentCursorPos(pc.X, pc.Y);
		ShowPC();
		return 0;
	}
	return 0;
}

int BlockDown()//�Ʒ��� �̵�
{
	if (DetectCollisionPC(pc.X, pc.Y + 1) == 0)
		return 0;
	DeletePC();
	pc.Y += 1;
	ShowPC();

	return 1;
}
int BlockUp()//���� �̵�
{
	if (DetectCollisionPC(pc.X, pc.Y - 1) == 0)
		return 0;
	DeletePC();
	pc.Y -= 1;
	ShowPC();

	return 1;
}
int ShiftLeft()//�������� �̵�
{
	if (DetectCollisionPC(pc.X - 2, pc.Y) == 0)
		return 0;
	DeletePC();
	pc.X -= 2;
	ShowPC();

	return 1;
}
int ShiftRight()//���������� �̵�
{
	if (DetectCollisionPC(pc.X + 2, pc.Y) == 0)
		return 0;
	DeletePC();
	pc.X += 2;
	ShowPC();

	return 1;
}
//�١ڡ١ڡ١ڡ١ڡ١ڡ١� PC �̵� ���� �Լ�

void ShowNPC(char NPCModel[4][9], COORD npc, int npc_color)//npc �����ִ� �Լ�
{
	int x, y;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), npc_color);

	for (y = 0; y<4; y++)
	{
		for (x = 0; x<9; x++)
		{
			SetCurrentCursorPos(npc.X + (x * 2), npc.Y + y);

			if (NPCModel[y][x] == 1)
			{
				printf("��");
			}
		}
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	SetCurrentCursorPos(npc.X, npc.Y);
}
void DeleteNPC(char NPCModel[4][9], COORD npc)//npc ����� �Լ�
{
	int x = 0, y = 0;

	for (y = 0; y<4; y++)
	{
		for (x = 0; x < 9; x++)
		{
			if (NPCModel[y][x] == 1)
			{
				SetCurrentCursorPos(npc.X + (x * 2), npc.Y + y);
				printf("  ");
			}
		}
	}
	SetCurrentCursorPos(npc.X, npc.Y);
}

int DetectCollisionNPC(int posX, int posY, char NPCModel[4][9])//NPC�� PC�� �浹�� ��
{
	int arrX = posX / 2;
	int arrY = posY;
	int x, y;

	for (y = 0; y < 4; y++)
	{
		for (x = 0; x < 9; x++)
		{
			if (NPCModel[y][x] == 1 && gameBoardInfo[arrY + y][arrX + x] == 123)
			{
				SetCurrentCursorPos(pc.X, pc.Y);
				DeletePC();
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
				SetCurrentCursorPos(pc.X, pc.Y);
				printf("��");
				Sleep(800);

				SetCurrentCursorPos(pc.X, pc.Y);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
				printf("��");
				Sleep(800);

				SetCurrentCursorPos(pc.X, pc.Y);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
				printf("��");
				Sleep(800);

				SetCurrentCursorPos(pc.X, pc.Y);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
				printf("��");
				Sleep(800);

				switch (stage)
				{
				case 1:
					pc.X = 52;
					pc.Y = 38;
					break;
				case 3:
					pc.X = 38;
					pc.Y = 26;
					break;
				case 2:
					pc.X = 6;
					pc.Y = 27;
					break;
				case 4:
					pc.X = 94;
					pc.Y = 10;
					break;
				default:
					break;
				}
				SetCurrentCursorPos(pc.X, pc.Y);
				ShowPC();
				return 0;
			}
		}
	}
	return 1;
}

void AddBlockToBoard(COORD npc, int npc_id)//npc�� �������� �ڸ��� info�� �߰�
{
	int x, y, arrCurX, arrCurY;
	for (y = 0; y<4; y++)
	{
		for (x = 0; x<9; x++)
		{
			arrCurX = npc.X / 2;
			arrCurY = npc.Y;

			if (NPCModel[npc_id][y][x] == 1)
			{
				gameBoardInfo[arrCurY + y][arrCurX + x] = 1;
			}
		}
	}
}
void AddBlockToBoard2(COORD npc, int npc_id)//�ڿ����� npc�� �������� �ڸ��� info�� �߰�
{
	int x, y, arrCurX, arrCurY;
	for (y = 0; y<4; y++)
	{
		for (x = 0; x<9; x++)
		{
			arrCurX = npc.X / 2;
			arrCurY = npc.Y;

			if (NPCModel[npc_id][y][x] == 1)
			{
				gameBoardInfo[arrCurY + y][arrCurX + x] = 78;
			}
		}
	}
}
void DeleteBlockToBoard(COORD npc, int npc_id)//npc�� ������ �ڸ��� info�� ����
{
	int x, y, arrCurX, arrCurY;
	for (y = 0; y<4; y++)
	{
		for (x = 0; x<9; x++)
		{
			arrCurX = npc.X / 2;
			arrCurY = npc.Y;

			if (NPCModel[npc_id][y][x] == 1)
			{
				gameBoardInfo[arrCurY + y][arrCurX + x] = 0;
			}
		}
	}
}

void NPCSet1()
{
	npc[0].color = npc_color_1;
	npc[0].npc_id = 0;
	npc[0].nPos.X = 20;
	npc[0].nPos.Y = 29;
	npc[0].direction = 2;//right, ������ �̵�
	npc[0].v = 50;

	npc[1].color = npc_color_1;
	npc[1].npc_id = 0;
	npc[1].nPos.X = 90;
	npc[1].nPos.Y = 32;
	npc[1].direction = 1;//right, ������ �̵�
	npc[1].v = 50;


	npc[2].color = 4;
	npc[2].npc_id = 1;
	npc[2].nPos.X = 84;
	npc[2].nPos.Y = 10;
	npc[2].direction = 1;//left, ���� �̵�=>�̰� ������ �ٲ۴�.
	npc[2].v = 50;
}
void NPCSet2()
{
	npc[0].color = npc_color_1;
	npc[0].npc_id = 0;
	npc[0].nPos.X = 5; npc[0].nPos.Y = 4;
	npc[0].direction = 1;
	npc[0].v = 50;

	npc[1].color = 2;
	npc[1].npc_id = 0;
	npc[1].nPos.X = 88; npc[1].nPos.Y = 7;
	npc[1].direction = 2;
	npc[1].v = 50;

	npc[2].color = npc_color_3;
	npc[2].npc_id = 2;
	npc[2].nPos.X = 5; npc[2].nPos.Y = 13;
	npc[2].direction = 1;
	npc[2].v = 50;

	npc[3].color = npc_color_3;
	npc[3].npc_id = 2;
	npc[3].nPos.X = 88; npc[3].nPos.Y = 16;
	npc[3].direction = 2;
	npc[3].v = 50;

	npc[4].color = npc_color_1;
	npc[4].npc_id = 0;
	npc[4].nPos.X = 5; npc[4].nPos.Y = 25;
	npc[4].direction = 1;
	npc[4].v = 50;

	npc[5].color = npc_color_5;
	npc[5].npc_id = 2;
	npc[5].nPos.X = 38; npc[5].nPos.Y = 22;
	npc[5].direction = 2;
	npc[5].v = 50;

	npc[6].color = npc_color_3;
	npc[6].npc_id = 2;
	npc[6].nPos.X = 30; npc[6].nPos.Y = 25;
	npc[6].direction = 1;
	npc[6].v = 50;

	npc[7].color = npc_color_1;
	npc[7].npc_id = 0;
	npc[7].nPos.X = 18; npc[7].nPos.Y = 22;
	npc[7].direction = 2;
	npc[7].v = 50;

	npc[8].color = 4;
	npc[8].npc_id = 0;
	npc[8].nPos.X = 60; npc[8].nPos.Y = 25;
	npc[8].direction = 1;
	npc[8].v = 88;

	npc[9].color = npc_color_4;
	npc[9].npc_id = 3;
	npc[9].nPos.X = 88; npc[9].nPos.Y = 31;
	npc[9].direction = 2;
	npc[9].v = 88;

	npc[10].color = npc_color_4;
	npc[10].npc_id = 3;
	npc[10].nPos.X = 60; npc[10].nPos.Y = 31;
	npc[10].direction = 2;
	npc[10].v = 88;

	npc[11].color = npc_color_5;
	npc[11].npc_id = 1;
	npc[11].nPos.X = 5; npc[11].nPos.Y = 35;
	npc[11].direction = 1;
	npc[11].v = 88;

	npc[12].color = 3;
	npc[12].npc_id = 0;
	npc[12].nPos.X = 36; npc[12].nPos.Y = 35;
	npc[12].direction = 1;
	npc[12].v = 88;
}
void NPCSet3()
{
	npc[0].color = npc_color_1;
	npc[0].npc_id = 0;
	npc[0].nPos.X = 2; npc[0].nPos.Y = 18;
	npc[0].direction = 2;
	npc[0].v = 50;

	npc[1].color = npc_color_3;
	npc[1].npc_id = 2;
	npc[1].nPos.X = 88; npc[1].nPos.Y = 21;
	npc[1].direction = 1;
	npc[1].v = 50;

	npc[2].color = npc_color_5;
	npc[2].npc_id = 4;
	npc[2].nPos.X = 40; npc[2].nPos.Y = 1;
	npc[2].direction = 4;//down
	npc[2].v = 50;

	npc[3].color = npc_color_5;
	npc[3].npc_id = 4;
	npc[3].nPos.X = 46; npc[3].nPos.Y = 36;
	npc[3].direction = 3;//up
	npc[3].v = 50;

	npc[4].color = npc_color_5;
	npc[4].npc_id = 4;
	npc[4].nPos.X = 52; npc[4].nPos.Y = 36;
	npc[4].direction = 3;//up
	npc[4].v = 50;
}
void NPCSet4()
{
	npc[0].color = npc_color_1;
	npc[0].npc_id = 0;
	npc[0].nPos.X = 88; npc[0].nPos.Y = 4;
	npc[0].direction = 1;//up
	npc[0].v = 50;

	npc[1].color = npc_color_1;
	npc[1].npc_id = 0;
	npc[1].nPos.X = 2; npc[1].nPos.Y = 16;
	npc[1].direction = 2;//up
	npc[1].v = 50;
}

void NPCLeft(int npcid, int index)//npc ���� �̵�
{
	if (DetectCollisionNPC(npc[index].nPos.X - 2, npc[index].nPos.Y, NPCModel[npcid]) == 0)
	{
		heart--;
		Heart_board();
		IsGameOver();
		return;
	}
	DeleteBlockToBoard(npc[index].nPos, npc[index].npc_id);
	DeleteNPC(NPCModel[npcid], npc[index].nPos);
	npc[index].nPos.X -= 2;
	AddBlockToBoard(npc[index].nPos, npc[index].npc_id);
	ShowNPC(NPCModel[npcid], npc[index].nPos, npc[index].color);
	SetCurrentCursorPos(npc[index].nPos.X, npc[index].nPos.Y);
}
void NPCLeft2(int npcid, int index)//�ڿ����� npc ���� �̵�
{
	AddBlockToBoard2(npc[index].nPos, npc[index].npc_id);
	DeleteNPC(NPCModel[npcid], npc[index].nPos);

	npc[index].nPos.X -= 2;

	DeleteBlockToBoard(npc[index].nPos, npc[index].npc_id);
	ShowNPC(NPCModel[npcid], npc[index].nPos, npc[index].color);
	SetCurrentCursorPos(npc[index].nPos.X, npc[index].nPos.Y);
}
void NPCRight(int npcid, int index)//npc ������ �̵�
{
	if (DetectCollisionNPC(npc[index].nPos.X + 2, npc[index].nPos.Y, NPCModel[npcid]) == 0)
	{
		heart--;
		Heart_board();
		return;
	}
	DeleteBlockToBoard(npc[index].nPos, npc[index].npc_id);
	DeleteNPC(NPCModel[npcid], npc[index].nPos);
	npc[index].nPos.X += 2;
	AddBlockToBoard(npc[index].nPos, npc[index].npc_id);
	ShowNPC(NPCModel[npcid], npc[index].nPos, npc[index].color);
	SetCurrentCursorPos(npc[index].nPos.X, npc[index].nPos.Y);
}
void NPCUp(int npcid, int index)//npc ���� �̵�
{
	if (DetectCollisionNPC(npc[index].nPos.X, npc[index].nPos.Y - 1, NPCModel[npcid]) == 0)
	{
		heart--;
		Heart_board();
		return;
	}
	DeleteBlockToBoard(npc[index].nPos, npc[index].npc_id);
	DeleteNPC(NPCModel[npcid], npc[index].nPos);
	npc[index].nPos.Y -= 1;
	AddBlockToBoard(npc[index].nPos, npc[index].npc_id);
	ShowNPC(NPCModel[npcid], npc[index].nPos, npc[index].color);
	SetCurrentCursorPos(npc[index].nPos.X, npc[index].nPos.Y);
}
void NPCDown(int npcid, int index)//npc �Ʒ� �̵�
{
	if (DetectCollisionNPC(npc[index].nPos.X, npc[index].nPos.Y + 1, NPCModel[npcid]) == 0)
	{
		heart--;
		Heart_board();
		return;
	}
	DeleteBlockToBoard(npc[index].nPos, npc[index].npc_id);
	DeleteNPC(NPCModel[npcid], npc[index].nPos);
	npc[index].nPos.Y += 1;
	AddBlockToBoard(npc[index].nPos, npc[index].npc_id);
	ShowNPC(NPCModel[npcid], npc[index].nPos, npc[index].color);
	SetCurrentCursorPos(npc[index].nPos.X, npc[index].nPos.Y);
}

void NPCMove()//ProcessKeyInput �Լ��� ���� �� ��� npc ������
{
	int j;

	for (j = 0; npc[j].direction != 0; j++)//direction!=0 (npc ���� ����) �� ���
	{
		if (stop == 1 && ((first * 10) + second) >= temp_clock)
			continue;
		if (((first * 10) + second)<temp_clock)

		{
			stop = 0;
		}
		if (stage == 4 && shinho2 == 0)
			if (j >= 4 && j <= 8)
				continue;
		if (stage == 1 && j == 2 && shinho == 0)
			continue;

		if (npc[j].direction == 1)//direction=1 �̸� ���� �̵�
		{
			if (stage == 1)
			{
				if (j != 2)
				{
					if (npc[j].nPos.X <= 4)
					{
						DeleteBlockToBoard(npc[j].nPos, npc[j].npc_id);
						DeleteNPC(NPCModel[npc[j].npc_id], npc[j].nPos);
						npc[j].nPos.X = 84;
					}
					NPCLeft(npc[j].npc_id, j);
				}
				else
				{
					if (npc[j].nPos.X <= 4)
					{
						AddBlockToBoard2(npc[j].nPos, npc[j].npc_id);
						DeleteNPC(NPCModel[npc[j].npc_id], npc[j].nPos);
						npc[j].nPos.X = 84;
					}
					NPCLeft2(npc[j].npc_id, j);
				}
			}
			else
			{
				if (npc[j].nPos.X <= 4)
				{
					DeleteBlockToBoard(npc[j].nPos, npc[j].npc_id);
					DeleteNPC(NPCModel[npc[j].npc_id], npc[j].nPos);
					npc[j].nPos.X = 84;
				}
				NPCLeft(npc[j].npc_id, j);
			}
		}
		else if (npc[j].direction == 2)//direction=2 �̸� ������ �̵�
		{
			if (npc[j].npc_id == 1)
			{
				if (npc[j].nPos.X >= 84)
				{
					DeleteBlockToBoard(npc[j].nPos, npc[j].npc_id);
					DeleteNPC(NPCModel[npc[j].npc_id], npc[j].nPos);
					npc[j].nPos.X = 2;
				}
			}
			else if (npc[j].nPos.X >= 88)
			{
				DeleteBlockToBoard(npc[j].nPos, npc[j].npc_id);
				DeleteNPC(NPCModel[npc[j].npc_id], npc[j].nPos);
				npc[j].nPos.X = 2;
			}
			NPCRight(npc[j].npc_id, j);
		}
		else if (npc[j].direction == 3)//direction=3 �̸� �� �̵�
		{
			if (stage == 2)
			{
				if (npc[j].nPos.Y <= 1)
				{
					DeleteBlockToBoard(npc[j].nPos, npc[j].npc_id);
					DeleteNPC(NPCModel[npc[j].npc_id], npc[j].nPos);
					npc[j].nPos.Y = 15;
				}
				NPCUp(npc[j].npc_id, j);
			}
			else
			{
				if (npc[j].nPos.Y <= 1)
				{
					DeleteBlockToBoard(npc[j].nPos, npc[j].npc_id);
					DeleteNPC(NPCModel[npc[j].npc_id], npc[j].nPos);
					npc[j].nPos.Y = 36;
				}
				NPCUp(npc[j].npc_id, j);
			}
		}
		else if (npc[j].direction == 4)//direction=4 �� �Ʒ� �̵�
		{
			if (stage == 2)
			{
				if (npc[j].nPos.Y >= 13)
				{
					DeleteBlockToBoard(npc[j].nPos, npc[j].npc_id);
					DeleteNPC(NPCModel[npc[j].npc_id], npc[j].nPos);
					npc[j].nPos.Y = 1;
				}
				NPCDown(npc[j].npc_id, j);
			}
			else
			{
				if (npc[j].nPos.Y >= 35)
				{
					DeleteBlockToBoard(npc[j].nPos, npc[j].npc_id);
					DeleteNPC(NPCModel[npc[j].npc_id], npc[j].nPos);
					npc[j].nPos.Y = 1;
				}
				NPCDown(npc[j].npc_id, j);
			}
		}
	}
}
//�١ڡ١ڡ١ڡ١ڡ١ڡ١�NPC �̵� ���� �Լ�

void Item_Info(int f)	// gameBoardInfo�� �־��ִ� �Լ�
{
	switch (f)
	{
	case 111:	// ��Ʈ
		gameBoardInfo[curPos.Y][curPos.X / 2] = 111;
		break;
	case 222:	// Ÿ�̸�
		gameBoardInfo[curPos.Y][curPos.X / 2] = 222;
		break;
	case 333:	// ����������
		gameBoardInfo[curPos.Y][curPos.X / 2] = 333;
		break;
	case 444:	// ��ī
		gameBoardInfo[curPos.Y][curPos.X / 2] = 444;
		break;
	}
}
void ShowItem(int a)	// ������ ����
{
	srand((unsigned int)time(NULL));

	if (stage == 1)
	{
		curPos = GetCurrentCursorPos();
		curPos.X = 6;
		curPos.Y = 23;
		SetCurrentCursorPos(curPos.X, curPos.Y);
		IItem(111);
		Item_Info(111);

		curPos = GetCurrentCursorPos();
		curPos.X = 78;
		curPos.Y = 23;
		SetCurrentCursorPos(curPos.X, curPos.Y);
		IItem(222);
		Item_Info(222);

		curPos = GetCurrentCursorPos();
		curPos.X = 40;
		curPos.Y = 15;
		SetCurrentCursorPos(curPos.X, curPos.Y);
		IItem(333);
		Item_Info(333);

		curPos = GetCurrentCursorPos();
		curPos.X = 6;
		curPos.Y = 6;
		SetCurrentCursorPos(curPos.X, curPos.Y);
		IItem(444);
		Item_Info(444);
	}

	if (stage == 4)
	{
		curPos.X = 26 * 2;
		curPos.Y = 24;
		SetCurrentCursorPos(curPos.X, curPos.Y);
		IItem(111);
		Item_Info(111);

		curPos = GetCurrentCursorPos();
		curPos.X = 42 * 2;
		curPos.Y = 38;
		SetCurrentCursorPos(curPos.X, curPos.Y);
		IItem(222);
		Item_Info(222);

		curPos.X = 20 * 2;
		curPos.Y = 6;
		SetCurrentCursorPos(curPos.X, curPos.Y);
		IItem(333);
		Item_Info(333);
	}

	if (stage == 3)
	{
		curPos = GetCurrentCursorPos();
		curPos.X = 64;
		curPos.Y = 20;
		SetCurrentCursorPos(curPos.X, curPos.Y);
		IItem(222);
		Item_Info(222);
		curPos.X = 90;
		curPos.Y = 10;
		SetCurrentCursorPos(curPos.X, curPos.Y);
		IItem(333);
		Item_Info(333);
		curPos.X = 20;
		curPos.Y = 13;
		SetCurrentCursorPos(curPos.X, curPos.Y);
		IItem(111);
		Item_Info(111);
		curPos.X = 48;
		curPos.Y = 4;
		SetCurrentCursorPos(curPos.X, curPos.Y);
		IItem(444);
		Item_Info(444);
	}

	if (stage == 2)
	{
		curPos = GetCurrentCursorPos();
		curPos.X = 38;
		curPos.Y = 35;
		SetCurrentCursorPos(curPos.X, curPos.Y);
		IItem(222);
		Item_Info(222);
		curPos.X = 72;
		curPos.Y = 27;
		SetCurrentCursorPos(curPos.X, curPos.Y);
		IItem(333);
		Item_Info(333);
		curPos.X = 82;
		curPos.Y = 33;
		SetCurrentCursorPos(curPos.X, curPos.Y);
		IItem(111);
		Item_Info(111);
		curPos.X = 4;
		curPos.Y = 10;
		SetCurrentCursorPos(curPos.X, curPos.Y);
		IItem(444);
		Item_Info(444);
	}

	if (a == 666)
	{
		return;
	}
}
//�١ڡ١ڡ١ڡ١ڡ١ڡ١ھ����� ���� �Լ�

void ProcessKeyInput()//�Է� Ű�� ���� PC �̵�, ������ ���, npc �̵�
{
	int i = 0;
	int j = 0;
	int key;
	RemoveCursor();
	if (stage == 1)
		Show_sinho();
	if (stage == 4)
		Show_sinho2();

	for (i = 0; npc[i].direction != 0; i++)//������ npc �� ��ŭ ������, direction=0 (������) �̸� npc ������ ����
	{
		if (stage == 1 && i == 2)
			continue;
		if (stage == 4 && shinho2 == 0 && i >= 4 && i <= 8)
			continue;
		AddBlockToBoard(npc[i].nPos, npc[i].npc_id);
		ShowNPC(NPCModel[npc[i].npc_id], npc[i].nPos, npc[i].color);
	}

	for (i = 0; i < 20; i++)
	{
		if (flag_over == 1)
		{
			return;
		}

		if (hall_flag == 1)
		{
			if (first == hall_first - 1 && second == hall_second)
			{
				hall_flag = 0;
			}
		}

		//printInfo();
		NPCMove();

		TicTocTicToc();
		if (_kbhit() != 0)
		{
			key = _getch();
			switch (key)
			{
			case LEFT:
				if (hall_flag == 0)
				{
					ShiftLeft();
				}
				else
				{
					ShiftRight();
				}
				break;

			case RIGHT:
				if (hall_flag == 0)
				{
					ShiftRight();
				}
				else
				{
					ShiftLeft();
				}
				break;

			case UP:
				if (hall_flag == 0)
				{
					BlockUp();
				}
				else
				{
					BlockDown();
				}
				break;

			case DOWN:
				if (hall_flag == 0)
				{
					BlockDown();
				}
				else
				{
					BlockUp();
				}
				break;

			case '1': 
				if (timer_item != 0)
				{
					timer_item--;
					Item_board();

					if (first == 9)
					{
						startTime += 9 - second;
					}
					else
					{
						startTime += 10;
					}
				}
				break;
			
			case '2':
				if (paper_item != 0)
				{
					paper = 1;
					paper_item--;
					Item_board();

					ShowRecipe();
					paper = 0;
					TimeBoardDelete();
				}
				break;
			
			case '3':
				if (card_item != 0)
				{
					stop = 1;
					temp_clock = first * 10 + second;
					temp_clock -= 10;
					r_first = temp_clock / 10;
					r_second = temp_clock % 10;

					card_item--;
					Item_board();
					/*npc stop ���� ��������̴�*/
				}
				break;
			}
		}
		Sleep(50);//��ü �ӵ��� ���� �ؾ��� �� ���� npc �ӵ��� �ϴ� �� ������ ...�Ф�
	}
}

int main()
{
	int i, j;
	plname = (char *)malloc(20 * sizeof(char));

	// ���� ��ü Ʋ = ����: 140, ����: 40
	system("title �丮���� �ɺθ� ���� p(���ԣ�)q");
	system("mode con: cols=145 lines=45");   // ����, ����                   // �ܼ�â ũ��.. ���� ���� �ϱ����� �÷������ϴ�,,
	center();
	RemoveCursor();

	drawBoard();
	Item_board();	// ������ â ǥ��
	Heart_board();	// ���� â ǥ��
	Score_board(); // ��ü ����(sum_score) ǥ��

	for (i = 0; i<30; i++)
	{
		npc[i].direction = 0;//npc ���� �ʱ�ȭ
	}
	PlaySound(TEXT("drama.wav"), NULL, SND_ASYNC | SND_LOOP);
	
	Intro();
	GameMapDelete();

	InputPlayer();//�÷��̾� �̸� �Է� �޴� �Լ�
	GameMapDelete();

	Bottari_board();//�Էµ� �÷��̾� �̸��� �������� ����
	Sleep(500);
	GameMapDelete();

	GameRule();//���� ��� ����
	GameMapDelete();//���� â ��ü �����ִ� �Լ�
	
	Drama();//�ɺθ� �ϰԵǴ� ���丮 �����ֱ�
	GameMapDelete();
	
	for (stage = 1; stage < 5; stage++)
	{
		if (stage == 1)
			SmallRule1();
		if (stage == 2)
		{
			if(flag_finish == 1)
				finishStage();
			SmallRule2();
		}
		if (stage == 3)
		{
			if (flag_finish == 1)
				finishStage();
			SmallRule3();
		}
		if (stage == 4)
		{
			if (flag_finish == 1)
				finishStage();
			SmallRule4();
		}
		Sleep(7000);
		GameMapDelete();

		recipe_flag2 = 1;
		show_flag = 1;
		Eatcnt = 0; //PC�� ��Ḧ ���� ����
		recipe_flag = 0; //���������� ����� �� ����

		ScorePrint();

		makeInfoEmpty();
		Recipe_Set2();
		recipe_flag2 = 0;
		// ��ī: 3, 6, 9
		// ��Ʈ: 4, 6, 8
		switch (stage)
		{
		case 1:
			SetCurrentCursorPos(2, 41);
			printf("�� : ���� / ��: ��ȣ�� / �� : ���� / �� : �κ� / �� : ���̹���");
			pc.X = 52;
			pc.Y = 38;
			ShowMap1();
			ShowItem(333);
			NPCSet1();
			break;
		case 4:
			SetCurrentCursorPos(2, 41);
			printf("                                                                                                                                                                        ");
			SetCurrentCursorPos(2, 41);
			printf("�� : ������ / �� : �� / �� : ���⸧ / �� : ��ġ/ �� : �� / �� : ����Ķ��� / �� : �� / �� : �ñ�ġ");
			pc.X = 94;
			pc.Y = 10;
			ShowMap2();
			ShowRoad2();
			NoRecipe_Set2();
			ShowItem(111);
			ShowItem(222);
			ShowItem(333);
			NPCSet2();
			break;
		case 3:
			SetCurrentCursorPos(2, 41);
			printf("                                                                                                                                                                        ");
			SetCurrentCursorPos(2, 41);
			printf("�� : ���� / �� : ��� / �� : �� /�� : ��� / �� : �ᳪ�� / �� : �ñ�ġ / �� : ���⸧");
			pc.X = 38;
			//	pc.X = 46;
			pc.Y = 26;
			ShowMap3();
			ShowItem(111);
			ShowItem(222);
			ShowItem(333);
			ShowItem(444);	// ��ī
			NoRecipe_Set2();
			NPCSet3();
			break;

		case 2:
			SetCurrentCursorPos(2, 41);
			printf("                                                                                                                                                                        ");
			SetCurrentCursorPos(2, 41);
			printf("�� : �丶��ҽ� / �� : ġ�� / �� : ���İ�Ƽ�� / �� : ���� / �� : ������ / �� : ����");
			pc.X = 6;
			pc.Y = 27;
			ShowMap4();
			ShowItem(111);	// ��Ʈ
			NoRecipe_Set2();
			NPCSet4();
			break;
		default:
			break;
		}
		Recipe_Set2();

		startTime = (int)time(NULL);

		for (i = 0; i < 5; i++)
		{
			myrecipe.ch[i] = "";
		}

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		SetCurrentCursorPos(132, 18);
		printf("%s", myrecipe.ch[0]);

		SetCurrentCursorPos(132, 22);
		printf("%s", myrecipe.ch[1]);

		SetCurrentCursorPos(132, 26);
		printf("%s", myrecipe.ch[2]);

		SetCurrentCursorPos(132, 30);
		printf("%s", myrecipe.ch[3]);

		SetCurrentCursorPos(132, 34);
		printf("%s", myrecipe.ch[4]);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 012);

		SetCurrentCursorPos(pc.X, pc.Y);//�ʱ� ��ǥ�� Ŀ�� �̵�
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 012);//PC ���� ���λ����� ����

		ShowPC();

		while (1)
		{
			if (flag_over == 1)
			{
				flag_finish = 1;
				score = 0;

				SetCurrentCursorPos(132, 18);
				printf("   ");

				SetCurrentCursorPos(132, 22);
				printf("   ");

				SetCurrentCursorPos(132, 26);
				printf("   ");

				SetCurrentCursorPos(132, 30);
				printf("   ");

				SetCurrentCursorPos(132, 34);
				printf("   ");

				SetCurrentCursorPos(108, 18);
				printf("                ");
				SetCurrentCursorPos(108, 19);
				printf("                ");
				SetCurrentCursorPos(108, 20);
				printf("                ");
				SetCurrentCursorPos(108, 21);
				printf("                ");
				SetCurrentCursorPos(108, 22);
				printf("                ");
				SetCurrentCursorPos(108, 23);
				printf("                ");
				SetCurrentCursorPos(108, 24);
				printf("                ");
				SetCurrentCursorPos(108, 25);
				printf("                ");

				SetCurrentCursorPos(pc.X, pc.Y);//�ʱ� ��ǥ�� Ŀ�� �̵�

				flag_over = 0;
				//DeletePC();
				GameMapDelete();

				for (j = 0; j < 30; j++)
				{
					npc[j].direction = 0;//�� ���������� ���� �� ���� npc ������ �ʱ�ȭ
				}
				temp_clock=100;
				break;
			}
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

			ProcessKeyInput();
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			switch (stage)
			{
			case 1:
				ShowRoad1();
				break;
			case 4:
				break;
			case 3:
				Draw_circle();
				break;
			case 2:
				Draw_wave();
				if (second == 8 || second == 9) //���� �ڸ�
				{
					if (first % 2 == 1)
					{
						Delete_maze();
						maze1();
					}
					else
					{
						Delete_maze();
						maze2();
					}
					ShowPC();
				}
				break;
			}
			ShowPC();
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		}
	}
	PlaySound(NULL, NULL, SND_PURGE | SND_NOWAIT | SND_ASYNC);
	GameMapDelete();
	PlaySound(TEXT("drama2.wav"), NULL, SND_ASYNC | SND_LOOP);
	Drama2();
	PlaySound(NULL, NULL, SND_PURGE | SND_NOWAIT | SND_ASYNC);
	
	if (sum_score <= 20)
	{
		PlaySound(TEXT("����1.wav"), NULL, SND_ASYNC | SND_LOOP);
		GameMapDelete();
		Ending1();
		Sleep(10000);
	}
	else if (sum_score > 20)
	{
		PlaySound(TEXT("����2.wav"), NULL, SND_ASYNC | SND_LOOP);
		GameMapDelete();
		Ending2();
		Sleep(10000);
	}
	SetCurrentCursorPos(122, 41);
	return 0;
}