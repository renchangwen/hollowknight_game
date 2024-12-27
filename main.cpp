
#include "EasyXPng.h"
#include <graphics.h>  
#include <conio.h>
#include "Timer.h"
#include <math.h>
#include <vector>
#include <unordered_map>
#include <iostream>
#pragma comment(lib,"Winmm.lib")

using namespace std;

#define  WIDTH 1000  
#define  HEIGHT 800
//#define  WIDTH2 1438
enum PlayerStatus // 枚举类型，游戏角色所有的可能状态
{
	standleft, standright, runleft, runright, jumpleftStart, jumprightStart, die, dashLeft, dashRight, jumpleftLoop, jumprightLoop, jumpleftLand, jumprightLand
};
class Carmera {//相机
public:
	float x, y;
	float w, h;

	void init(float x1, float y1, float w1, float h1) {
		x = x1;
		y = y1;
		w = w1;
		h = h1;
	}
	void trans(float x1, float y1, PlayerStatus playerstatus, int t) {
		if ((playerstatus == runleft || playerstatus == runright ||
			playerstatus == jumpleftStart || playerstatus == jumpleftLoop || playerstatus == jumpleftLand ||
			playerstatus == jumprightStart || playerstatus == jumprightLoop || playerstatus == jumprightLand) && t == -1) {//在攻击时不会出现抖动
			if (x1 <= WIDTH / 2) {
				x = WIDTH / 2;
			}
			else if (x1 >= (w - WIDTH / 2)) {
				x = w - WIDTH / 2;
			}
			else {
				if (abs(x - x1) >= 5)//防抖动
					x = x1;
			}
			if (y1 >= (h - HEIGHT / 2)) {
				y = (h - HEIGHT / 2);
			}
			else if (y1 <= HEIGHT / 2) {
				y = HEIGHT / 2;
			}
			else {
				if (abs(y - y1) >= 5)
					y = y1;
			}
		}
	}
};


class Enemy {
public:
	float left_x, top_y, right_x, bottom_y;
	int blood;//血量
	int dis, vx, dir;
	int animId, turnmux;
	int c;//c为里程数
	int tEnemy;//enemy类别,1表示苔藓虫，2表示
	int sprint;
	IMAGE im_walkR[4];
	IMAGE im_walkL[4];
	IMAGE im_turnR[3];
	IMAGE im_turnL[3];
	IMAGE im_attackL[4];
	IMAGE im_attackR[4];
	IMAGE im_show; 

	void init(float xm, float ym, int cm,int t) {
		tEnemy = t;
		//cout <<"ddd" << tEnemy << endl;
		TCHAR filename[80];
		sprint = 0;
		if (tEnemy == 1) {
			for (int i = 0; i < 4; i++) {
				_stprintf_s(filename, _T("images\\enemy1\\WalkL\\%d.png"), i);
				loadimage(&im_walkL[i], filename);
				_stprintf_s(filename, _T("images\\enemy1\\WalkR\\%d.png"), i);
				loadimage(&im_walkR[i], filename);
			}
			im_show = im_walkR[animId];
			top_y = ym;//1720;//floor的平台
			left_x = xm;//400;
			right_x = left_x + im_show.getwidth();
			bottom_y = top_y + im_show.getheight();
			dis = 0;
			vx = 6;
			dir = 1;//1或-1
			animId = 0;
			turnmux = 0;
			blood = 9;//玩家每刀减3
			c = cm;
		}
		if (tEnemy == 2) {
			for (int i = 0; i < 4; i++) {
				_stprintf_s(filename, _T("images\\enemy2\\Walk\\%d.png"), i);
				loadimage(&im_walkL[i], filename);
				_stprintf_s(filename, _T("images\\enemy2\\WalkR\\%d.png"), i);
				loadimage(&im_walkR[i], filename);
			}
			blood = 18;
			im_show = im_walkR[animId];
			top_y = ym;//1720;//floor的平台
			left_x = xm;//400;
			right_x = left_x + im_show.getwidth();
			bottom_y = top_y + im_show.getheight();
			bottom_y = top_y + im_show.getheight();
			dis = 0;
			vx = 6;
			dir = 1;//1或-1，1向右，-1向左
			animId = 0;
			turnmux = 0;//enemy2,为1时冲刺，不走路
			c = cm;
		}
		for (int i = 0; i < 3; i++) {
			_stprintf_s(filename, _T("images\\enemy1\\TurnL\\%d.png"), i);
			loadimage(&im_turnL[i], filename);
			_stprintf_s(filename, _T("images\\enemy1\\TurnR\\%d.png"), i);
			loadimage(&im_turnR[i], filename);
		}
		for (int i = 0; i < 4; i++) {
			_stprintf_s(filename, _T("images\\enemy2\\Attack\\%d.png"), i);
			loadimage(&im_attackL[i], filename);
			_stprintf_s(filename, _T("images\\enemy2\\AttackR\\%d.png"), i);
			loadimage(&im_attackR[i], filename);
		}
	}
	void draw(Carmera& carmera) {
		putimagePng(left_x - carmera.x + WIDTH / 2, top_y - carmera.y + HEIGHT / 2, &im_show);
	}
	void run(float px1, float px2, float py1, float py2) {//c为enemy1的里程数
		if (tEnemy == 1) {
			if (turnmux == 0) {
				if (animId >= 4) {
					animId = 0;
				}
				left_x += dir * vx;
				right_x += dir * vx;
				dis += dir * vx;//有正有负
				if (dir == 1) {
					im_show = im_walkR[animId];
				}
				if (dir == -1) {
					im_show = im_walkL[animId];
				}
				if (abs(dis) > c) {
					dir = -dir;
					animId = 0;
					turnmux = 1;
				}
				else {
					animId++;
				}
			}
			turn();
		}
		if (tEnemy == 2) {
			if (turnmux == 0) {
				if (animId >= 4) {
					animId = 0;
				}
				left_x += dir * vx;
				right_x += dir * vx;
				dis += dir * vx;
				if (dir == 1) {
					im_show = im_walkR[animId];
				}
				if (dir == -1) {
					im_show = im_walkL[animId];
				}
				if (abs(dis) > c) {//大于一定距离转向
					dir = -dir;
					animId = 0;
				}
				else {
					animId++;
				}
			}
			attack(px1,px2,py1,py2);
  		}
	}
	void turn() {
		if (turnmux == 1) {
			if (animId >= 3) {
				animId = 0;
				turnmux = 0;
				return;
			}
			if (dir == 1) {
				im_show = im_turnR[animId];
			}
			if (dir == -1) {
				im_show = im_turnL[animId];
			}
			animId++;
		}
	}
	void attack(float px1,float px2,float py1,float py2) {
		if (turnmux == 0) {
			//if (abs(dis + c * dir) <= c) {
				if (abs(py1-top_y)<=50 && (dir == 1 && px1 - right_x <= 150 && px1 - right_x >= 50 || dir == -1 && left_x - px2 <= 150 && left_x - px2 >= 50)) {
					turnmux = 1;
					animId = 0;
					vx = 16;
				}
			//}
		}
		else {
			if (animId >= 3) {
				animId = 3;
				if (abs(sprint)>=200) {
					turnmux = 0;
					dir = -dir;
					vx = 6;
					sprint = 0;
					animId = 0;
					dis = 0;
					return;
				}
			}
			if (dir == 1) {
				im_show = im_attackR[animId];
			}
			if (dir == -1) {
				im_show = im_attackL[animId];
			}
			left_x += dir * vx;
			right_x += dir * vx;
			sprint += dir * vx;
			
			animId++;
		}
	}
	void hurt(int hur) {
		//cout << blood << endl;
		blood -= hur;
	}
};

