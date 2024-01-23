#include <graphics.h>		// 引用图形库头文件
#include <conio.h>
#include <iostream>
#include <mmsyscom.h>
#include<fstream>
#pragma comment(lib,"Winmm.lib")
int flagyinyue=0;
int fastmax = -100000;//最大优先度
int cut = 0;
int cunlema=0;//本次是否读取了数据
int nafang=0;//哪一方下棋（10为先手，-10为后手）
int cun1=-1, cun2, cun3, cun4;
int cut5;//(是否对方成眼)
int cut6;//(是否对方成眼)
int cut3;//(是否自己成眼)
int cut4;//(是否自己成眼)
int fast[81] = {};
int board[9][9] = {};//1为对方，-1为自己
bool dfsair[9][9] = {};//工具数组
int dfv[9][9] = {};
int in[9][9];
int cx[4] = { 1,0,0,-1 };
int cy[4] = { 0,1,-1,0 };
int cut2 = 0;
bool jud(int x, int y)//判断是不是自己眼
{
	int flat = 0;
	if (x == 0 || board[x - 1][y] == -1)
		flat++;
	if (x == 8 || board[x + 1][y] == -1)
		flat++;
	if (y == 0 || board[x][y - 1] == -1)
		flat++;
	if (y == 8 || board[x][y + 1] == -1)
		flat++;
	if (flat >= 3)
		return true;
	else return false;
}
bool inboard(int x, int y)//是否在棋盘内
{
	return (x <= 8 && x >= 0 && y >= 0 && y <= 8);
}
bool dfs_air(int fx, int fy)//是否有气
{
	dfv[fx][fy] = true;
	bool flag = false;
	for (int dir = 0; dir < 4; dir++)
	{
		int dx = fx + cx[dir], dy = fy + cy[dir];
		if (inboard(dx, dy))
		{
			if (board[dx][dy] == 0)
				flag = true;
			if (board[dx][dy] == board[fx][fy] && !dfv[dx][dy])
				if (dfs_air(dx, dy))
					flag = true;
		}
	}
	return flag;
}
bool judgeAvailable(int fx, int fy, int col)//是否能下
{
	if (board[fx][fy]) return false;
	board[fx][fy] = col;
	memset(dfv, 0, sizeof(dfv));
	if (!dfs_air(fx, fy))
	{
		board[fx][fy] = 0;
		return false;
	}
	for (int dir = 0; dir < 4; dir++)
	{
		int dx = fx + cx[dir], dy = fy + cy[dir];
		if (inboard(dx, dy))
		{
			if (board[dx][dy] && !dfv[dx][dy])
				if (!dfs_air(dx, dy))
				{
					board[fx][fy] = 0;
					return false;
				}
		}
	}
	board[fx][fy] = 0;
	return true;
}
bool jud2(int x, int y)//判断对手是否能下
{
	if (!inboard(x, y))
		return 0;
	else return judgeAvailable(x, y, 1);
}
int jud3(int x, int y)//判断自己能不能下（就是为了省事）
{
	if (!inboard(x, y))
		return 0;
	else return judgeAvailable(x, y, -1);
}
int xiaqi()
{
	int c = 0;//哪一行下棋
	int d = 0;//哪一列下气
	int avaliablelist[100] = {};
	memset(fast, 0, sizeof(fast));
	for (int i = 0; i < 9; i++)//判断自己能下的区域
		for (int j = 0; j < 9; j++)
			if (judgeAvailable(i, j, -1)==true)
			{
				avaliablelist[cut] = i * 9 + j;
				cut++;
			}
	for (int i = 0; i < cut; i++)
		if (avaliablelist[i] % 2 == 1)//对角线的优先度相对较大
		{
			fast[i] = 5;
			cut2++;
		}
	for (int i = 0; i < cut; i++)//计算优先度
	{
		c = avaliablelist[i] / 9;
		d = avaliablelist[i] % 9;
		if ((c == 8 && d == 8) || (c == 8 && d == 0) || (c == 0 && d == 8) || (c == 0 && d == 0))
			fast[i] -= 10;
		else if (c == 8 || d == 0 || d == 8 || c == 0)
			fast[i] += 10;
		for (int k = -1; k < 2; k++)
			for (int h = -1; h < 2; h++)
			{
				if (board[c + k][d + h] == -1)
					fast[i] -= 10;
			}
		if (board[c][d + 1] == 1)
			fast[i] += 10;
		if (board[c][d - 1] == 1)
			fast[i] += 10;
		if (board[c - 1][d] == 1)
			fast[i] += 10;
		if (board[c + 1][d] == 1)
			fast[i] += 10;
		if (jud(c, d) != 0)
			fast[i] -= 1000;
	}
	for (int i = 0; i < cut; i++)//计算优先度2(能否形成眼）
	{
		c = avaliablelist[i] / 9;
		d = avaliablelist[i] % 9;
		cut3 = jud2(c - 1, d) + jud2(c + 1, d) + jud2(c, d - 1) + jud2(c, d + 1);
		board[c][d] = -1;
		cut4 = jud2(c - 1, d) + jud2(c + 1, d) + jud2(c, d - 1) + jud2(c, d + 1);
		if (cut3 != cut4)
			fast[i] += 100;
		board[c][d] = 0;
	}
	for (int i = 0; i < cut; i++)//计算优先度3(对方是否能下）
	{
		c = avaliablelist[i] / 9;
		d = avaliablelist[i] % 9;
		if (judgeAvailable(c, d, 1) == 0)
			fast[i] -= 1000;
	}
	for (int i = 0; i < cut; i++)//计算优先度4(对角是否有自己的子）
	{
		c = avaliablelist[i] / 9;
		d = avaliablelist[i] % 9;
		if (inboard(c - 1, d - 1))
			if (board[c - 1][d - 1] == -1)
				fast[i] += 10;
		if (inboard(c + 1, d - 1))
			if (board[c + 1][d - 1] == -1)
				fast[i] += 10;
		if (inboard(c + 1, d + 1))
			if (board[c + 1][d + 1] == -1)
				fast[i] += 10;
		if (inboard(c - 1, d + 1))
			if (board[c - 1][d + 1] == -1)
				fast[i] += 10;
	}
	for (int i = 0; i < cut; i++)//计算优先度5(是否为对方成眼位置）
	{
		c = avaliablelist[i] / 9;
		d = avaliablelist[i] % 9;
		cut5 = jud3(c - 1, d) + jud3(c + 1, d) + jud3(c, d + 1) + jud3(c, d - 1);
		board[c][d] = 1;
		cut6 = jud3(c - 1, d) + jud3(c + 1, d) + jud3(c, d - 1) + jud3(c, d + 1);
		if (cut5 != cut6)
			fast[i] += 100;
		board[c][d] = 0;
	}
	for (int i = 0; i < cut; i++)
	{
		if (fast[i] > fastmax)
		{
			fastmax = fast[i];
			c = avaliablelist[i] / 9;
			d = avaliablelist[i] % 9;
		}
	}
	if (cut == 0)
		return -1;
	fastmax = -100000;
	cut = 0;
	cut2 = 0;
	return (d+c*9);
}
using namespace std;
int hanshu1();
int hanshu2();
int hanshu3();
int hanshu4();
int hanshu5();
int hanshu6();
int hanshu7();
int main()//菜单
{
	MOUSEMSG m;
	initgraph(640, 740);	// 创建绘图窗口，大小为 640x480 像素
	IMAGE p1, p2, p3, p4, p5, p6, p7;
	FlushMouseMsgBuffer();
	loadimage(&p2, "data/picture3.jpg", 640, 740);
	putimage(0, 0, &p2);
	loadimage(&p1, "data/picture2.jpg", 320, 320);
	putimage(0, 0, &p1);
	loadimage(&p3, "data/picture4.jpg", 160, 160);
	putimage(480, 0, &p3);
	loadimage(&p4, "data/picture7.jpg", 160, 160);
	putimage(480, 160, &p4);
	loadimage(&p5, "data/picture6.jpg", 160, 160);
	putimage(320, 0, &p5);
	loadimage(&p6, "data/picture5.jpeg", 160, 160);
	putimage(320, 160, &p6);
	settextcolor(BLACK);
	setbkmode(TRANSPARENT);
	settextstyle(80, 40, _T("华文新魏"));
	outtextxy(183,350,"不围棋");
	settextstyle(40, 20, _T("华文新魏"));
	outtextxy(100, 450, "新的游戏");
	settextstyle(40, 20, _T("华文新魏"));
	outtextxy(100, 550, "读取游戏");
	settextstyle(40, 20, _T("华文新魏"));
	outtextxy(340, 450, "查看说明");
	settextstyle(40, 20, _T("华文新魏"));
	outtextxy(340, 450, "查看说明");
	settextstyle(40, 20, _T("华文新魏"));
	outtextxy(340, 550, "退出游戏");
	settextstyle(40, 20, _T("华文新魏"));
	outtextxy(265, 630, "彩蛋");
	settextstyle(20, 10, _T("华文新魏"));
	outtextxy(400, 640, "关闭音乐");
	settextstyle(20, 10, _T("华文新魏"));
	outtextxy(510, 640, "开启音乐");
	setlinecolor(BLACK);
	roundrect(90, 440, 300, 500, 30, 30);
	roundrect(90, 540, 300, 600, 30, 30);
	roundrect(330, 440, 540, 500, 30, 30);
	roundrect(330, 540, 540, 600, 30, 30);
	roundrect(255, 620, 370, 680, 20, 20);
	roundrect(397, 635, 496, 665, 10, 10);
	roundrect(507, 635, 606, 665, 10, 10);
	mciSendString("open data/猪突猛进.mp3 alias mymusic", NULL, 0, NULL);
	mciSendString("open data/victory.mp3 alias mymusic4", NULL, 0, NULL);
	mciSendString("open data/defeat.mp3 alias mymusic2", NULL, 0, NULL);
	if(flagyinyue == 0)
		mciSendString("play mymusic repeat", NULL, 0, NULL);
	while (1) {
		m = GetMouseMsg();
		if (m.x >= 90 && m.x <= 300 && m.y >= 440 && m.y <= 500 && m.uMsg == WM_LBUTTONDOWN)
			hanshu4();
		else if (m.x >= 330 && m.x <= 540 && m.y >= 440 && m.y <= 500 && m.uMsg == WM_LBUTTONDOWN)
			hanshu2();
		else if (m.x >= 90 && m.x <= 300 && m.y >= 540 && m.y <= 600 && m.uMsg == WM_LBUTTONDOWN)
		{
			cunlema = 1;
			ifstream infile("data.txt");
			infile >> nafang;
			for (int i = 0; i < 9; i++)
				for (int j = 0; j < 9; j++)
					infile >> board[i][j];
			if (nafang == 10)
				hanshu1();
			else if (nafang == -10)
				hanshu6();
		}
		else if (m.x >= 397 && m.x <= 496 && m.y >= 635 && m.y <= 665 && m.uMsg == WM_LBUTTONDOWN)
		{
			mciSendString("stop mymusic", NULL, 0, NULL);
			flagyinyue = 1;
		}
		else if (m.x >= 507 && m.x <= 606 && m.y >= 635 && m.y <= 665 && m.uMsg == WM_LBUTTONDOWN)
		{
			mciSendString("play mymusic", NULL, 0, NULL);
			flagyinyue = 0;
		}
		else if (m.x >= 330 && m.x <= 540 && m.y >= 540 && m.y <= 600 && m.uMsg == WM_LBUTTONDOWN)
		{
			exit(0);			// 关闭绘图窗口
		}
		else if (m.x >= 255 && m.x <= 370 && m.y >= 620 && m.y <= 680 && m.uMsg == WM_LBUTTONDOWN)
			hanshu3();
		else continue;
	}		
	return 0;
}
int hanshu2()//说明
{
	setbkcolor(WHITE);
	cleardevice();
	IMAGE p;
	loadimage(&p, "data/picture8.png", 640, 540);
	putimage(0, 0, &p);
	settextstyle(40, 20, _T("华文新魏"));
	outtextxy(220, 590, "点击任意");
	settextstyle(40, 20, _T("华文新魏"));
	outtextxy(240, 640, "处继续");
	MOUSEMSG m;
	while (1) {
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN)
			return main();
	}
}
int hanshu3()//彩蛋
{
	MOUSEMSG m;
	setbkcolor(WHITE);
	cleardevice();
	IMAGE p1, p2;
	loadimage(&p1, "data/picture9.jpeg", 640, 640);
	putimage(0, 0, &p1);
	settextstyle(40, 20, _T("华文新魏"));
	outtextxy(225, 670, "返回菜单");
	roundrect(215, 660, 420, 720, 30, 30);
	while(1) {
		m = GetMouseMsg();
		if (m.x >= 215 && m.x <= 420 && m.y >= 660 && m.y <= 720 && m.uMsg == WM_LBUTTONDOWN)
			return main();
		else if(m.uMsg == WM_LBUTTONDOWN){
			setbkcolor(WHITE);
			cleardevice();
			IMAGE p1, p2;
			loadimage(&p1, "data/picture10.jpeg", 640, 640);
			putimage(0, 0, &p1);
			settextstyle(40, 20, _T("华文新魏"));
			outtextxy(225, 670, "返回菜单");
			roundrect(215, 660, 420, 720, 30, 30);
			while (1) {
				m = GetMouseMsg();
				if (m.x >= 215 && m.x <= 420 && m.y >= 660 && m.y <= 720 && m.uMsg == WM_LBUTTONDOWN)
					return main();
				else if (m.uMsg == WM_LBUTTONDOWN) {
					setbkcolor(WHITE);
					cleardevice();
					IMAGE p1;
					loadimage(&p1, "data/picture11.jpeg", 640, 640);
					putimage(0, 0, &p1);
					settextstyle(40, 20, _T("华文新魏"));
					outtextxy(225, 670, "返回菜单");
					roundrect(215, 660, 420, 720, 30, 30);
					while (1) {
						m = GetMouseMsg();
						if (m.x >= 215 && m.x <= 420 && m.y >= 660 && m.y <= 720 && m.uMsg == WM_LBUTTONDOWN)
							return main();
						else if (m.uMsg == WM_LBUTTONDOWN) {
							setbkcolor(WHITE);
							cleardevice();
							IMAGE p1;
							settextstyle(40, 20, _T("华文新魏"));
							outtextxy(250, 100, "别看了");
							settextstyle(40, 20, _T("华文新魏"));
							outtextxy(225, 200, "再看也不");
							settextstyle(40, 20, _T("华文新魏"));
							outtextxy(225, 300, "是你老婆");
							settextstyle(40, 20, _T("华文新魏"));
							outtextxy(225, 400, "都是我的");
							settextstyle(40, 20, _T("华文新魏"));
							outtextxy(225, 670, "返回菜单");
							roundrect(215, 660, 420, 720, 30, 30);
							while (1) {
								m = GetMouseMsg();
								if (m.x >= 215 && m.x <= 420 && m.y >= 660 && m.y <= 720 && m.uMsg == WM_LBUTTONDOWN)
									return main();
							}
						}
					}
				}
			}
		}
	}
}
int hanshu1()//主程序（下棋）（黑子）（先手）
{
	int xialema = 0;
	cleardevice();
	IMAGE p, p2;
	loadimage(&p2, "data/picture3.jpg", 640, 740);
	putimage(0, 0, &p2);
	loadimage(&p, "data/picture1.png", 640, 640);
	putimage(0, 0, &p);
	settextstyle(40, 20, _T("华文新魏"));
	outtextxy(20, 670, "保存进度");
	roundrect(17, 665, 210, 715, 10, 10);
	settextstyle(40, 20, _T("华文新魏"));
	outtextxy(230, 670, "容我三思");
	roundrect(229, 665, 420, 715, 10, 10);
	settextstyle(40, 20, _T("华文新魏"));
	outtextxy(440, 670, "回到菜单");
	roundrect(439, 665, 628, 715, 10, 10);
	setfillcolor(WHITE);
	MOUSEMSG m;
	if(cunlema==1)
		for (int i = 0; i < 9; i++)
			for (int j = 0; j < 9; j++)
			{
				if (board[i][j] == 1)
				{
					setfillcolor(BLACK);
					fillcircle(51 + j * 67, 51 + i * 67, 25);
				}
				else if (board[i][j] == -1)
				{
					setfillcolor(WHITE);
					fillcircle(51 + j * 67, 51 + i * 67, 25);
				}
			}
	for (;;)
	{
		while (1) {
			m = GetMouseMsg();
			if (m.x >= 439 && m.x <= 628 && m.y >= 665 && m.y <= 715 && m.uMsg == WM_LBUTTONDOWN)
			{
				cut2 = 0;
				memset(fast, 0, sizeof(fast));
				memset(board, 0, sizeof(board));
				memset(dfv, 0, sizeof(dfv));
				fastmax = -100000;
				cut = 0;
				return main();
			}
			else if (m.x >= 229 && m.x <= 420 && m.y >= 665 && m.y <= 715 && m.uMsg == WM_LBUTTONDOWN && cun1 != -1)
			{
				mciSendString("open data/砸瓦鲁多.mp3 alias mymusic3", NULL, 0, NULL);
				mciSendString("play mymusic3", NULL, 0, NULL);
				board[cun1][cun2] = 0;
				board[cun3][cun4] = 0;
				putimage(0, 0, &p);
				for(int i=0;i<9;i++)
					for (int j = 0; j < 9; j++)
					{
						if (board[i][j] == 1)
						{
							setfillcolor(BLACK);
							fillcircle(51 + j * 67, 51 + i * 67, 25);
						}
						else if (board[i][j] == -1)
						{
							setfillcolor(WHITE);
							fillcircle(51 + j * 67, 51 + i * 67, 25);
						}
					}
			}
			else if (m.x >= 17 && m.x <= 210 && m.y >= 665 && m.y <= 715 && m.uMsg == WM_LBUTTONDOWN)
			{

				ofstream outfile("data.txt");
				outfile << nafang << ' ';
				for (int i = 0; i < 9; i++)
					for (int j = 0; j < 9; j++)
						outfile << board[i][j] << ' ';
				settextstyle(80, 40, _T("华文新魏"));
				outtextxy(140, 260, "保存成功");
				MOUSEMSG m1;
				while (1) {
					m1 = GetMouseMsg();
					if (m1.uMsg == WM_LBUTTONDOWN)
					{
						putimage(0, 0, &p);
						for (int i = 0; i < 9; i++)
							for (int j = 0; j < 9; j++)
							{
								if (board[i][j] == 1)
								{
									setfillcolor(BLACK);
									fillcircle(51 + j * 67, 51 + i * 67, 25);
								}
								else if (board[i][j] == -1)
								{
									setfillcolor(WHITE);
									fillcircle(51 + j * 67, 51 + i * 67, 25);
								}
							}
						break;
					}
				}
			}
			else if(m.uMsg == WM_LBUTTONDOWN){
				for (int i = 0; i < 9; i++)
					for (int j = 0; j < 9; j++)
						if (m.x >= (51 + j * 67 - 33) && m.x <= (51 + 67 * j + 33) && m.y >= (51 + i * 67 - 33) && m.y <= (51 + 67 * i + 33) && m.uMsg == WM_LBUTTONDOWN && board[i][j] == 0)
						{
							mciSendString("close mymusic3", NULL, 0, NULL);
							if (judgeAvailable(i, j, 1) == false)
								return hanshu5();
							board[i][j] = 1;
							cun1 = i;
							cun2 = j;
							xialema = 1;
							setfillcolor(BLACK);
							fillcircle(51 + j * 67, 51 + i * 67, 25);
							FlushMouseMsgBuffer();
							break;
						}
						else continue;
				if (xialema == 1)
				{
					int xianali = xiaqi();
					if (xianali == -1)
						hanshu7();
					int x = xianali / 9;
					int y = xianali % 9;
					cun3 = x;
					cun4 = y;
					board[x][y] = -1;
					xialema = 0;
					setfillcolor(WHITE);
					fillcircle(51 + y * 67, 51 + x * 67, 25);
				}
			}
		}
	}
}
int hanshu4()//选择棋子
{
	MOUSEMSG m;
	cleardevice();
	IMAGE p2;
	loadimage(&p2, "data/picture3.jpg", 640, 740);
	putimage(0, 0, &p2);
	settextstyle(80, 40, _T("华文新魏"));
	outtextxy(140, 200, "您执白子");
	roundrect(130, 195, 510, 285, 30, 30);
	settextstyle(80, 40, _T("华文新魏"));
	outtextxy(140, 440, "您执黑子");
	roundrect(130, 435, 510, 525, 30, 30);
	while (1) {
		m = GetMouseMsg();
		if (m.x >= 130 && m.x <= 510 && m.y >= 435 && m.y <= 525 && m.uMsg == WM_LBUTTONDOWN)
		{
			nafang = 10;
			hanshu1();
		}
		else if (m.x >= 130 && m.x <= 510 && m.y >= 195 && m.y <= 285 && m.uMsg == WM_LBUTTONDOWN)
		{
			nafang = -10;
			hanshu6();
		}
	}
}
int hanshu5()//死亡特效
{
	memset(fast, 0, sizeof(fast));
	memset(board, 0, sizeof(board));
	MOUSEMSG m;
	cleardevice();
	IMAGE p2, p3;
	loadimage(&p2, "data/picture3.jpg", 640, 740);
	putimage(0, 0, &p2);
	loadimage(&p3, "data/picture12.jpeg", 640, 640);
	putimage(0, 0, &p3);
	settextstyle(40, 20, _T("华文新魏"));
	outtextxy(225, 670, "返回菜单");
	roundrect(215, 660, 420, 720, 30, 30);
	mciSendString("stop mymusic", NULL, 0, NULL);
	mciSendString("play mymusic2", NULL, 0, NULL);
	cut2 = 0;
	memset(fast, 0, sizeof(fast));
	memset(board, 0, sizeof(board));
	memset(dfv, 0, sizeof(dfv));
	fastmax = -100000;
	cut = 0;
	while (1) {
		m = GetMouseMsg();
		if (m.x >= 215 && m.x <= 420 && m.y >= 660 && m.y <= 720 && m.uMsg == WM_LBUTTONDOWN)
		{
			mciSendString("close mymusic2", NULL, 0, NULL);
			return main();
		}
	}
}
int hanshu6()//主程序（下棋）（白子）（后手）
{
	int xialema = 0;
	cleardevice();
	IMAGE p, p2;
	loadimage(&p2, "data/picture3.jpg", 640, 740);
	putimage(0, 0, &p2);
	loadimage(&p, "data/picture1.png", 640, 640);
	putimage(0, 0, &p);
	settextstyle(40, 20, _T("华文新魏"));
	outtextxy(20, 670, "保存进度");
	roundrect(17, 665, 210, 715, 10, 10);
	settextstyle(40, 20, _T("华文新魏"));
	outtextxy(230, 670, "容我三思");
	roundrect(229, 665, 420, 715, 10, 10);
	settextstyle(40, 20, _T("华文新魏"));
	outtextxy(440, 670, "回到菜单");
	roundrect(439, 665, 628, 715, 10, 10);
	setfillcolor(BLACK);
	MOUSEMSG m;
	if (cunlema == 1)
	{
		for (int i = 0; i < 9; i++)
			for (int j = 0; j < 9; j++)
			{
				if (board[i][j] == 1)
				{
					setfillcolor(WHITE);
					fillcircle(51 + j * 67, 51 + i * 67, 25);
				}
				else if (board[i][j] == -1)
				{
					setfillcolor(BLACK);
					fillcircle(51 + j * 67, 51 + i * 67, 25);
				}
			}
		for (;;)
		{
			while (1) {
				m = GetMouseMsg();
				if (m.x >= 439 && m.x <= 628 && m.y >= 665 && m.y <= 715 && m.uMsg == WM_LBUTTONDOWN)
				{
					cut2 = 0;
					memset(fast, 0, sizeof(fast));
					memset(board, 0, sizeof(board));
					memset(dfv, 0, sizeof(dfv));
					fastmax = -100000;
					cut = 0;
					return main();
				}
				else if (m.x >= 229 && m.x <= 420 && m.y >= 665 && m.y <= 715 && m.uMsg == WM_LBUTTONDOWN && cun1 != -1)
				{
					board[cun1][cun2] = 0;
					board[cun3][cun4] = 0;
					putimage(0, 0, &p);
					for (int i = 0; i < 9; i++)
						for (int j = 0; j < 9; j++)
						{
							if (board[i][j] == 1)
							{
								setfillcolor(WHITE);
								fillcircle(51 + j * 67, 51 + i * 67, 25);
							}
							else if (board[i][j] == -1)
							{
								setfillcolor(BLACK);
								fillcircle(51 + j * 67, 51 + i * 67, 25);
							}
						}
				}
				else if (m.x >= 17 && m.x <= 210 && m.y >= 665 && m.y <= 715 && m.uMsg == WM_LBUTTONDOWN)
				{

					ofstream outfile("data.txt");
					outfile << nafang << ' ';
					for (int i = 0; i < 9; i++)
						for (int j = 0; j < 9; j++)
							outfile << board[i][j] << ' ';
					settextstyle(80, 40, _T("华文新魏"));
					outtextxy(140, 260, "保存成功");
					MOUSEMSG m1;
					while (1) {
						m1 = GetMouseMsg();
						if (m1.uMsg == WM_LBUTTONDOWN)
						{
							putimage(0, 0, &p);
							for (int i = 0; i < 9; i++)
								for (int j = 0; j < 9; j++)
								{
									if (board[i][j] == 1)
									{
										setfillcolor(BLACK);
										fillcircle(51 + j * 67, 51 + i * 67, 25);
									}
									else if (board[i][j] == -1)
									{
										setfillcolor(WHITE);
										fillcircle(51 + j * 67, 51 + i * 67, 25);
									}
								}
							break;
						}
					}
				}
				else if (m.uMsg == WM_LBUTTONDOWN) {
					for (int i = 0; i < 9; i++)
						for (int j = 0; j < 9; j++)
							if (m.x >= (51 + j * 67 - 33) && m.x <= (51 + 67 * j + 33) && m.y >= (51 + i * 67 - 33) && m.y <= (51 + 67 * i + 33) && m.uMsg == WM_LBUTTONDOWN && board[i][j] == 0)
							{
								if (judgeAvailable(i, j, 1) == false)
									return hanshu5();
								board[i][j] = 1;
								cun1 = i;
								cun2 = j;
								xialema = 1;
								setfillcolor(WHITE);
								fillcircle(51 + j * 67, 51 + i * 67, 25);
								FlushMouseMsgBuffer();
								break;
							}
							else continue;
					if (xialema == 1)
					{
						int xianali = xiaqi();
						if (xianali == -1)
							return hanshu7();
						int x = xianali / 9;
						int y = xianali % 9;
						cun3 = x;
						cun4 = y;
						board[x][y] = -1;
						xialema = 0;
						setfillcolor(BLACK);
						fillcircle(51 + y * 67, 51 + x * 67, 25);
					}
				}
			}
		}
	}
	else {
		int xianali = xiaqi();
		int x = xianali / 9;
		int y = xianali % 9;
		cun3 = x;
		cun4 = y;
		board[x][y] = -1;
		xialema = 0;
		setfillcolor(BLACK);
		fillcircle(51 + y * 67, 51 + x * 67, 25);
		for (;;)
		{
			while (1) {
				m = GetMouseMsg();
				if (m.x >= 439 && m.x <= 628 && m.y >= 665 && m.y <= 715 && m.uMsg == WM_LBUTTONDOWN)
				{
					cut2 = 0;
					memset(fast, 0, sizeof(fast));
					memset(board, 0, sizeof(board));
					memset(dfv, 0, sizeof(dfv));
					fastmax = -100000;
					cut = 0;
					return main();
				}
				else if (m.x >= 229 && m.x <= 420 && m.y >= 665 && m.y <= 715 && m.uMsg == WM_LBUTTONDOWN && cun1 != -1)
				{
					board[cun1][cun2] = 0;
					board[cun3][cun4] = 0;
					putimage(0, 0, &p);
					for (int i = 0; i < 9; i++)
						for (int j = 0; j < 9; j++)
						{
							if (board[i][j] == 1)
							{
								setfillcolor(WHITE);
								fillcircle(51 + j * 67, 51 + i * 67, 25);
							}
							else if (board[i][j] == -1)
							{
								setfillcolor(BLACK);
								fillcircle(51 + j * 67, 51 + i * 67, 25);
							}
						}
				}
				else if (m.x >= 17 && m.x <= 210 && m.y >= 665 && m.y <= 715 && m.uMsg == WM_LBUTTONDOWN)
				{

					ofstream outfile("data.txt");
					outfile << nafang << ' ';
					for (int i = 0; i < 9; i++)
						for (int j = 0; j < 9; j++)
							outfile << board[i][j] << ' ';
					settextstyle(80, 40, _T("华文新魏"));
					outtextxy(140, 260, "保存成功");
					MOUSEMSG m1;
					while (1) {
						m1 = GetMouseMsg();
						if (m1.uMsg == WM_LBUTTONDOWN)
						{
							putimage(0, 0, &p);
							for (int i = 0; i < 9; i++)
								for (int j = 0; j < 9; j++)
								{
									if (board[i][j] == 1)
									{
										setfillcolor(WHITE);
										fillcircle(51 + j * 67, 51 + i * 67, 25);
									}
									else if (board[i][j] == -1)
									{
										setfillcolor(BLACK);
										fillcircle(51 + j * 67, 51 + i * 67, 25);
									}
								}
							break;
						}
					}
				}
				else if (m.uMsg == WM_LBUTTONDOWN) {
					for (int i = 0; i < 9; i++)
						for (int j = 0; j < 9; j++)
							if (m.x >= (51 + j * 67 - 33) && m.x <= (51 + 67 * j + 33) && m.y >= (51 + i * 67 - 33) && m.y <= (51 + 67 * i + 33) && m.uMsg == WM_LBUTTONDOWN && board[i][j] == 0)
							{
								if (judgeAvailable(i, j, 1) == false)
									return hanshu5();
								board[i][j] = 1;
								cun1 = i;
								cun2 = j;
								xialema = 1;
								setfillcolor(WHITE);
								fillcircle(51 + j * 67, 51 + i * 67, 25);
								FlushMouseMsgBuffer();
								break;
							}
							else continue;
					if (xialema == 1)
					{
						int xianali = xiaqi();
						if (xianali == -1)
							return hanshu7();
						int x = xianali / 9;
						int y = xianali % 9;
						cun3 = x;
						cun4 = y;
						board[x][y] = -1;
						xialema = 0;
						setfillcolor(BLACK);
						fillcircle(51 + y * 67, 51 + x * 67, 25);
					}
				}
			}
		}
	}
}
int hanshu7()//胜利特效
{
memset(fast, 0, sizeof(fast));
memset(board, 0, sizeof(board));
MOUSEMSG m;
cleardevice();
IMAGE p2, p3;
loadimage(&p2, "data/picture3.jpg", 640, 740);
putimage(0, 0, &p2);
loadimage(&p3, "data/picture13.jpg", 640, 640);
putimage(0, 0, &p3);
settextstyle(40, 20, _T("华文新魏"));
outtextxy(225, 670, "返回菜单");
roundrect(215, 660, 420, 720, 30, 30);
mciSendString("stop mymusic", NULL, 0, NULL);
mciSendString("play mymusic4", NULL, 0, NULL);
cut2 = 0;
memset(fast, 0, sizeof(fast));
memset(board, 0, sizeof(board));
memset(dfv, 0, sizeof(dfv));
fastmax = -100000;
cut = 0;
while (1) {
	m = GetMouseMsg();
	if (m.x >= 215 && m.x <= 420 && m.y >= 660 && m.y <= 720 && m.uMsg == WM_LBUTTONDOWN)
	{
		mciSendString("close mymusic4", NULL, 0, NULL);
		return main();
	}
}
}