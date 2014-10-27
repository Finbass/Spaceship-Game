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


int Distance( int x1, int y1, int x2, int y2) {
	return sqrt((float)(x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
}

Point Midpoint( Point p1, Point p2) {
	Point p3;
	p3.x = (p1.x + p2.x) / 2;
	p3.y = (p1.y + p2.y) / 2;
	return p3;
	}

Game::Game( HWND hWnd,const KeyboardServer& kServer,const MouseServer& mServer )
:	gfx( hWnd ),
	audio( hWnd ),
	kbd( kServer ),
	mouse( mServer ),
	x (200),
	y (300),
	swap (false),
	speed (8),
	x2 (498),
	y2 (400),
	//bulletX (x),
	//bulletX2 (x),
	//bulletY (y),
	//bulletY2 (y),
	ballCount (0),
	ballRad (30),
	ballX (400),
	ballY (100),
	//noOfBulletsOnScreen (0),
	ballHit (false)
	//noBullets (true)
{
	frameCount = 0;
	changeDirection = false;
	clockwise = true;

	xVel = 0;
	yVel = 0;

	boss.lives = 50;
	boss.x = 200;
	boss.y = 300;

	LoadSprite( &ship3, "Ship 3.bmp", 150, 150, D3DCOLOR_XRGB( 0, 0, 0));
}

Game::~Game() {
	FreeSprite(&ship3);
}

void Game::Go()
{
	bulletLogic();
	updateBall();
	updateEnemy();

	gfx.BeginFrame();
	ComposeFrame();
	gfx.EndFrame();

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
		for (int i = 0; i < amountOfArrays; i++) {
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
			ballX = rand() % (800 - ballRad);
			ballY = rand() % (600 - ballRad);
			if (ballX > ballRad && ballY > ballRad && ballRad > 14) break;
			}
		ballCount++;
		//liveBullet1 = false;
		//bulletX = x;
		//bulletY = y;
		ballHit =  false;
	}
}

void Game::updateEnemy() {
	for (int i = 0; i < amountOfArrays; i++) {
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
	/*bullet bullet1;
	bullet1.x = x;
	bullet1.y = y;
	bullet.r = rand() % 255;
	bullet.g = rand() % 255;
	bullet.b = rand() % 255;
	bullet1.r = 0;
	bullet1.g = 255;
	bullet1.b = 0;

	//try different color patterns here
	if (bullet.r < 255 ) bullet.r += 5;
	else bullet.r -= 5;
	if (bullet.g < 255 ) bullet.g += 3;
	else bullet.g -= 5;
	if (bullet.b < 255 ) bullet.b += 7;
	else bullet.b -= 5;
	bulletArray.push_back(bullet1);

	/*bullet bullet2;
	bullet2.x = x;
	bullet2.y = y;
	bullet2.r = rand() % 255;
	bullet2.g = rand() % 255;
	bullet2.b = rand() % 255;

	bulletArray2.push_back(bullet2);

	bullet bullet3;
	bullet3.x = x;
	bullet3.y = y;
	bullet3.r = rand() % 255;
	bullet3.g = rand() % 255;
	bullet3.b = rand() % 255;

	bulletArray3.push_back(bullet3);

	bullet bullet4;
	bullet4.x = x;
	bullet4.y = y;
	bullet4.r = rand() % 255;
	bullet4.g = rand() % 255;
	bullet4.b = rand() % 255;

	bulletArray4.push_back(bullet4);

	bullet bullet5;
	bullet5.x = x;
	bullet5.y = y;
	bullet5.r = rand() % 255;
	bullet5.g = rand() % 255;
	bullet5.b = rand() % 255;

	bulletArray5.push_back(bullet5);*/

	/*for (int i = 0; i < amountOfArrays; i++) {
		bullet bullet1;
		bullet1.x = x;
		bullet1.y = y;
		bullet1.r = rand() % 255;
		bullet1.g = rand() % 255;
		bullet1.b = rand() % 255;
		//bullet1.r = 0;
		//bullet1.g = 255;
		//bullet1.b = 0;
		//bullet1.vx = rand() % XvelocityRand - (XvelocityRand / 2);
		//bullet1.vy = rand() % YvelocityRand - (YvelocityRand / 2);
		int cursorX = mouse.GetMouseX();
		int cursorY = mouse.GetMouseY();
		//int cursorX = 100;
		//int cursorY = 100;

		int deltaY = y - cursorY;
		int deltaX = x - cursorX;
		float slope = deltaY / deltaX;

		float angle = atan(slope);

		bullet1.vx = cos(angle) * 10;
		bullet1.vy = sin(angle) * 10;

		//bullet1.vx = 0;
		//bullet1.vy = - 10;
		array1[i].push_back(bullet1);
	}*/

	for (int i = 0; i < amountOfArrays; i++) {
		bullet bullet1;
		int x = 100;
		int y = 100;
		bullet1.x = x;
		bullet1.y = y;
		int cursorX = 50;
		int cursorY = 50;

		int deltaY = y - cursorY;
		int deltaX = x - cursorX;
		float slope = deltaY / deltaX;

		float angle = atan(slope);

		bullet1.vx = cos(angle) * 10;
		bullet1.vy = sin(angle) * 10;

		//bullet1.vx = 0;
		//bullet1.vy = - 10;
		array1[i].push_back(bullet1);
	}
}

void Game::bulletLogic() {
	if (kbd.SpaceIsPressed() /*&& !keysPressedLastFrame*/) {
		createBullet();
		keysPressedLastFrame = true;
	}

	if (!(kbd.SpaceIsPressed())) keysPressedLastFrame = false;

	/*if (frameCount % 10  == 0) {
		if (changeDirection) changeDirection = false;
		else changeDirection = true;
	}

	for (int i = 0; i < bulletArray.size(); i++) {
		bulletArray[i].y -= 5;
		if (changeDirection) bulletArray[i].x += 5;
		else bulletArray[i].x -= 5;
		if (bulletArray[i].y < 5) {
			bulletArray.erase(bulletArray.begin() + i);
		}
	}

	for (int i = 0; i < bulletArray2.size(); i++) {
		bulletArray2[i].y -= 5;
		bulletArray2[i].x += 5;
		if (bulletArray2[i].y < 5) {
			bulletArray2.erase(bulletArray2.begin() + i);
		}
	}

	for (int i = 0; i < bulletArray3.size(); i++) {
		bulletArray3[i].y -= 5;
		bulletArray3[i].x -= 5;
		if (bulletArray3[i].y < 5) {
			bulletArray3.erase(bulletArray3.begin() + i);
		}
	}

	for (int i = 0; i < bulletArray4.size(); i++) {
		bulletArray4[i].y += 5;
		bulletArray4[i].x -= 5;
		if (bulletArray4[i].y > 595) {
			bulletArray4.erase(bulletArray4.begin() + i);
		}
	}

	for (int i = 0; i < bulletArray5.size(); i++) {
		bulletArray5[i].y += 5;
		bulletArray5[i].x += 5;
		if (bulletArray5[i].y > 595) {
			bulletArray5.erase(bulletArray5.begin() + i);
		}
	}*/

	//swiper/sprinkler motion!!!!
/*	if (xVel <= 10 && clockwise) xVel += 0.5;
	else if (xVel > - 10 && !clockwise) xVel -= 0.5;
	
	if (xVel >= 10) clockwise = false;
	else if (xVel <= -10) clockwise = true;

	//if (yVel < 30) yVel += 5;
	yVel = -2;*/


	for (int i = 0; i < amountOfArrays; i++) {
		for (int j = 0; j < array1[i].size(); j++) {
			//array1[i].at(j).vx += xVel;
			//array1[i].at(j).vy += yVel;
			array1[i].at(j).x += array1[i].at(j).vx;
			array1[i].at(j).y += array1[i].at(j).vy;
			if (array1[i].at(j).y > 595 || array1[i].at(j).y < 5 || array1[i].at(j).x > 795 || array1[i].at(j).x < 5) {
				
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
	if (kbd.RightIsPressed()) {
		x = x + speed;
	}

	if (kbd.LeftIsPressed()) {
		x = x - speed;
	}

	if (kbd.UpIsPressed()) {
		y = y - speed;
	}

	if (kbd.DownIsPressed()) {
		y = y + speed;
	}

	//if (mouse.GetMouseX() >= TOPLEFTX && mouse.GetMouseX() <= BOTTOMRIGHTX && mouse.GetMouseY() >= TOPLEFTY && mouse.GetMouseY() <= BOTTOMRIGHTY) {
		//gfx.DrawDisc(mouse.GetMouseX(), mouse.GetMouseY(), 5, 0, 255, 0);
	//}

	Point player, mid, mouse1;
	player.x = x;
	player.y = y;
	mouse1.x = mouse.GetMouseX();
	mouse1.y = mouse.GetMouseY();
	mid = Midpoint(player, mouse1);
	gfx.DrawDisc(mid.x, mid.y, 5, 0, 255, 0);

	//boundary conditions for all
	if (x > 1900 - 150) x = 1900 - 150;
	if (x < 5) x = 5;
	if (y > 1080 - 150) y = 1080 - 150;
	if (y < 5) y = 5;

	//patrolling bot AI
	/*if (y2 == 200) x2 = x2 + 2;
	if (x2 == 500) y2 = y2 + 2;
	if (y2 == 400) x2 = x2 - 2;
	if (x2 == 300) y2 = y2 - 2;*/
	if (x2 > x) x2 = x2 - 2;
	else x2 = x2 + 2;
	if (y2 > y) y2 = y2 - 2;
	else y2 = y2 + 2;

	drawBall(ballX, ballY, ballRad, 255, 255, 255);

	/*for (int i = 0; i < NOOFBULLETS; i++) {
		if (bulletArray[i].live) drawReticle(bulletArray[i].x, bulletArray[i].y, 0, 255, 0);
	}*/

	for (int i = 0; i < bulletArray.size(); i++) {
		drawReticle(bulletArray[i].x, bulletArray[i].y, bulletArray[i].r, bulletArray[i].g, bulletArray[i].b);
	}

	for (int i = 0; i < bulletArray2.size(); i++) {
		drawReticle(bulletArray2[i].x, bulletArray2[i].y, bulletArray2[i].r, bulletArray2[i].g, bulletArray2[i].b);
	}

	for (int i = 0; i < bulletArray3.size(); i++) {
		drawReticle(bulletArray3[i].x, bulletArray3[i].y, bulletArray3[i].r, bulletArray3[i].g, bulletArray3[i].b);
	}

	for (int i = 0; i < bulletArray4.size(); i++) {
		drawReticle(bulletArray4[i].x, bulletArray4[i].y, bulletArray4[i].r, bulletArray4[i].g, bulletArray4[i].b);
	}

	for (int i = 0; i < bulletArray5.size(); i++) {
		drawReticle(bulletArray5[i].x, bulletArray5[i].y, bulletArray5[i].r, bulletArray5[i].g, bulletArray5[i].b);
	}

	for (int i = 0; i < amountOfArrays; i++) {
		for (int j = 0; j < array1[i].size(); j++) {
			drawReticle(array1[i].at(j).x, array1[i].at(j).y, array1[i].at(j).r, array1[i].at(j).g, array1[i].at(j).b);
		}
	}

	if (boss.lives > 0) drawBall(boss.x, boss.y, 40, 255, 0, 0);

	drawHealth();
	
	/*to do:
		1. Make reticles go in random directions and speeds really fast
		2. make it so your missiles kills the baddies
	*/

	gfx.drawSprite(x, y, &ship3);
	drawReticle(x2, y2, 255, 0, 0);

	//massive 4 point reticle
	gfx.PutPixel( 500,300,255,255,255);
	gfx.PutPixel( 300,300,255,255,255);
	gfx.PutPixel( 400,400,255,255,255);
	gfx.PutPixel( 400,200,255,255,255);
}