class Floor {
public:
	float left_x, top_y, right_x;
	float floor_width, floor_height;
	IMAGE im_floor, im_plat2, im_now;//im_now为实际的平台
	IMAGE im_wideplat, im_block;
	void init(int c) {
		loadimage(&im_floor, _T("images/environment/plat/Floor_1.png"));
		loadimage(&im_plat2, _T("images\\environment\\plat\\ruin_plat_float_02.png"));
		loadimage(&im_wideplat, _T("images\\environment\\plat\\ruin_plat_float_05_wide.png"));
		loadimage(&im_block, _T("images\\environment\\plat\\block.png"));
		left_x = 0;
		right_x = 1;
		top_y = 0;
		if (c == 1) {
			floor_width = im_floor.getwidth();
			floor_height = im_floor.getheight();
			im_now = im_floor;
		}
		if (c == 2) {
			//cout << "plat" << endl;
			floor_width = im_plat2.getwidth();
			floor_height = im_plat2.getheight();
			im_now = im_plat2;
		}
		if (c == 3) {
			floor_width = im_wideplat.getwidth();
			floor_height = im_wideplat.getheight();
			im_now = im_wideplat;
		}
		if (c == 4) {
			floor_width = im_block.getwidth();
			floor_height = im_block.getheight();
			im_now = im_block;
		}

	}
	void draw(Carmera& carmera) {
		putimagePng(left_x - carmera.x + WIDTH / 2, top_y - carmera.y + HEIGHT / 2, &im_now);
	}
};
class Trip {//陷阱
public:
	IMAGE im_trap;
	float x_left, x_right, y_top, y_bottom;
	int vy,mutex,hei;
	float limit1, limit2;
	void init(int h,float lim1,float lim2){
		loadimage(&im_trap, _T("images\\environment\\envir\\trap.png"));
		x_left = (lim1+lim2)/2;
		x_right = x_left + im_trap.getwidth();
		y_bottom = h - HEIGHT;
		hei = h;
		y_top = y_bottom - im_trap.getheight();
		vy = 12;
		limit1 = lim1;
		limit2 = lim2;
		mutex = 0;
	}
	void draw(Carmera& carmera) {
		if (mutex == 1) {
			putimagePng(x_left - carmera.x + WIDTH / 2, y_top - carmera.y + HEIGHT / 2, &im_trap);
		}
	}
	void trick(float tx) {
		if (tx > limit1 && tx < limit2) {
			mutex = 1;
		}
		move();
	}
	void move() {
		if (mutex == 1) {
			y_bottom += vy;
			y_top = y_bottom - im_trap.getheight();
		}
		if (y_top> hei) {
			mutex = 0;
			y_bottom = hei - HEIGHT;
			y_top = y_bottom - im_trap.getheight();
		}
	}
};
class Sword {
public:
	float x_left, x_right, y_top, y_bottom;
	IMAGE im_swordL, im_swordR;
	float w, h;
	int dir,vx,dir2;
	float bx;
	int mutex;
	void init() {
		loadimage(&im_swordL, _T("images\\Boss1\\sword\\1.png"));
		loadimage(&im_swordR, _T("images\\Boss1\\swordR\\1.png"));
		w = im_swordL.getwidth();
		h = im_swordL.getheight();
		x_left = 0, x_right = 0, y_bottom = 0, y_top = 0;
		bx = 0, dir = 1,dir2=1;
		mutex = 0;
	}
	void draw(Carmera& carmera) {
		if (dir == -1) {
			putimagePng(x_left - carmera.x + WIDTH / 2, y_top - carmera.y + HEIGHT / 2, &im_swordL);
		}
		if (dir == 1) {
			putimagePng(x_left - carmera.x + WIDTH / 2, y_top - carmera.y + HEIGHT / 2, &im_swordR);
		}
	}
	void set(int d, float bx1,float by1,int vx1) {
		dir = d;
		dir2 = d;
		bx = bx1;
		y_top = by1;
		y_bottom = y_top + h;
		if (dir == 1) {
			x_left = bx;
			x_right = x_left + w;
		}
		if (dir == -1) {
			x_right = bx;
			x_left = x_right - w;
		}
		vx = vx1;
	}
	int move(int doc) {
		if (dir == 1) {
			if (x_right >= doc) {
				if (mutex == 0) {
					dir2 = -dir2;
					vx = 2 * vx / 5;
					mutex = 1;
				}
			}
			if (x_left < bx) {
				mutex = 0;
				return 1;
			}
		}
		if (dir == -1) {
			//cout << x_left << endl;
			if (x_left <= doc) {
				if (mutex == 0) {
					dir2 = -dir2;
					vx = 2 * vx / 5;
					mutex = 1;
				}
			}
			if (x_right > bx) {
				mutex = 0;
				return 1;
			}
		 }
		x_left += dir2 * vx;
		x_right += dir2 * vx;
		return 0;
		}
};
class Boss {
public:
	int blood;
	IMAGE im_idleL,im_idleR;
	IMAGE im_show;
	IMAGE im_runL[8], im_runR[8];
	IMAGE im_jumpL[8], im_jumpR[8];
	IMAGE im_dashfloorL, im_dashfloorR;
	IMAGE im_dashairL, im_dashairR;
	IMAGE im_throwL[16], im_throwR[16];
	IMAGE im_barbL[8], im_barbR[8];

	float x_left, x_right, y_top, y_bottom;
	float w, h;
	int direction,vx,vy,sprintx,sprinty;
	float docL, docR;
	int jumpMux, jumpsprintMux, floorsprintMux, runMux, barbMux,standMux,throwMux;
	int animId, gravity;
	int countAtt,px,floor_top,win;
	Sword sword;
	DWORD t1, t2;

