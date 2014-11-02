/****************************************************************************************** 
 *	Chili DirectX Framework Version 12.04.24											  *	
 *	Game.cpp																			  *
 *	Copyright 2012 PlanetChili.net														  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "Game.h"
#include <math.h>

LPDIRECT3DTEXTURE9 ship3;

int Distance( int x1, int y1, int x2, int y2) {
	return sqrt((float)(x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
}

Game::Game( HWND hWnd,const KeyboardServer& kServer,const MouseServer& mServer )
:	gfx( hWnd ),
	audio( hWnd ),
	kbd( kServer ),
	mouse( mServer ),
	x (200),
	y (300),
	speed (8),
	x2 (498),
	y2 (400),
	ballRad (30),
	ballX (400),
	ballY (100),
	ballHit (false)
{
	frameCount = 0;
	changeDirection = false;
	clockwise = true;

	rotation = 0.0f;

	xVel = 0;
	yVel = 0;

	boss.lives = 50;
	boss.x = 200;
	boss.y = 300;

	//LoadSprite( &ship3, "Ship 3.bmp", 150, 150, D3DCOLOR_XRGB( 0, 0, 0));
	ship3 = gfx.LoadTexture("Ship 3.bmp");
}

Game::~Game() {
	//FreeSprite(&ship3);
}

void Game::Go()
{

	if (kbd.RightIsPressed()) {
		x = x + speed;
		//rotation += ROTATIONAMOUNT;
	}

	if (kbd.LeftIsPressed()) {
		//rotation -= ROTATIONAMOUNT;
		x = x - speed;
	}

	if (kbd.UpIsPressed()) {
		y = y - speed;
	}

	if (kbd.DownIsPressed()) {
		y = y + speed;
	}

	//boundary conditions for all
	if (x > RIGHTBOUNDARY - 150) x = RIGHTBOUNDARY - 150;
	if (x < LEFTBOUNDARY) x = LEFTBOUNDARY;
	if (y > LOWERBOUNDARY - 150) y = LOWERBOUNDARY - 150;
	if (y < UPPERBOUNDARY) y = UPPERBOUNDARY;

	//patrolling bot AI
	/*if (y2 == 200) x2 = x2 + 2;
	if (x2 == 500) y2 = y2 + 2;
	if (y2 == 400) x2 = x2 - 2;
	if (x2 == 300) y2 = y2 - 2;*/
	if (x2 > x) x2 = x2 - 2;
	else x2 = x2 + 2;
	if (y2 > y) y2 = y2 - 2;
	else y2 = y2 + 2;

	bulletLogic();
	updateBall();
	updateEnemy();

	float cursorX = mouse.GetMouseX();
	float cursorY = mouse.GetMouseY();

	float deltaY = abs(y - cursorY);
	float deltaX = abs(x - cursorX);
	if (deltaX == 0) deltaX = 0.001;
	float slope = deltaY / deltaX;

	rotation = atan(slope);

	//C
	if (cursorX > x + 75 && cursorY < y + 75) rotation = PI / 2 - rotation;
	//A
	if (cursorX < x + 75 && cursorY < y + 75) rotation = 3 * PI / 2 + rotation;
	//S
	if (cursorX < x + 75 && cursorY > y + 75) rotation = 3 * PI / 2 - rotation;
	//T
	if (cursorX > x + 75 && cursorY > y + 75) rotation = rotation + PI / 2;

	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	
	if (pDevice->BeginScene()) {
		gfx.BeginFrame();
		ComposeFrame();
		gfx.EndFrame();
	}

	frameCount++;
}

void Game::drawReticle(int x, int y, int r, int g, int b) {
	gfx.PutPixel(  x      ,y       ,r,g,b);
	gfx.PutPixel( (x + 2) ,y       ,r,g,b);
	gfx.PutPixel( (x + 3) ,y       ,r,g,b);
	gfx.PutPixel( (x - 2) ,y       ,r,g,b);
	gfx.PutPixel( (x - 3) ,y       ,r,g,b);
	gfx.PutPixel( x       ,(y - 2) ,r,g,b);
	gfx.PutPixel( x       ,(y - 3) ,r,g,b);
	gfx.PutPixel( x       ,(y + 2) ,r,g,b);
	gfx.PutPixel( x       ,(y + 3) ,r,g,b);
}

void Game::drawRectangle(int TopCornerX, int TopCornerY, int width, int height, int r, int g, int b) {
	int count1 = 0;
	while (count1 < height) {
		int i = 0;
		r = r + 3;
		g = g + 3;
		b = b + 3;
		for (i = 0; i < width; i++) {
			gfx.PutPixel(TopCornerX + i,TopCornerY + count1,r+(i^3),g+(i^3),b+20*i);
		}
		count1++;
	}
}

void Game::drawBall(int x, int y, int rad, int r, int g, int b) {
	gfx.DrawDisc(x, y, rad, r, g, b);
	}

void Game::drawHealth() {
	for (int y = 0; y < 20; y++) {
		for (int x = 0; x < 50; x++) {
			gfx.PutPixel(x + 730, y + 10, 255, 0, 0);
		}
	}

	for (int y = 0; y < 20; y++) {
		for (int x = 0; x < boss.lives; x++) {
			gfx.PutPixel(x + 730, y + 10, 0, 255, 0);
		}
	}
}