	void init(int L,int R,float floor_top1) {
		loadimage(&im_idleL, _T("images/Boss1/idle/1.png"));
		loadimage(&im_idleR, _T("images\\Boss1\\idleR\\1.png"));
		loadimage(&im_dashairL, _T("images\\Boss1\\dash_in_air\\1.png"));
		loadimage(&im_dashairR, _T("images\\Boss1\\dash_in_airR\\1.png"));
		loadimage(&im_dashfloorL, _T("images\\Boss1\\dash_on_floor\\1.png"));
		loadimage(&im_dashfloorR, _T("images\\Boss1\\dash_on_floorR\\1.png"));

		TCHAR filename[80];
		for (int i = 0; i < 8; i++) {
			_stprintf_s(filename, _T("images\\Boss1\\run\\%d.png"), i+1);
			loadimage(&im_runL[i], filename);
			_stprintf_s(filename, _T("images\\Boss1\\runR\\%d.png"), i + 1);
			loadimage(&im_runR[i], filename);
			_stprintf_s(filename, _T("images\\Boss1\\jump\\%d.png"), i + 1);
			loadimage(&im_jumpL[i], filename);
			_stprintf_s(filename, _T("images\\Boss1\\jumpR\\%d.png"), i + 1);
			loadimage(&im_jumpR[i], filename);
			_stprintf_s(filename, _T("images\\Boss1\\throw_barb\\%d.png"), i + 1);
			loadimage(&im_barbL[i], filename);
			_stprintf_s(filename, _T("images\\Boss1\\throw_barbR\\%d.png"), i + 1);
			loadimage(&im_barbR[i], filename);
		}
		for (int i = 0; i < 16; i++) {
			_stprintf_s(filename, _T("images\\Boss1\\throw_sword\\%d.png"), i + 1);
			loadimage(&im_throwL[i], filename);
			_stprintf_s(filename, _T("images\\Boss1\\throw_swordR\\%d.png"), i + 1);
			loadimage(&im_throwR[i], filename);
		}
		im_show = im_idleL;
		w = im_show.getwidth();
		h = im_show.getheight();
		floor_top = floor_top1;
		x_left = WIDTH - 150;
		y_bottom = floor_top ;
		y_top = y_bottom-h;
		x_right = x_left + w;
		direction = -1;
		vx = 10;
		vy = 0;
		sprintx = 16;
		sprinty = 0;
		docL = L;
		docR = R;
		blood = 48;
		jumpMux = 0, jumpsprintMux = 0, floorsprintMux = 0, runMux = 0, barbMux = 0,throwMux=0,standMux=1;
		animId = 0, gravity = 4, countAtt=0,px=0; //countAtt计数
		sword.init();
		t1 = 0, t2 = 0;//计时
		win = 0;
	}
	void updateXY(float xm,float ym) {
		x_left = xm;
		y_top = ym;
		x_right = x_left + w;
		y_bottom = y_top + h;
	}
	void draw(Carmera& carmera) {
	/*	w = im_show.getwidth();
		h = im_show.getheight();*/
		putimagePng(x_left - carmera.x + WIDTH / 2, y_top - carmera.y + HEIGHT / 2, &im_show);
		if (throwMux == 1&&animId>=15) {
			sword.draw(carmera);
		}
	}
	void statusChange(float px1)  {
		/*计时1s*/
		//cout << standMux << jumpsprintMux << floorsprintMux << runMux << throwMux << jumpMux << endl;
		if (standMux == 1&&(t1-t2)>=500) {
			px = px1;
			if (rand()%100<60&&((abs(x_left - docL) <= 50) || abs(x_right - docR) <= 50)) {
				if (abs(x_left- docL) <= 50) {//左
					direction = 1;
					jumpsprintMux = 1;
					vy = -60;
					standMux = 0;
				}
				if (abs(x_right- docR) <= 50) {//右
					direction = -1;
					jumpsprintMux = 1;
					vy = -60;
					standMux = 0;
				}
			}
			else if (rand() % 100 < 50&&abs(px - x_left) < 150) {
				standMux = 0;
				jumpMux = 1;
				vy = -60;
				if (x_left - px < 150 && px < x_left) {//左
					direction = -1;
				}
				if (px - x_left <= 150 && px >= x_left) {//右
					direction = 1;
				}
				//return;
			}
			else{
				if (abs(px - x_left) >= 500) {
					throwMux = 1;
					standMux = 0;
					if (x_left >px ) {//左
						direction = -1;
						sword.set(-1, x_left - 20, y_top + 50 , 5*vx);
					}
					if (x_left <= px) {//右
						direction = 1;
						sword.set(1, x_right + 20, y_top + 50 , 5*vx);
					}
				}
				if (abs(px - x_left) < 500|| abs(px - x_left) >= 500&& throwMux == 0) {
					floorsprintMux = 1;
					vx = 3 * vx;
					standMux = 0;
					if (x_left >= px) {//左
						direction = -1;
					}
					if (x_left <= px) {//右
						direction = 1;
					}
				}
			}
		}
		if (floorsprintMux == 2) {//2代表动作完成
			px = px1;
			standMux = 1;
			t2 = GetTickCount();
			floorsprintMux = 0;
		}
		if (throwMux == 2) {
			px = px1;
			if (rand() % 100 < 60) {
				standMux = 1;
				t2 = GetTickCount();
			}
			else {
				floorsprintMux = 1;
				vx = 3 * vx;
			}
			throwMux = 0;
		}
		if (jumpMux == 2) {
			px = px1;
			standMux = 1;
			t2 = GetTickCount();
			jumpMux = 0;
		}
		if (jumpsprintMux == 2) {
			px = px1;
			if ((x_left - docL < 320 && docL<x_left && px>x_left ||
				docR - x_right < 320 && docR>x_right && px < x_left)&&rand()%100<65) {
				runMux = 1;
				jumpsprintMux = 0;
				animId = 0;
				if (x_left - docL <= 320 && docL<x_left && px>x_left) {//左
					direction = -1;
				}
				if (docR - x_right <= 320 && docR>x_right && px < x_left) {//右
					direction = 1;
				}
			}
			else if (x_left - docL < 420 && x_left-docL>150 && px>x_left ||
				docR - x_right < 420 && docR- x_right>150 && px < x_left) {
				if ( rand() % 100 < 50) {
			//else if (rand() % 100 < 40) {
					floorsprintMux = 1;
					vx = 3 * vx;
					jumpsprintMux = 0;
					animId = 0;
					if (x_left >= px) {//左
						direction = -1;
					}
					if (x_left <= px) {//右
						direction = 1;
					}
					}
				else {
					throwMux = 1;
					jumpsprintMux = 0;
					animId = 0;
					if (x_left  >= px) {//左
						direction = -1;
						sword.set(-1, x_left - 20,y_top+50 ,5*vx );
					}
					if (x_left <= px) {//右
						direction = 1;
						sword.set(1, x_right + 20, y_top + 50, 5*vx);
					}
				}
			}
			else {
				standMux = 1;
				t2 = GetTickCount();
				jumpsprintMux = 0;
			}
		}
		if (barbMux == 2) {
			px = px1;
			standMux = 1;
			t2 = GetTickCount();
			barbMux = 0;
		}
		if (runMux == 2) {
			px = px1;
			runMux = 0;
			standMux = 1;
			t2 = GetTickCount();
		}
	}
	void standStill() {
		if (standMux == 1) {
			t1 = GetTickCount();
			animId = 0;
			if (px > x_left) {
				direction = 1;
				im_show = im_idleR;
			}
			if (px <= x_left) {
				direction = -1;
				im_show = im_idleL;
			}
		}
	}
	void run(){
		if (runMux == 1) {
		if (direction == -1) {
			if (animId >= 8) {
				animId = 0;
			}
			if (x_left <= docL) {
				runMux = 2;
	/*			direction = -direction;
				im_show = im_idleR;*/
				return;
			}
			im_show = im_runL[animId];
			animId++;
			x_left += direction*vx;
			x_right = x_left + w;
		}
		if (direction == 1) {
			if (animId >= 8) {
				animId = 0;
			}
			if (x_right >= docR) {
				runMux = 2;
			/*	direction = -direction;
				im_show = im_idleL;*/
				return;
			}
			im_show = im_runR[animId];
			animId++;
			x_left += direction*vx;
			x_right = x_left + w;
		}
		}
	}
	void jump() {
		if (jumpMux == 1) {
			vy += gravity;
			y_bottom += vy;  // y方向位置受速度影响变化
			y_top = y_bottom - h;
			//cout << y_bottom << "   " << vy << endl;
			if (direction == -1) {
				if (animId >= 7) {
					animId = 7;
				}
				if (y_bottom >= floor_top) {
					y_bottom = floor_top;  // y方向位置受速度影响变化
					y_top = y_bottom - h;
					jumpMux = 2;
					vy = 0;
					return;
				}
				im_show = im_jumpL[animId];
				animId++;
			}
			if (direction == 1) {
				if (animId >= 7) {
					animId = 7;
				}
				if (y_bottom >= floor_top) {
					y_bottom = floor_top;  // y方向位置受速度影响变化
					y_top = y_bottom - h;
					jumpMux = 2;
					return;
				}
				im_show = im_jumpR[animId];
				animId++;
			}
		}
	}
	void sprintJump(float px1) {
		if (jumpsprintMux == 1) {
			//cout << y_bottom << "   " << vy << endl;
			if (abs(vy) < 3) {
				px = px1;
				gravity = 6;//总上升为450
				vx = abs(x_left - px)/ (sqrt(2 * 450 / gravity));
			}
			if (vy <= -3) {
				vy += gravity;
				y_bottom += vy;  // y方向位置受速度影响变化
				y_top = y_bottom - h;
				if (direction == -1) {
					if (animId >= 7) {
						animId = 7;
					}
					im_show = im_jumpL[animId];
					animId++;
				}
				if (direction == 1) {
					if (animId >= 7) {
						animId = 7;
					}
					im_show = im_jumpR[animId];
					animId++;
				}
			}
			else {//冲刺
				vy += gravity;
				y_bottom += vy;  // y方向位置受速度影响变化
				y_top = y_bottom - h;
				x_left += direction * vx;
				x_right = x_left + w;
				if (direction == -1) {
					im_show = im_dashairL;
				}
				if (direction == 1) {
					im_show = im_dashairR;
				}
				if (y_bottom >= floor_top) {
					y_bottom = floor_top;  // y方向位置受速度影响变化
					y_top = y_bottom - h;
					jumpsprintMux = 2;
					vx = 10;
					vy = 0;
					return;
				}
			}
		}
	}
	void sprintFloor() {
		if (floorsprintMux == 1) {
			x_left += direction * vx;
			x_right = x_left + w;
			if (direction == -1) {
				im_show = im_dashfloorL;
				if (x_left <= docL) {
					floorsprintMux = 2;
					vx = 10;
					return;
				}
			}
			if (direction == 1) {
				im_show = im_dashfloorR;
				if (x_right >= docR) {
					floorsprintMux = 2;
					vx = 10;
					return;
				}
			}
		}
	}
	void throwSword(){
		if (throwMux == 1) {
			if (direction == -1) {
				//cout << animId << endl;
				if (animId >= 15) {
					animId = 15;
					if (sword.move(docL) == 1) {
						throwMux = 2;
						return;
					}
				}
				im_show = im_throwL[animId];
				animId++;
			}
			if (direction == 1) {
				if (animId >= 15) {
					animId = 15;
					if (sword.move(docR) == 1) {
						throwMux = 2;
						return;
					}
				}
				im_show = im_throwR[animId];
				animId++;
			}
		}
	}

	void hurt(int hur) {
		blood -= hur;
		cout << blood << endl;
		if (blood <= 0) {
			win = 1;
			blood = 0;
		}
		countAtt++;
	}
};
class Scene {//场景类
public:
	IMAGE im_bk;
	float bk_w, bk_h, fl_w;
	Carmera carmera;
	vector<Floor> floors;
	vector<Enemy> enemys;//enemy1,苔藓虫
	Boss boss;
	Trip trip[5];//陷阱
	int playerUpdate;
	int level;
	int spawnpointx[3];//复活点
	int spawnpointy[3];
	void draw() {
		putimage(0 - carmera.x + WIDTH / 2, 0 - carmera.y + HEIGHT / 2, &im_bk);
		for (int i = 0; i < floors.size(); i++) {
			floors[i].draw(carmera);
		}
		for (int i = 0; i < enemys.size(); i++) {
			enemys[i].draw(carmera);
		}
		for (int i = 0; i < 5; i++) {
			trip[i].draw(carmera);
		}
	}
	void draw2() {
		putimage(0 - carmera.x + WIDTH / 2, 0 - carmera.y + HEIGHT / 2, &im_bk);
		for (int i = 0; i < floors.size(); i++) {
			floors[i].draw(carmera);
		}
		boss.draw(carmera);
	}
	void init() {
		level = 1;
			loadimage(&im_bk, _T("images/environment/background/5.png"));
			cout << &im_bk << endl;
			bk_w = im_bk.getwidth();
			bk_h = im_bk.getheight();
			carmera.init(WIDTH / 2, bk_h - HEIGHT / 2, bk_w, bk_h);
			floors.clear();
			enemys.clear();
			Floor floor;
			Floor floor2;
			Floor floor3;
			Floor floor4;
			Enemy enemy;
			Enemy enemy2;
			for (int i = 0; i < 1; i++) {
				floor.init(1);
				floor.left_x = i * floor.floor_width;
				floor.right_x = floor.left_x + floor.floor_width;
				floor.top_y = bk_h - floor.floor_height;
				fl_w = floor.floor_width;
				floors.push_back(floor);
				spawnpointx[0] = floor.right_x - 100;//左
				spawnpointy[0] = floor.top_y;//下
			}
			for (int i = 0; i < 5; i++) {
				floor2.init(2);
				floor2.left_x = fl_w + 100 + 150 * (i + 1) * ((1.0 * (10 + rand() % 3)) / 10) + i * floor2.floor_width;//(10+rand()%5)/10: 1——1.5
				floor2.right_x = floor2.left_x + floor2.floor_width;
				floor2.top_y = 1700 + rand() % 50;
				floors.push_back(floor2);
				if (i == 2) {
					enemy.init(floor2.left_x, floor2.top_y - 110, 18, 1);
					enemys.push_back(enemy);
				}
			}
			int cc = 0;
			for (int i = 0; i < 2; i++) {
				floor3.init(3);
				floor3.left_x = fl_w + (200 * 8 + floor3.floor_width * i);
				floor3.right_x = floor3.left_x + floor3.floor_width;
				floor3.top_y = 1700;
				floors.push_back(floor3);
				if (i == 1) {
					cc = floor3.right_x;
					spawnpointx[1] = floor3.right_x - 100;//左
					spawnpointy[1] = floor3.top_y;//下
				}
			}
			for (int i = 0; i < 5; i++) {
				floor2.init(2);
				floor2.left_x = cc + 100 + 150 * (i + 1) * ((1.0 * (10 + rand() % 3)) / 10) + i * floor2.floor_width;//(10+rand()%5)/10: 1——1.5
				floor2.right_x = floor2.left_x + floor2.floor_width;
				floor2.top_y = 1800 + rand() % 100;
				floors.push_back(floor2);
				trip[i].init(im_bk.getheight(), floor2.left_x, floor2.right_x);//陷阱
				if (i == 4) {
					cc = floor2.right_x;
				}
			}
			for (int i = 0; i < 6; i++) {
				floor4.init(4);
				floor4.left_x = cc + (100 + floor4.floor_width * i);
				floor4.right_x = floor4.left_x + floor4.floor_width;
				floor4.top_y = 1900;
				floors.push_back(floor4);
				if (i == 2 || i == 4) {
					enemy2.init(floor4.left_x, floor4.top_y - 130, 140, 2);//产生enemy2
					enemys.push_back(enemy2);
				}
				if (i == 5) {
					cc = floor4.right_x;
				}
			}
			for (int i = 0; i < 9; i++) {
				floor4.init(4);
				floor4.left_x = cc + (100 + floor4.floor_width * i);
				floor4.right_x = floor4.left_x + floor4.floor_width;
				floor4.top_y = 1700;
				if (i == 1) {//在block上生成一个plat2
					//Floor floor1;
					floor2.init(2);
					floor2.left_x = floor4.right_x;
					floor2.right_x = floor2.left_x + floor2.floor_width;
					floor2.top_y = 1500;
					floors.push_back(floor2);
				}
				floors.push_back(floor4);
				if (i == 0) {
					spawnpointx[2] = floor4.left_x + 100;//左
					spawnpointy[2] = floor4.top_y;//下
				}
				if (i >= 2 && i <= 7) {
					enemy2.init(floor4.left_x, floor4.top_y - 140, 110 + rand() % 60, 2);//产生enemy2
					enemys.push_back(enemy2);
					enemy2.init(floor4.left_x + 100, floor4.top_y - 140, 110 + rand() % 60, 2);//产生enemy2
					enemys.push_back(enemy2);
					enemy2.init(floor4.left_x + 200, floor4.top_y - 140, 110 + rand() % 60, 2);//产生enemy2
					enemys.push_back(enemy2);
				}
				if (i == 8) {
					cc = floor4.right_x;
				}
			}
			for (int i = 0; i < 7; i++) {
				if (i % 2 == 0) {
					//Floor floor;
					floor4.init(4);
					floor4.left_x = cc;
					floor4.right_x = floor4.left_x + floor4.floor_width;
					floor4.top_y = 1900;
					cc += floor4.floor_width;
					floors.push_back(floor4);
					//Enemy enemy;
					enemy.init(floor4.left_x + 120, floor4.top_y - 110, 120, 1);
					enemys.push_back(enemy);
				}
				else {
					cc += 200;
				}
			}
			//Floor floor;
			floor4.init(4);
			floor4.left_x = cc;
			floor4.right_x = floor4.left_x + floor4.floor_width;
			floor4.top_y = 1900;
			floors.push_back(floor4);

			//Enemy enemytt;
			enemy.init(400, 1720, 120, 1);//在初始地方
			enemys.push_back(enemy);
			enemy.init(fl_w + (200 * 8 + 400), 1600, 120, 1);//在plat2上
			enemys.push_back(enemy);
			playerUpdate = 0;
	}
	void init2() {
		loadimage(&im_bk, _T("images\\environment\\background\\background_fight2.png"));
		level = 2;
		bk_w = im_bk.getwidth();
		bk_h = im_bk.getheight();
		cout << &im_bk<<endl;
		cout << im_bk.getheight();
		carmera.init(WIDTH / 2, HEIGHT / 2, bk_w, bk_h);
		floors.clear();
		enemys.clear();
		Floor floor;
		floor.init(1);
		floor.left_x =-50;
		floor.right_x = floor.left_x + floor.floor_width;
		floor.top_y = bk_h - floor.floor_height+100;
		fl_w = floor.floor_width;
		floors.push_back(floor);
		boss.init(50, WIDTH - 80, floor.top_y);
		playerUpdate = 0;
		//floor.init(1);
		//floor.left_x = -50+ floor.floor_width;
		//floor.right_x = floor.left_x + floor.floor_width;
		//floor.top_y = bk_h - floor.floor_height + 100;
		//floors.push_back(floor);

	}
	void enemyMove(float px1, float px2, float py1, float py2) {
		for (int i = 0; i < enemys.size(); i++) {
			enemys[i].run(px1,px2,py1,py2);
			//enemys[i].turn();
			if (enemys[i].blood <= 0) {
				if (enemys[i].tEnemy == 2&& playerUpdate<2&&enemys[i].bottom_y>1900-140) {
					playerUpdate++;
				}
				enemys.erase(enemys.begin() + i);
			}
		}
	}
};
//class Animation {
//public:
//	int count;
//	vector<IMAGE*> lists;
//	vector<float> xcor;
//	vector<float> ycor;
//	void init() {
//		count = 6;
//		lists.clear();
//		//xcor.clear();
//		//ycor.clear();
//	}
//	void putIMA(float x, float y, IMAGE* im) {
//		lists.push_back(im);
//		//xcor.push_back(x);
//		//ycor.push_back(y);
//		putimagePng(x, y, (lists.front()));
//		count--;
//		if (count == 0) {
//			count = 6;
//			lists.erase(lists.begin());
//			//xcor.erase(xcor.begin());
//			//ycor.erase(ycor.begin());
//		}
//	}
//};
class Player {
public:
	float x_left, y_top, y_bottom, x_right;
	float effect_x, effect_y;
	float width, height;
	float vx, vy;
	int animId, effectId, attId;  //当前时刻要显示的图像序列
	int life, death, alive;//生命条
	int hur;
	PlayerStatus playerStatus;
	float gravity;
	IMAGE im_show;////当前时刻要显示的图像
	IMAGE im_effect;
	IMAGE im_leftsplit, im_rightsplit, im_downsplit;