void Game::updateBall() {
	//check if ball is collected
	if (!ballHit) {
		for (int i = 0; i < AMOUNTOFVECTORS; i++) {
			for (int j = 0; j < array1[i].size(); j++) {
				if (array1[i].at(j).x > ballX - ballRad && 
				array1[i].at(j).x < ballX + ballRad &&
				array1[i].at(j).y > ballY - ballRad && 
				array1[i].at(j).y < ballY + ballRad) {
     				array1[i].erase(array1[i].begin() + j);
					ballHit = true;
				}
			}		
		}
	}

	//Move ball somewhere if collected and set back to false
	if (ballHit) {
		while (true) {
			//random size for each ball?
			ballX = rand() % (800 - 2 * ballRad);
			ballY = rand() % (600 - 2 * ballRad);
			if (ballX > ballRad && ballY > ballRad && ballRad > 14) break;
			}
		ballHit =  false;
	}
}

void Game::updateEnemy() {
	for (int i = 0; i < AMOUNTOFVECTORS; i++) {
		for (int j = 0; j < array1[i].size(); j++) {
			if (array1[i].at(j).x > boss.x - 40 && 
				array1[i].at(j).x < boss.x + 40 &&
				array1[i].at(j).y > boss.y - 40 && 
				array1[i].at(j).y < boss.y + 40) {
     				array1[i].erase(array1[i].begin() + j);
					boss.lives--;
					if (boss.lives <= 0) {
						boss.lives = 50;
						boss.x = rand() % 500;
						boss.y = rand() % 500;
					}
			}
		}
	}
}

void Game::createBullet() {
	for (int i = 0; i < AMOUNTOFVECTORS; i++) {
		bullet bullet1;
		bullet1.r = rand() % 255;
		bullet1.g = rand() % 255;
		bullet1.b = rand() % 255;
		bullet1.x = x + 150 / 2;
		bullet1.y = y;
		float cursorX = mouse.GetMouseX();
		float cursorY = mouse.GetMouseY();

		float deltaY = abs(y - cursorY);
		float deltaX = abs(x - cursorX);
		if (deltaX == 0) deltaX = -0.001;
		float slope = deltaY / deltaX;

		float angle = atan(slope);
		int xSign;
		int ySign;

		if (cursorX < x) xSign = -1;
		else xSign = 1;
		if (cursorY < y) ySign = -1;
		else ySign = 1;

		bullet1.vx = cos(angle) * 10 * xSign;
		bullet1.vy = sin(angle) * 10 * ySign;

		array1[i].push_back(bullet1);
	}
}

void Game::bulletLogic() {
	if (mouse.LeftIsPressed() /*&& !keysPressedLastFrame*/) {
		createBullet();
		keysPressedLastFrame = true;
	}

	if (!(kbd.LeftIsPressed())) keysPressedLastFrame = false;

	/*if (frameCount % 10  == 0) {
		if (changeDirection) changeDirection = false;
		else changeDirection = true;
	}

	//swiper/sprinkler motion!!!!
/*	if (xVel <= 10 && clockwise) xVel += 0.5;
	else if (xVel > - 10 && !clockwise) xVel -= 0.5;
	
	if (xVel >= 10) clockwise = false;
	else if (xVel <= -10) clockwise = true;

	//if (yVel < 30) yVel += 5;
	yVel = -2;*/


	for (int i = 0; i < AMOUNTOFVECTORS; i++) {
		for (int j = 0; j < array1[i].size(); j++) {
			//array1[i].at(j).vx += xVel;
			//array1[i].at(j).vy += yVel;
			array1[i].at(j).x += array1[i].at(j).vx;
			array1[i].at(j).y += array1[i].at(j).vy;
			if (array1[i].at(j).y > LOWERBOUNDARY || array1[i].at(j).y < UPPERBOUNDARY || array1[i].at(j).x > RIGHTBOUNDARY || array1[i].at(j).x < LEFTBOUNDARY) {
				//FOR A MOTHERFUCKIN CLUSTERFUCK
				//array1[i].at(j).vx = array1[i].at(j).vx * - 1;
				//array1[i].at(j).vy = array1[i].at(j).vy * - 1;
				
				array1[i].erase(array1[i].begin() + j);
			}
		}	
	}
}

void Game::ComposeFrame()
{	
	gfx.Sprite_Transform_Draw(ship3, x, y, 150, 150, 0, 1, rotation, 1.0f, D3DCOLOR_XRGB(255, 255, 255));

	drawReticle(x2, y2, 255, 0, 0);

	drawBall(ballX, ballY, ballRad, 255, 255, 255);

	if (boss.lives > 0) drawBall(boss.x, boss.y, 40, 255, 0, 0);

	for (int i = 0; i < AMOUNTOFVECTORS; i++) {
		for (int j = 0; j < array1[i].size(); j++) {
			drawReticle(array1[i].at(j).x, array1[i].at(j).y, array1[i].at(j).r, array1[i].at(j).g, array1[i].at(j).b);
		}
	}

	drawHealth();
}