	IMAGE im_idleL, im_idleR;//站立
	IMAGE im_walkL[6];//向左移动
	IMAGE im_walkR[6];//向右移动
	IMAGE im_JumpStartL[9];//左跳图像序列
	IMAGE im_JumpLoopL;
	IMAGE im_JumpLandL[3];
	IMAGE im_JumpStartR[9];//右跳图像序列
	IMAGE im_JumpLoopR;
	IMAGE im_JumpLandR[3];
	IMAGE im_death[10];
	IMAGE im_attackL[5];//攻击人物图像序列
	IMAGE im_attackR[5];
	IMAGE im_attackD[5];
	IMAGE im_attackeffectL[5];
	IMAGE im_attackeffectR[5];
	IMAGE im_life[7];
	int attmuxL, attmuxR, attmuxD, jumpmux, doAttackL, doAttackR;
	int mutexUpdate;//是否更新
	PlayerStatus frontstatus;

	//Animation animation;

	void init() {
		loadimage(&im_idleL, _T("images\\player\\Idle\\0.png"));
		loadimage(&im_idleR, _T("images\\player\\IdleR\\0.png"));
		TCHAR filename[80];
		for (int i = 0; i < 6; i++) {
			_stprintf_s(filename, _T("images\\player\\Walk2\\%d.png"), i);
			loadimage(&im_walkL[i], filename);
			_stprintf_s(filename, _T("images\\player\\WalkR2\\%d.png"), i);
			loadimage(&im_walkR[i], filename);
		}
		for (int i = 0; i < 9; i++) {//导入起跳
			_stprintf_s(filename, _T("images\\player\\Jump\\Start\\%d.png"), i);
			loadimage(&im_JumpStartL[i], filename);
			_stprintf_s(filename, _T("images\\player\\JumpR\\Start\\%d.png"), i);
			loadimage(&im_JumpStartR[i], filename);
		}
		loadimage(&im_JumpLoopL, _T("images\\player\\Jump\\Loop\\0.png"));
		loadimage(&im_JumpLoopR, _T("images\\player\\JumpR\\Loop\\0.png"));
		for (int i = 0; i < 3; i++) {//导入着陆
			_stprintf_s(filename, _T("images\\player\\Jump\\Land\\%d.png"), i);
			loadimage(&im_JumpLandL[i], filename);
			_stprintf_s(filename, _T("images\\player\\JumpR\\Land\\%d.png"), i);
			loadimage(&im_JumpLandR[i], filename);
		}
		for (int i = 0; i < 5; i++) {//导入人物攻击
			_stprintf_s(filename, _T("images\\player\\Attack\\Attack\\1\\%d.png"), i);
			loadimage(&im_attackL[i], filename);
			_stprintf_s(filename, _T("images\\player\\Attack\\AttackR\\%d.png"), i);
			loadimage(&im_attackR[i], filename);
			_stprintf_s(filename, _T("images\\player\\Attack\\AttackDown\\%d.png"), i);
			loadimage(&im_attackD[i], filename);
		}
		loadimage(&im_leftsplit, _T("images\\player\\Attack\\Attack\\1\\100.png"));
		loadimage(&im_rightsplit, _T("images\\player\\Attack\\AttackR\\100.png"));
		loadimage(&im_downsplit, _T("images\\player\\Attack\\AttackDown\\100.png"));
		for (int i = 0; i < 5; i++) {
			_stprintf_s(filename, _T("images\\player\\AttackEffect\\Attack\\%d.png"), i);
			loadimage(&im_attackeffectL[i], filename);
			_stprintf_s(filename, _T("images\\player\\AttackEffect\\AttackR\\%d.png"), i);
			loadimage(&im_attackeffectR[i], filename);
		}
		for (int i = 0; i < 7; i++) {
			loadimage(&im_life[i], _T("images\\player\\UI\\Blood\\0.png"));
		}
		for (int i = 0; i < 10; i++) {
			_stprintf_s(filename, _T("images\\player\\Death\\%d.png"), i);
			loadimage(&im_death[i], filename);
		}

		updateXY(100, 1000);
		im_show = im_idleR;
		mutexUpdate = 0;
		im_effect = im_attackeffectL[0];
		x_right = x_left + im_show.getwidth();
		y_bottom = y_top + im_show.getheight();
		vx = 30;
		vy = 0;
		width = im_show.getwidth();
		height = im_show.getheight();
		animId = 0, effectId = 0, attId = -1;
		gravity = 4;
		playerStatus = jumprightStart;
		attmuxL = 0, attmuxR = 0, jumpmux = 0, attmuxD = 0, doAttackL = 0, doAttackR = 0;
		//animation.init();
		effect_x = 0, effect_y = 0;
		life = 5, death = 0, alive = 0;
		hur = 3;//每刀伤害
	}
	void draw(Scene& scene) {
		scene.carmera.trans(x_left + im_show.getwidth() / 2, y_top + im_show.getheight() / 2, playerStatus, attId);//所以需保证每次先绘制player
		//putimagePng(x_left - scene.carmera.x + WIDTH / 2, y_top - scene.carmera.y + HEIGHT / 2,&im_show);
		if (alive <= 8 && alive > 4 || alive <= 16 && alive > 12 || alive <= 24 && alive > 20 || alive == 0) {
			//animation.putIMA(x_left - scene.carmera.x + WIDTH / 2, y_top - scene.carmera.y + HEIGHT / 2, &im_show);
			putimagePng(x_left - scene.carmera.x + WIDTH / 2, y_top - scene.carmera.y + HEIGHT / 2,&im_show);
		}
		if (alive != 0) {
			alive--;
		}
		if (doAttackL == 1 || doAttackR == 1) {
			putimagePng(effect_x - scene.carmera.x + WIDTH / 2, effect_y - scene.carmera.y + HEIGHT / 2, &im_effect);
		}
		if (attmuxL == 1) {
			putimagePng(x_left - scene.carmera.x + WIDTH / 2 - im_leftsplit.getwidth(), y_top - scene.carmera.y + HEIGHT / 2, &im_leftsplit);
		}
		if (attmuxR == 1) {
			putimagePng(x_right - scene.carmera.x + WIDTH / 2, y_top - scene.carmera.y + HEIGHT / 2, &im_rightsplit);
		}
		if (attmuxD == 1) {
			putimagePng(x_left - scene.carmera.x + WIDTH / 2, y_bottom - scene.carmera.y + HEIGHT / 2, &im_downsplit);
		}
		for (int i = 0; i < life; i++) {
			putimagePng(50 * i, 0, &im_life[i]);// carmera.y-HEIGHT/2+50跟随相机
		}
		//cout << x_left - scene.carmera.x + WIDTH / 2<<"    " << y_top - scene.carmera.y + HEIGHT / 2 <<"   " <<animId<< endl;  测试1
	}
	void updateXY(float xm, float ym) {
		x_left = xm;
		y_top = ym;
	}
	void walkLeft(Scene& scene) {
		if (isNotOnAllFloor(scene.floors, vy)) { //如果离开地面
			//im_show = im_JumpStartL[0];
			//playerStatus = jumpleftStart;// 切换到向左起跳状态
			jump(0);
			if (collsionFloors(scene) == 0) {
				x_left -= (2*vx / 3);
				x_right -= (2*vx / 3);
			}
			if (x_left <= 0) {
				x_left = 0;
				x_right = x_left + im_show.getwidth();
			}
			if (jumpmux == 1) {
				if (playerStatus == jumprightStart) {
					playerStatus = jumpleftStart;
				}
				if (playerStatus == jumprightLoop) {
					playerStatus = jumpleftLoop;
				}
				if (playerStatus == jumprightLand) {
					playerStatus = jumpleftLand;
				}
			}
			return;
		}

		else if (jumpmux == 0) {

			if (playerStatus != runleft) {
				playerStatus = runleft; // 切换为向右奔跑状态
				animId = 0; // 动画播放id初始化为0
			}
			else {
				animId++; // 动画图片开始切换
				if (animId >= 5) // 循环切换
					animId = 0;
			}
			im_show = im_walkL[animId];
		}
		if (collsionFloors(scene) == 0) {
			x_left -= vx;
			x_right -= vx;
		}
		if (x_left <= 0) {
			x_left = 0;
			x_right =x_left+ im_show.getwidth();
		}
	}
	void walkRight(Scene& scene) {
		if (isNotOnAllFloor(scene.floors, vy)) { //如果离开地面
			jump(0);
			if (collsionFloors(scene) == 0) {
				x_left += (2*vx / 3);
				x_right += (2*vx / 3);
			}
			if (x_right >= scene.bk_w) {
				x_right = scene.bk_w;
				x_left = x_right - im_show.getwidth();
			}
			if (jumpmux == 1) {
				if (playerStatus == jumpleftStart) {
					playerStatus = jumprightStart;
				}
				if (playerStatus == jumpleftLoop) {
					playerStatus = jumprightLoop;
				}
				if (playerStatus == jumpleftLand) {
					playerStatus = jumprightLand;
				}
			}
			return;
		}
		else if (jumpmux == 0) {
			if (playerStatus != runright) {
				playerStatus = runright; // 切换为向右奔跑状态
				animId = 0; // 动画播放id初始化为0
			}
			else {
				animId++; // 动画图片开始切换
				if (animId >= 5) // 循环切换
					animId = 0;
			}
			im_show = im_walkR[animId];
		}

		if (collsionFloors(scene) == 0) {
			x_left += vx;
			x_right += vx;
		}
		if (x_right >= scene.bk_w) {
			x_right = scene.bk_w;
			x_left = x_right - im_show.getwidth();
		}
	}
	void dashLeft() {

	}
	void dashRight() {

	}
	void attack(Scene& scene) {
		/*列出所有向左的可能*/
		if (playerStatus == runleft || playerStatus == standleft || playerStatus == jumpleftStart || playerStatus == jumpleftLoop || playerStatus == jumpleftLand)//||playerStatus==attackleft)
		{
			if (attmuxL == 0 && attmuxR == 0 && attmuxD == 0) {
				attmuxL = 1;
				attmuxR = 0;
				attmuxD = 0;
				attId = -1;
				//frontstatus = playerStatus;
				int ord_i = isAttacked(scene);
				if (ord_i != -1&&scene.level==1) {
					scene.enemys[ord_i].hurt(hur);
					doAttackL = 1;
					doAttackR = 0;
					effect_x = scene.enemys[ord_i].left_x;
					effect_y = scene.enemys[ord_i].top_y;
				}
				if (ord_i != -1 && scene.level == 2) {
					scene.boss.hurt(hur);
					doAttackL = 1;
					doAttackR = 0;
					effect_x = scene.boss.x_left;
					effect_y = scene.boss.y_top;
				}
			}
		}
		else if (playerStatus == runright || playerStatus == standright || playerStatus == jumprightStart || playerStatus == jumprightLoop || playerStatus == jumprightLand)// || playerStatus == attackright)
		{
			if (attmuxL == 0 && attmuxR == 0 && attmuxD == 0) {
				attmuxL = 0;
				attmuxD = 0;
				attmuxR = 1;
				attId = -1;
				//frontstatus = playerStatus;
				int ord_i = isAttacked(scene);
				if (ord_i != -1 && scene.level == 1) {
					scene.enemys[ord_i].hurt(hur);
					doAttackR = 1;
					doAttackL = 0;
					effect_x = scene.enemys[ord_i].left_x;
					effect_y = scene.enemys[ord_i].top_y;
				}
				if (ord_i != -1 && scene.level == 2) {
					scene.boss.hurt(hur);
					doAttackR = 1;
					doAttackL = 0;
					effect_x = scene.boss.x_left;
					effect_y = scene.boss.y_top;
				}
			}
		}
	}
	void attackDU(Scene& scene) {
		if (attmuxL == 0 && attmuxR == 0 && attmuxD == 0) {
			attmuxD = 1;
			attmuxL = 0;
			attmuxR = 0;
			attId = -1;
			int ord_i = isAttacked(scene);
			if (ord_i != -1 && scene.level == 1) {
				scene.enemys[ord_i].hurt(hur);
				doAttackR = 1;//借用right的effect
				doAttackL = 0;
				effect_x = scene.enemys[ord_i].left_x;
				effect_y = scene.enemys[ord_i].top_y;
			}
			if (ord_i != -1 && scene.level == 2) {
				scene.boss.hurt(hur);
				doAttackR = 1;
				doAttackL = 0;
				effect_x = scene.boss.x_left;
				effect_y = scene.boss.y_top;
			}
		}
	}
	void attackLeft() {
		//if (playerStatus!=attackleft) {
		if (attId == -1) {
			/*animId = 0;
			playerStatus = attackleft;*/
			attId++;
			/*im_show = im_attackL[animId++];*/
			im_show = im_attackL[attId++];
		}
		else {
			im_show = im_attackL[attId];
			attId++;
			if (attId >= 4) {
				attId = -1;
				//im_show = im_idleL;
				//playerStatus = standleft;
				attmuxL = 0;
				//playerStatus = frontstatus;
			}
		}
	}
	void attackRight() {
		//if (playerStatus != attackright) {
		if (attId == -1) {
			attId++;
			im_show = im_attackR[attId++];
		}
		else {
			im_show = im_attackR[attId];
			attId++;
			if (attId >= 4) {
				attId = -1;
				//im_show = im_idleR;
				//playerStatus = standright;
				attmuxR = 0;
				//playerStatus = frontstatus;
			}
		}
	}
	void attackDown() {
		//cout << playerStatus << "   " << attId <<"  "<<vy << endl;
		if (attId == -1) {
			attId++;
			im_show = im_attackD[attId++];
		}
		else {
			im_show = im_attackD[attId];
			attId++;
			if (attId >= 4) {
				attId = -1;
				attmuxD = 0;
			}
		}
	}
	int isAttacked(Scene& scene) {
		if (attmuxL == 1) {
			if (scene.level == 1) {
				for (int i = 0; i < scene.enemys.size(); i++) {
					if (x_left - scene.enemys[i].right_x <= 130 && x_left - scene.enemys[i].right_x >= 0 && abs(y_top - scene.enemys[i].top_y) <= 60) {//150为split的宽度
						effectId = 0;
						return i;
					}
				}
			}
			if (scene.level == 2) {
				if (x_left - scene.boss.x_right <= 130 && x_left - scene.boss.x_right >= 0 && abs(y_top - scene.boss.y_top) <= 60) {//150为split的宽度
					effectId = 0;
					return 1;
				}
			}
		}
		if (attmuxR == 1) {
			if (scene.level == 1) {
				for (int i = 0; i < scene.enemys.size(); i++) {
					if (scene.enemys[i].left_x - x_right <= 130 && scene.enemys[i].left_x - x_right >= 0 && abs(y_top - scene.enemys[i].top_y) <= 60) {
						effectId = 0;
						return i;
					}
				}
			}
			if (scene.level == 2) {
				if (scene.boss.x_left - x_right <= 130 && scene.boss.x_left - x_right >= 0 && abs(y_top - scene.boss.y_top) <= 60) {
					effectId = 0;
					return 1;
				}
			}
			}
		if (attmuxD == 1) {
				if (scene.level == 1) {
					for (int i = 0; i < scene.enemys.size(); i++) {
						if ((abs(x_left - scene.enemys[i].left_x) <= 90 || abs(x_right - scene.enemys[i].right_x) <= 90) && scene.enemys[i].top_y - y_bottom <= 130 && scene.enemys[i].top_y - y_bottom >= 0) {
							effectId = 0;
							vy = -35;
							return i;
						}
					}
				}
				if (scene.level == 2) {
					if ((abs(x_left - scene.boss.x_left) <= 90 || abs(x_right - scene.boss.x_right) <= 90) && scene.boss.y_top - y_bottom <= 130 && scene.boss.y_top - y_bottom >= 0) {
						effectId = 0;
						vy = -35;
						return 1;
					}
				}
			}
		return -1;
	}
	void jump(int c) {
		if (playerStatus != jumpleftStart && playerStatus != jumprightStart &&
			playerStatus != jumpleftLoop && playerStatus != jumprightLoop &&
			playerStatus != jumpleftLand && playerStatus != jumprightLand) // 已经在空中的话，不要起跳
		{
			/*jumpmux = 1;*/
			if (jumpmux == 0) {
				animId = 0;
				jumpmux = 1;
				if (playerStatus == runleft || playerStatus == standleft)//|| playerStatus == attackleft)  // 起跳前是向左跑或向左站立状态
				{
					playerStatus = jumpleftStart; // 切换到向左起跳状态
				}
				else if (playerStatus == runright || playerStatus == standright)//|| playerStatus == attackright)// 起跳前是向右跑或向右站立状态
				{
					playerStatus = jumprightStart;// 切换到向右起跳状态
				}
				vy = c; // 给角色一个向上的初速度
			}
		}
	}

	void standStill() {
		if (playerStatus == runleft || playerStatus == standleft)//||playerStatus == attackleft)
		{
			im_show = im_idleL;
		}
		else if (playerStatus == runright || playerStatus == standright)//||playerStatus == attackright)
		{
			im_show = im_idleR;
		}
	}
	void collsionEnemys(Scene& scene) {
		for (int i = 0; i < scene.enemys.size(); i++) {
			//if ((abs(x_left - scene.enemys[i].right_x) <= 15|| abs(x_right - scene.enemys[i].left_x)<=15) && abs(y_top - scene.enemys[i].top_y) <= 50&&abs(y_bottom - scene.enemys[i].top_y)<=50 && alive == 0) {//闪烁计时
			if (alive == 0 && x_left< scene.enemys[i].right_x && x_right>scene.enemys[i].left_x &&
				y_top <= scene.enemys[i].bottom_y && y_bottom >= scene.enemys[i].top_y) {
				if (death == 0) {
					life--;
					alive = 24;
				}
				break;
			}
		}
		for (int i = 0; i < 5; i++) {
			scene.trip[i].trick(x_left);
			if (alive == 0 && x_left< scene.trip[i].x_right && x_right>scene.trip[i].x_left &&
				y_top <= scene.trip[i].y_bottom && y_bottom >= scene.trip[i].y_top) {
				if (death == 0) {
					life--;
					alive = 24;
				}
				//break;
			}
		}
		if (life <= 0) {
			life = 0;
			death = 1;
			animId = 0;
		}
	}
	void collosionBoss(Boss& boss) {
		if (alive == 0 && boss.floorsprintMux == 1) {
			if (boss.direction == 1) {
				if (x_left <= boss.x_right && x_right >= (boss.x_left + boss.w / 2) && y_top <= boss.y_bottom && y_bottom >= boss.y_top) {
					if (death == 0) {
						life--;
						alive = 24;
					}
				}
			}
			if ( boss.direction == -1) {
				if (x_left <= boss.x_right- boss.w / 2 && x_right >= (boss.x_left) && y_top <= boss.y_bottom && y_bottom >= boss.y_top) {
					if (death == 0) {
						life--;
						alive = 24;
					}
				}
			}
		}
		if (alive == 0 && boss.jumpsprintMux == 1) {
			if (x_left <= boss.x_right && x_right >= (boss.x_left ) && y_top <= boss.y_bottom && y_bottom >= boss.y_top+boss.h/2) {
				if (death == 0) {
					life--;
					alive = 24;
				}
			}
		}
		if (alive == 0 && boss.throwMux == 1) {
			if (boss.direction == 1) {
				if (x_left <= boss.sword.x_right && x_right >= (boss.sword.x_left + boss.sword.w / 2) && y_top <= boss.sword.y_bottom && y_bottom >= boss.sword.y_top) {
					if (death == 0) {
						life--;
						alive = 24;
					}
				}
			}
			if (boss.direction == -1) {
				if (x_left <= boss.sword.x_right - boss.sword.w / 2 && x_right >= (boss.sword.x_left) && y_top <= boss.sword.y_bottom && y_bottom >= boss.sword.y_top) {
					if (death == 0) {
						life--;
						alive = 24;
					}
				}
			}
		}
		if (life <= 0) {
			life = 0;
			death = 1;
			animId = 0;
		}
	}
	int collsionFloor(Floor& floor) {
		x_right = x_left + im_show.getwidth();
		y_bottom = y_top + im_show.getheight();
		width = im_show.getwidth();
		height = im_show.getheight();
		if (abs(x_right - floor.left_x) < 5 + vx && y_top<(floor.top_y + floor.floor_height) 
			&& y_bottom>floor.top_y + 5 && playerStatus == runright) {//向右
			x_right = floor.left_x;
			x_left = x_right - width;
			return 1;
		}
		if (abs(x_left - floor.right_x) < 5 + vx && y_top < (floor.top_y + floor.floor_height)
			&& y_bottom > floor.top_y + 5 && playerStatus == runleft) {//向左
			x_left = floor.right_x;
			x_right = x_left + width;
			return 1;
		}
		return 0;
	}
	int isNotOnAllFloor(vector<Floor>& floors, float vy) {
		for (int i = 0; i < floors.size(); i++) {
			if (isOnFloor(floors[i], vy)) {
				return 0;// 在任何一块地面上，返回0
			}

		}
		return 1;// 不在所有地面上，返回1
	}
	int isOnFloor(Floor& floor, float vy) {
		x_right = x_left + im_show.getwidth();
		y_bottom = y_top + im_show.getheight();
		width = im_show.getwidth();
		height = im_show.getheight();
		if (vy <= 0) {
			vy = 0;
		}
		if (floor.left_x - x_left <= width * 0.6 && x_right - floor.right_x <= width * 0.6 && abs(y_bottom - floor.top_y) <= 5 + vy) {
			return 1;
		}
		return 0;
	}
	int ifDeath() {
		if (death == 1) {
			if (animId >= 9) {
				animId = 9;
				return 1;
			}
			im_show = im_death[animId];
			animId++;
		}
		return 0;
	}
	void updateYcoordinate(Scene& scene) {//自动实时更新
		if (scene.level == 1) {
			if (y_top >= scene.bk_h) {
				if (life <= 0) {
					life = 0;
					death = 1;
					animId = 0;
				}
				else {
					life--;
					alive = 24;
				}
				if (x_left >=scene.spawnpointx[0]&&x_left<scene.spawnpointx[1]) {
					x_left = scene.spawnpointx[0];
					y_bottom = scene.spawnpointy[0];
					y_top = scene.spawnpointy[0]-im_show.getheight();
					x_right = x_left + im_show.getwidth();
				}
				if (x_left >= scene.spawnpointx[1] && x_left < scene.spawnpointx[2]) {
					x_left = scene.spawnpointx[1];
					y_bottom = scene.spawnpointy[1];
					y_top = scene.spawnpointy[1] - im_show.getheight();
					x_right = x_left + im_show.getwidth();
				}
				if (x_left >= scene.spawnpointx[2]) {
					x_left = scene.spawnpointx[2];
					y_bottom = scene.spawnpointy[2];
					y_top = scene.spawnpointy[2] - im_show.getheight();
					x_right = x_left + im_show.getwidth();
				}
			}

		}
		if (playerStatus == jumpleftStart || playerStatus == jumprightStart ||
			playerStatus == jumpleftLoop || playerStatus == jumprightLoop)// 当前是在空中跳跃状态
		{
			vy += gravity; // y方向速度受重力影响变化
			y_bottom += vy;  // y方向位置受速度影响变化
			y_top += vy;
			if (playerStatus == jumpleftStart || playerStatus == jumprightStart) {
				if (playerStatus == jumpleftStart) {
					im_show = im_JumpStartL[animId];
					if (animId >= 8) {
						playerStatus = jumpleftLoop;
						im_show = im_JumpLoopL;
					}
				}
				if (playerStatus == jumprightStart) {
					im_show = im_JumpStartR[animId];
					if (animId >= 8) {
						playerStatus = jumprightLoop;
						im_show = im_JumpLoopR;
					}
				}
				animId++;
			}
		}
		if (playerStatus == jumpleftLoop) {
			im_show = im_JumpLoopL;
		}
		if (playerStatus == jumprightLoop) {
			im_show = im_JumpLoopR;
		}
		for (int i = 0; i < scene.floors.size(); i++)   // 对所有地面遍历
		{
			if (isOnFloor(scene.floors[i], vy)) // 当火柴人正好站在一个地面上时
			{
				y_bottom = scene.floors[i].top_y-5; // 保证正好落在地面上
				y_top = y_bottom - im_show.getheight();
				if (playerStatus != jumpleftLand && playerStatus != jumprightLand) {
					if (playerStatus == jumpleftStart || playerStatus == jumpleftLoop) {
						playerStatus = jumpleftLand;
						animId = 0;
						//im_show = im_JumpLandL[animId++];
					}
					if (playerStatus == jumprightStart || playerStatus == jumprightLoop) {
						playerStatus = jumprightLand;
						animId = 0;
						//im_show = im_JumpLandR[animId++];
					}
				}
				if (playerStatus == jumpleftLand) {
					im_show = im_JumpLandL[animId];
					animId++;
					if (animId >= 3) {
						animId = 0;
						im_show = im_idleL;
						playerStatus = standleft;
						jumpmux = 0;

					}
				}
				if (playerStatus == jumprightLand) {
					im_show = im_JumpLandR[animId];
					animId++;
					if (animId >= 3) {
						animId = 0;
						im_show = im_idleR;
						playerStatus = standright;
						jumpmux = 0;
						//cout << "降落" <<jumpmux<< endl;
					}
				}
				//break; // 跳出循环，不需要再对其他地面判断了
				return;
			}
		}
		if ((playerStatus == jumpleftLand || playerStatus == jumprightLand)) {// && vy != 0) {//防止出现land但是腾空
			vy += gravity; // y方向速度受重力影响变化
			y_bottom += vy;  // y方向位置受速度影响变化
			y_top += vy;
		}
	}

	void updateAttack(Scene& scene) {
		if (scene.playerUpdate == 1 && mutexUpdate == 0) {
			loadimage(&im_leftsplit, _T("images\\player\\Attack\\Attack\\1\\1_1.png"));
			loadimage(&im_rightsplit, _T("images\\player\\Attack\\AttackR\\2_1.png"));
			loadimage(&im_downsplit, _T("images\\player\\Attack\\AttackDown\\3_1.png"));
			hur++;
			mutexUpdate = 1;
		}
		if (scene.playerUpdate == 2 && mutexUpdate == 1) {
			life += 2;
			mutexUpdate = 2;
		}
		if (attmuxL == 1 && attmuxD == 0) {
			attackLeft();
		}
		else if (attmuxR == 1 && attmuxD == 0) {
			attackRight();
		}
		if (attmuxD == 1) {
			attackDown();
		}
		if (doAttackL == 1) {
			if (effectId >= 5) {
				effectId = 0;
				doAttackL = 0;
			}
			im_effect = im_attackeffectL[effectId];
			effectId++;
		}
		else if (doAttackR == 1) {
			if (effectId >= 5) {
				effectId = 0;
				doAttackR = 0;
			}
			im_effect = im_attackeffectR[effectId];
			effectId++;
		}

		//升级
		
	}

	int collsionFloors(Scene& scene) {
		for (int i = 0; i < scene.floors.size(); i++) {
			if (collsionFloor(scene.floors[i])) {
				//cout << "Collision detected at floor " << i <<endl;
				return 1;
			}
		}
		return 0;
	}
	int uplevel(Scene& scene) {
		if (x_left >= scene.bk_w-200) {
			return 1;
		}
		return 0;
	}
};

Scene scene,scene2;
Player player;
Timer timer;  // 用于精确延时
int level2,startgame;
IMAGE startBK;

void updateWithoutInput(Scene& s) {// 与输入无关的更新
	player.updateYcoordinate(s);
	player.updateAttack(s);
	if (level2 == 0) {
		s.enemyMove(player.x_left, player.x_right, player.y_top, player.y_bottom);
		player.collsionEnemys(s);
		//player.ifDeath();
	}
	if (level2 == 1) {
		s.boss.statusChange(player.x_left);
		s.boss.run();
		s.boss.standStill();
		s.boss.sprintFloor();
		s.boss.sprintJump(player.x_left);
		s.boss.throwSword();
		s.boss.jump();
		player.collosionBoss(s.boss);
		//player.ifDeath();
	}
	if (level2 == 0&&player.uplevel(s) == 1) {
		cleardevice();
		level2++;
		srand(time(0));
		scene2.init2();
		player.life += 2;
		player.updateXY(10, 450);
		//1.run测试
		//scene2.boss.updateXY(600);
		//scene2.boss.runMux = 1;
		BeginBatchDraw();
	}
}
void updateWithInput(Scene& s) {    // 与输入有关的更新
	player.standStill();
	if (_kbhit()) {
		if (GetAsyncKeyState('D')) {
			player.walkRight(s);
		}
		if (GetAsyncKeyState('A')) {
			player.walkLeft(s);
		}
		if (GetAsyncKeyState('W')) {
			player.jump(-45);
		}
		if (GetAsyncKeyState('H')) {
			if (GetAsyncKeyState('S')) {
				player.attackDU(s);
			}
			else {
				player.attack(s);
			}
		}
	}
}

void startup() {
	srand(time(0));
	level2 = 0;
	scene.init();
	//cout << 1 << endl;
	player.init();
	mciSendString(_T("open rain.mp3 alias bkmusic"), NULL, 0, NULL);
	//play music
	mciSendString(_T("play bkmusic repeat"), NULL, 0, NULL);
	/*initgraph(WIDTH, HEIGHT);*/
	BeginBatchDraw();
}
void show(Scene& s) {
	if (level2 == 0) {
		s.draw();
	}
	if (level2 == 1) {
		s.draw2();
	}

	player.draw(s);
	FlushBatchDraw();
	timer.Sleep(50);
}

int main() {
	//cout << 1 << endl;
	initgraph(WIDTH, HEIGHT);
	loadimage(&startBK, _T("images//Title.png"));
	putimage(0, 0, &startBK);
	startgame = 0;
	while(1&&startgame==0){
		if (GetAsyncKeyState(' ')) {
			startgame = 1;
			cleardevice();
			startup();
			//player.updateXY(14000, 40);
			while (1) {
				if (player.death != 1) {
					if (level2 == 0) {
						show(scene);
						updateWithInput(scene);
						updateWithoutInput(scene);
					}
					if (level2 == 1) {
						show(scene2);
						updateWithInput(scene2);
						updateWithoutInput(scene2);
						if (scene2.boss.win == 1) {
							show(scene2);
							setbkmode(TRANSPARENT);
							settextstyle(80, 30, _T("Arial"));
							outtextxy(400, 300, _T("WIN"));
							FlushBatchDraw();
							Sleep(4000);
							break;
						}
					}
				}
				else {
					if (level2 == 0) {
						show(scene);
					}
					if (level2 == 1) {
						show(scene2);
					}
					if (player.ifDeath() == 1) {
						setbkmode(TRANSPARENT);
						settextstyle(80, 30, _T("Arial  "));
						outtextxy(400, 300, _T("LOSE"));
						FlushBatchDraw();
						Sleep(4000);
						break;
					}
				}

			}
		}
	}
	_getch();
	closegraph();
}