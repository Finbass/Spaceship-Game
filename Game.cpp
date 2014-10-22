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

Game::Game( HWND hWnd,const KeyboardServer& kServer,const MouseServer& mServer )
:	gfx( hWnd ),
	audio( hWnd ),
	kbd( kServer ),
	mouse( mServer )
{
	frameCount = 0;
	wave = 0;
	keysPressedLastFrame = false;
	gameOver = false;
	invincible = false;
	paused = false;

	//sprinkler stuff, stupid maybe
	/*
	changeDirection = false;
	clockwise = true;
	xVel = 0;
	yVel = 0;
	*/

	playerShip.x = 900;
	playerShip.y = 600;
	playerShip.lives = PLAYERSHIPLIVES;
	playerShip.rotation = 0.0f;
	playerShip.framesSinceHit = 0;
	playerShip.ifHit = 0;
	speed = 8.0f;
	weaponChoice = BULLET;

	setUpShips();

	playerShipTexture = gfx.LoadTexture("Player ship 3.bmp", D3DCOLOR_XRGB(0, 0, 0));
	enemyShipTexture = gfx.LoadTexture("Ship 3.bmp");
	insectKamikazeTexture = gfx.LoadTexture("Ship 4.bmp");
	plasmaBullet = gfx.LoadTexture("PlasmaBullet.bmp 2.bmp", D3DCOLOR_XRGB(255, 255, 255));
	testBallTexture = gfx.LoadTexture("Ball.bmp", D3DCOLOR_XRGB(255, 255, 255));

	if (murrayPlaying){
		LEFTBOUNDARY = 5;
		RIGHTBOUNDARY = 1361;
		UPPERBOUNDARY = 5;
		LOWERBOUNDARY = 763;
	}
	else{
		LEFTBOUNDARY = 5;
		RIGHTBOUNDARY = 1915;
		UPPERBOUNDARY = 5;
		LOWERBOUNDARY = 1075;
	}
}

Game::~Game() {
	//get some releases in here for all textures and maybe more
	/*
	
	
	
	
	

	
	*/
}

void Game::Go()
{
	frameCount++;

	

	if (kbd.SpaceIsPressed() && !keysPressedLastFrame) {
		if (paused) paused = false;
		else paused = true;
		keysPressedLastFrame = true;
	}	

	if (!paused) {
		if (!gameOver) {
			playerShipLogic();
			bulletLogic();
			updateEnemy();
		}
	}

	//why isnt this working?
	//wasnt working because there was a separate make false for if enter wasnt pressed in bullet logic
	if (!(kbd.SpaceIsPressed()) && !(kbd.EnterIsPressed())) keysPressedLastFrame = false;

	

	

	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	//2 PROBLEMS:
	//1. Mixing of beginscene/endscene unlock/lock rect causes us unable to draw over texture
	//2. Logic Is updating faster than we draw
	//AHAAA might be able to fix problem 1 by begining and ending the sprite object in the right place. Textures only work when spriteobj has begun
	if (pDevice->BeginScene()) {
		gfx.BeginFrame();
		ComposeFrame();
		gfx.EndFrame();
	}
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
		for (i = 0; i < width; i++) {
			gfx.PutPixel(TopCornerX + i,TopCornerY + count1,r+(i^3),g+(i^3),b+20*i);
		}
		count1++;
	}
}

void Game::drawHealth() {
	const int xBar = 50;
	int yBar;
	if (murrayPlaying) yBar = 700;
	else yBar = 1000;


	//red rectangle
	for (int y = 0; y < 20; y++) {
		for (int x = 0; x < PLAYERSHIPLIVES * 13; x++) {
			gfx.PutPixel(x + xBar, y + yBar, 255, 0, 0);
		}
	}

	//green rectangle
	for (int y = 0; y < 20; y++) {
		for (int x = 0; x < playerShip.lives * 13; x++) {
			gfx.PutPixel(x + xBar, y + yBar, 0, 255, 0);
		}
	}
}

void Game::setUpShips() {
	for (int i = 0; i < SIZEOFENEMYSHIPFLEET; i++) {
		Ship enemyShip;
		enemyShip.ifHit = false;
		enemyShip.framesSinceHit = 0;
		enemyShip.lives = 20;
		enemyShip.y = (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2 + 10;
		//enemyShip.x = rand() % (RIGHTBOUNDARY - ENEMYDIMENSION); //Problem setting up ships since RIGHTBOUNDARY not #defined, fine with hard-coding though
		enemyShip.x = 5 + (i*ENEMYDIMENSION * ENEMYSHIPSCALE);
		enemyShip.vx = 0;
		enemyShip.vy = 1.0f;
		enemyShipFleet.push_back(enemyShip);
	}

	for (int i = 0; i < SIZEOFINSECTFLEET; i++) {
		Ship insect;
		insect.ifHit = false;
		insect.framesSinceHit = 0;
		insect.lives = 15;
		insect.y = (ENEMYDIMENSION * INSECTSCALE) / 2 + 20;
		insect.x = rand() % (1316 - ENEMYDIMENSION);
		insectKamikazeFleet.push_back(insect);
	}
}

void Game::playerShipLogic() {
	if (kbd.RightIsPressed()/* || kbd.dIsPressed()*/) {
		playerShip.x = playerShip.x + speed;
	}

	if (kbd.LeftIsPressed()/* || kbd.aIsPressed()*/) {
		playerShip.x = playerShip.x - speed;
	}

	if (kbd.UpIsPressed()/* || kbd.wIsPressed()*/) {
		playerShip.y = playerShip.y - speed;
	}

	if (kbd.DownIsPressed()/* || kbd.sIsPressed()*/) {
		playerShip.y = playerShip.y + speed;
	}

	//boundary conditions for all
	if (playerShip.x + PLAYERDIMENSION * PLAYERSHIPSCALE > RIGHTBOUNDARY) playerShip.x = RIGHTBOUNDARY - PLAYERDIMENSION * PLAYERSHIPSCALE;
	else if (playerShip.x - PLAYERDIMENSION * PLAYERSHIPSCALE < LEFTBOUNDARY) playerShip.x = LEFTBOUNDARY + PLAYERDIMENSION * PLAYERSHIPSCALE;
	if (playerShip.y + PLAYERDIMENSION * PLAYERSHIPSCALE> LOWERBOUNDARY) playerShip.y = LOWERBOUNDARY - PLAYERDIMENSION * PLAYERSHIPSCALE;
	else if (playerShip.y - PLAYERDIMENSION * PLAYERSHIPSCALE < UPPERBOUNDARY) playerShip.y = UPPERBOUNDARY + PLAYERDIMENSION * PLAYERSHIPSCALE;

	findRotation(playerShip.x, playerShip.y, mouse.GetMouseX(), mouse.GetMouseY(), playerShip.rotation);

	if (playerShip.ifHit) {
		playerShip.framesSinceHit++;
		if (playerShip.framesSinceHit > FRAMESPERFLASH){
			playerShip.framesSinceHit = 0;
			playerShip.ifHit = false;
		}
	}

	//collision with enemy bullets
	if (!invincible) {
		for (int a = 0; a < enemyBulletVector.size(); a++) {
			if (enemyBulletVector[a].x > playerShip.x - (PLAYERDIMENSION * PLAYERSHIPSCALE) / 2 &&
				enemyBulletVector[a].x < playerShip.x + (PLAYERDIMENSION * PLAYERSHIPSCALE) / 2 &&
				enemyBulletVector[a].y > playerShip.y - (PLAYERDIMENSION * PLAYERSHIPSCALE) / 2 &&
				enemyBulletVector[a].y < playerShip.y + (PLAYERDIMENSION * PLAYERSHIPSCALE) / 2)
			{
				enemyBulletVector.erase(enemyBulletVector.begin() + a);
				if (playerShip.lives > 1) {
					playerShip.lives--;
					playerShip.ifHit = true;
					playerShip.framesSinceHit = 0;
				}
				else {
					playerShip.lives--;
					gameOver = true;
				}
			}
		}
	}

	//collision with insects
	if (!invincible) {
		for (int i = 0; i < insectKamikazeFleet.size(); i++) {
			if (insectKamikazeFleet[i].x - (ENEMYDIMENSION * INSECTSCALE) / 2 > playerShip.x - (PLAYERDIMENSION * PLAYERSHIPSCALE) / 2 &&
				insectKamikazeFleet[i].x + (ENEMYDIMENSION * INSECTSCALE) / 2 < playerShip.x + (PLAYERDIMENSION * PLAYERSHIPSCALE) / 2 &&
				insectKamikazeFleet[i].y - (ENEMYDIMENSION * INSECTSCALE) / 2 > playerShip.y - (PLAYERDIMENSION * PLAYERSHIPSCALE) / 2 &&
				insectKamikazeFleet[i].y + (ENEMYDIMENSION * INSECTSCALE) / 2 < playerShip.y + (PLAYERDIMENSION * PLAYERSHIPSCALE) / 2)
			{
				insectKamikazeFleet.erase(insectKamikazeFleet.begin() + i);
				if (playerShip.lives > 1) {
					playerShip.lives--;
					playerShip.ifHit = true;
					playerShip.framesSinceHit = 0;
				}
				else {
					playerShip.lives--;
					gameOver = true;
				}
			}
		}
	}
}

void Game::updateEnemy() {
	//enemy ship rotation, movement, boundaries, bullet and ship collision
	for (int i = 0; i < enemyShipFleet.size(); i++) {
		findRotation(enemyShipFleet[i].x, enemyShipFleet[i].y, playerShip.x, playerShip.y, enemyShipFleet[i].rotation);
		enemyShipFleet[i].x += enemyShipFleet[i].vx;
		enemyShipFleet[i].y += enemyShipFleet[i].vy;

		/*
		**************************************************************************************************************
		
		changes to try(change direction somehow if boundary hit) Talking about enemyship1 (which he is called from now on to avoid ambiguity with the insect with just enemyship itself): 
		1. Make his y = x * x
		2. y = x * x * x, log(x) sqrt(x), sin(x) etc
		3. his y = cos(rotation) and x = y * y
		4. find circle formula (based on centre maybe)
		5. semi circle
		6. form function which takes distance close to player and works dynamic circle  which changes every frame if player moves. Then try semi circle. Work it maybe with angles, or maybe just come circumference follower
		7. Finally work some way finally after doing all that to make him avoid his fellow teammates ships which will be hard. 
			Function that takes in x and y of every teammate within 200-400 pixels and moves in a direction away from them but towards you if possible. Have them go slow so they can easily plan in a group mentality kinda way?
			Maybe have them eventually just go fast and slow. They slow down when clsoe to an enemy ship and eventually move away. They maybe never get too close to an enemy ship because they slow down same with yours.
		8. And separate point as to how it will look if you do eventually collide off an enemy ship. Maybe just bounce off and lose health so player avoids hitting them then. 
		9. make them go in a parade kinda way. or a big s towards you and instead of avoiding you maybe enemyship1s also eventually crash into you but take their time about it. 
			left all the way then small or mediumu turn towards right
	
		
		
		*/

		if (enemyShipFleet[i].x + (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2 > RIGHTBOUNDARY) enemyShipFleet[i].x = RIGHTBOUNDARY - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2;
		else if (enemyShipFleet[i].x - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2 < LEFTBOUNDARY) enemyShipFleet[i].x = LEFTBOUNDARY + (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2;
		if (enemyShipFleet[i].y + (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2 > LOWERBOUNDARY) {
			enemyShipFleet[i].y = LOWERBOUNDARY - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2;
			enemyShipFleet[i].vy *= -1;
		}
		else if (enemyShipFleet[i].y - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2 < UPPERBOUNDARY) {
			enemyShipFleet[i].y = UPPERBOUNDARY + (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2;
			enemyShipFleet[i].vy *= -1;
		}
		/*if (enemyShipFleet[i].y + ENEMYDIMENSION > LOWERBOUNDARY) {
			enemyShipFleet[i].y = enemyShipFleet[i].y - 300;
			enemyShipFleet[i].vy *= -1;
		}
		else if (enemyShipFleet[i].y < UPPERBOUNDARY) {
			enemyShipFleet[i].y = enemyShipFleet[i].y + 300;
			enemyShipFleet[i].vy *= -1;
		}
		*/

		if (enemyShipFleet[i].ifHit){
			enemyShipFleet[i].framesSinceHit++;
			if (enemyShipFleet[i].framesSinceHit > FRAMESPERFLASH){
				enemyShipFleet[i].framesSinceHit = 0;
				enemyShipFleet[i].ifHit = false;
			}
		}
	}

	for (int k = 0; k < enemyShipFleet.size(); k++) {
		for (int i = 0; i < AMOUNTOFVECTORS; i++) {
			for (int j = 0; j < arrayOfVectors[i].size(); j++) {
				if (arrayOfVectors[i].at(j).x > enemyShipFleet[k].x - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2 &&
					arrayOfVectors[i].at(j).x < enemyShipFleet[k].x + (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2 &&
					arrayOfVectors[i].at(j).y > enemyShipFleet[k].y - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2 &&
					arrayOfVectors[i].at(j).y < enemyShipFleet[k].y + (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2) 
				{
					arrayOfVectors[i].erase(arrayOfVectors[i].begin() + j);
					if (enemyShipFleet[k].lives > 1) {
						enemyShipFleet[k].lives--;
						enemyShipFleet[k].ifHit = true;
						enemyShipFleet[k].framesSinceHit = 0;
					}
					else {
						enemyShipFleet.erase(enemyShipFleet.begin() + k);
						break;
					}
				}
			}
		}
	}

	for (int k = 0; k < enemyShipFleet.size(); k++){
		for (int i = 0; i < missileVector.size(); i++){
			if (missileVector[i].x > enemyShipFleet[k].x - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2 &&
				missileVector[i].x < enemyShipFleet[k].x + (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2 &&
				missileVector[i].y > enemyShipFleet[k].y - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2 &&
				missileVector[i].y < enemyShipFleet[k].y + (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2)
			{
				missileVector.erase(missileVector.begin() + i);
				if (enemyShipFleet[k].lives > 1) {
					enemyShipFleet[k].lives--;
					enemyShipFleet[k].ifHit = true;
					enemyShipFleet[k].framesSinceHit = 0;
				}
				else {
					enemyShipFleet.erase(enemyShipFleet.begin() + k);
					break;
				}
			}
		}
	}

	if (frameCount % 60 == 0 && enemyShipFleet.size() > 0) createBullet(false);

	//insect kamikaze ship rotation, movement, boundaries, bullet and ship collision
	for (int i = 0; i < insectKamikazeFleet.size(); i++) {
		findRotation(insectKamikazeFleet[i].x, insectKamikazeFleet[i].y, playerShip.x, playerShip.y, insectKamikazeFleet[i].rotation);
		insectKamikazeFleet[i].vx = cos(insectKamikazeFleet[i].rotation) * 2.2f;
		insectKamikazeFleet[i].vy = sin(insectKamikazeFleet[i].rotation) * 2.2f;
		insectKamikazeFleet[i].x += insectKamikazeFleet[i].vx;
		insectKamikazeFleet[i].y += insectKamikazeFleet[i].vy;
		if (insectKamikazeFleet[i].x + (ENEMYDIMENSION * INSECTSCALE) / 2 > RIGHTBOUNDARY) insectKamikazeFleet[i].x = RIGHTBOUNDARY - (ENEMYDIMENSION * INSECTSCALE) / 2;
		else if (insectKamikazeFleet[i].x - (ENEMYDIMENSION * INSECTSCALE) / 2 < LEFTBOUNDARY) insectKamikazeFleet[i].x = LEFTBOUNDARY + 5;
		if (insectKamikazeFleet[i].y + (ENEMYDIMENSION * INSECTSCALE) / 2 > LOWERBOUNDARY) insectKamikazeFleet[i].y = LOWERBOUNDARY - (ENEMYDIMENSION * INSECTSCALE) / 2;
		else if (insectKamikazeFleet[i].y - (ENEMYDIMENSION * INSECTSCALE) / 2 < UPPERBOUNDARY) insectKamikazeFleet[i].y = UPPERBOUNDARY + 5;

		if (insectKamikazeFleet[i].ifHit){
			insectKamikazeFleet[i].framesSinceHit++;
			if (insectKamikazeFleet[i].framesSinceHit > FRAMESPERFLASH){
				insectKamikazeFleet[i].framesSinceHit = 0;
				insectKamikazeFleet[i].ifHit = false;
			}
		}
	}

	for (int k = 0; k < insectKamikazeFleet.size(); k++) {
		for (int i = 0; i < AMOUNTOFVECTORS; i++) {
			for (int j = 0; j < arrayOfVectors[i].size(); j++) {
				if (arrayOfVectors[i].at(j).x > insectKamikazeFleet[k].x - (ENEMYDIMENSION * INSECTSCALE) / 2 &&
					arrayOfVectors[i].at(j).x < insectKamikazeFleet[k].x + (ENEMYDIMENSION * INSECTSCALE) / 2 &&
					arrayOfVectors[i].at(j).y > insectKamikazeFleet[k].y - (ENEMYDIMENSION * INSECTSCALE) / 2 &&
					arrayOfVectors[i].at(j).y < insectKamikazeFleet[k].y + (ENEMYDIMENSION * INSECTSCALE) / 2)
				{
					arrayOfVectors[i].erase(arrayOfVectors[i].begin() + j);
					if (insectKamikazeFleet[k].lives > 1) {
						insectKamikazeFleet[k].lives--;
						insectKamikazeFleet[k].ifHit = true;
						insectKamikazeFleet[k].framesSinceHit = 0;
					}
					else {
						insectKamikazeFleet.erase(insectKamikazeFleet.begin() + k);
						break;
					}
				}
			}
		}
	}	

	for (int k = 0; k < insectKamikazeFleet.size(); k++){
		for (int i = 0; i < missileVector.size(); i++){
			if (missileVector[i].x > insectKamikazeFleet[k].x - (ENEMYDIMENSION * INSECTSCALE) / 2 &&
				missileVector[i].x < insectKamikazeFleet[k].x + (ENEMYDIMENSION * INSECTSCALE) / 2 &&
				missileVector[i].y > insectKamikazeFleet[k].y - (ENEMYDIMENSION * INSECTSCALE) / 2 &&
				missileVector[i].y < insectKamikazeFleet[k].y + (ENEMYDIMENSION * INSECTSCALE) / 2)
			{
				missileVector.erase(missileVector.begin() + i);
				if (insectKamikazeFleet[k].lives > 1) {
					insectKamikazeFleet[k].lives--;
					insectKamikazeFleet[k].ifHit = true;
					insectKamikazeFleet[k].framesSinceHit = 0;
				}
				else {
					insectKamikazeFleet.erase(insectKamikazeFleet.begin() + k);
					break;
				}
			}
		}
	}

	if (insectKamikazeFleet.size() == 0 && enemyShipFleet.size() == 0) {
		setUpShips();
		wave++;
	}
}

void Game::bulletLogic() {
	if (kbd.EnterIsPressed() && weaponChoice == BULLET && !keysPressedLastFrame){
		weaponChoice = MISSILE;
		keysPressedLastFrame = true;
	}

	else if (kbd.EnterIsPressed() && weaponChoice == MISSILE && !keysPressedLastFrame){
		weaponChoice = BULLET;
		keysPressedLastFrame = true;
	}

	if (mouse.LeftIsPressed() && weaponChoice == BULLET/*&& !keysPressedLastFrame*/) {
		if (frameCount % 6 == 0) //perfect for slowing bullets down
		createBullet(true);
		keysPressedLastFrame = true;
	}

	else if (mouse.LeftIsPressed() && weaponChoice == MISSILE/*&& !keysPressedLastFrame*/) {
		if (frameCount % 6 == 0) //perfect for slowing bullets down
			createMissile(true);
		keysPressedLastFrame = true;
	}

	//if (!(mouse.LeftIsPressed())) keysPressedLastFrame = false;
	//if (!(kbd.EnterIsPressed())) keysPressedLastFrame = false;

	//sprinkler stuff, probably stupid to add
	/*if (frameCount % 10  == 0) {
	if (changeDirection) changeDirection = false;
	else changeDirection = true;
	}

	//swiper/sprinkler motion!!!!
	if (xVel <= 10 && clockwise) xVel += 0.5;
	else if (xVel > - 10 && !clockwise) xVel -= 0.5;

	if (xVel >= 10) clockwise = false;
	else if (xVel <= -10) clockwise = true;

	//if (yVel < 30) yVel += 5;
	yVel = -2;*/

	//movement and boundaries of bullets and animation
	for (int i = 0; i < AMOUNTOFVECTORS; i++) {
		for (int j = 0; j < arrayOfVectors[i].size(); j++) {
			//arrayOfVectors[i].at(j).vx += xVel;
			//arrayOfVectors[i].at(j).vy += yVel;
			arrayOfVectors[i].at(j).x += arrayOfVectors[i].at(j).vx;
			arrayOfVectors[i].at(j).y += arrayOfVectors[i].at(j).vy;
			arrayOfVectors[i].at(j).frameExposure++;
			if (arrayOfVectors[i].at(j).frameExposure > 3) {
				arrayOfVectors[i].at(j).frame++;
				if (arrayOfVectors[i].at(j).frame > 3) arrayOfVectors[i].at(j).frame = 0;
				arrayOfVectors[i].at(j).frameExposure = 0;
			}

			if (arrayOfVectors[i].at(j).y > LOWERBOUNDARY || arrayOfVectors[i].at(j).y < UPPERBOUNDARY || arrayOfVectors[i].at(j).x > RIGHTBOUNDARY || arrayOfVectors[i].at(j).x < LEFTBOUNDARY) {
				//FOR A MOTHERFUCKIN CLUSTERFUCK
				//arrayOfVectors[i].at(j).vx = arrayOfVectors[i].at(j).vx * - 1;
				//arrayOfVectors[i].at(j).vy = arrayOfVectors[i].at(j).vy * - 1;
				arrayOfVectors[i].erase(arrayOfVectors[i].begin() + j);
			}
		}
	}

	for (int a = 0; a < enemyBulletVector.size(); a++) {
		enemyBulletVector[a].x += enemyBulletVector[a].vx;
		enemyBulletVector[a].y += enemyBulletVector[a].vy;
		if (enemyBulletVector[a].y > LOWERBOUNDARY || enemyBulletVector[a].y < UPPERBOUNDARY || enemyBulletVector[a].x > RIGHTBOUNDARY || enemyBulletVector[a].x < LEFTBOUNDARY)
		{
			enemyBulletVector.erase(enemyBulletVector.begin() + a);
		}
	}

	for (int j = 0; j < missileVector.size(); j++) {
		findRotation(missileVector[j].x, missileVector[j].y, mouse.GetMouseX(), mouse.GetMouseY(), missileVector[j].rotation);
		missileVector[j].vx = cos(missileVector[j].rotation) * 4.0f;
		missileVector[j].vy = sin(missileVector[j].rotation) * 4.0f;

		missileVector[j].x += missileVector[j].vx;
		missileVector[j].y += missileVector[j].vy;

		if (missileVector[j].y > LOWERBOUNDARY || missileVector[j].y < UPPERBOUNDARY || missileVector[j].x > RIGHTBOUNDARY || missileVector[j].x < LEFTBOUNDARY) {
			missileVector.erase(missileVector.begin() + j);
		}
	}
}

void Game::createBullet(bool ifPlayer) {
	for (int i = 0; i < AMOUNTOFVECTORS; i++) {
		bullet bullet1;
		bullet1.r = rand() % 255;
		bullet1.g = rand() % 255;
		bullet1.b = rand() % 255;
		bullet1.frameExposure = 0;
		bullet1.frame = 0;

		if (ifPlayer) {
			bullet1.x = playerShip.x;
			bullet1.y = playerShip.y;
			findRotation(bullet1.x, bullet1.y, mouse.GetMouseX(), mouse.GetMouseY(), bullet1.rotation);

			bullet1.vx = cos(bullet1.rotation) * PLAYERBULLETSPEED;
			bullet1.vy = sin(bullet1.rotation) * PLAYERBULLETSPEED;

			arrayOfVectors[i].push_back(bullet1);
		}

		else {
			int k = rand() % enemyShipFleet.size();
			bullet1.x = enemyShipFleet[k].x;
			bullet1.y = enemyShipFleet[k].y;
			findRotation(bullet1.x, bullet1.y, playerShip.x, playerShip.y, bullet1.rotation);

			bullet1.vx = cos(bullet1.rotation) * ENEMYBULLETSPEED;
			bullet1.vy = sin(bullet1.rotation) * ENEMYBULLETSPEED;

			enemyBulletVector.push_back(bullet1);
		}	
	}
}

void Game::createMissile(bool ifPlayer){
	missile missile1;
	missile1.frameExposure = 0;
	missile1.frame = 0;

	if (ifPlayer) {
		missile1.x = playerShip.x;
		missile1.y = playerShip.y;
		findRotation(missile1.x, missile1.y, mouse.GetMouseX(), mouse.GetMouseY(), missile1.rotation);

		missile1.vx = cos(missile1.rotation) * PLAYERMSSILESPEED;
		missile1.vy = sin(missile1.rotation) * PLAYERMSSILESPEED;

		missileVector.push_back(missile1);
	}

	else {
	}
}

void Game::findRotation(int x1, int y1, int x2, int y2, float& rotation) {
	float deltaY = abs(y1 - y2);
	float deltaX = abs(x1 - x2);
	if (deltaX == 0.0) deltaX = 0.001;
	float slope = deltaY / deltaX;

	rotation = atan(slope);

	//T
	if (x2 > x1 && y2 >= y1);
	//C
	else if (x2 >= x1 && y2 < y1) rotation = -rotation;
	//A
	else if (x2 <= x1 && y2 < y1) rotation = PI + rotation;
	//S
	else if (x2 < x1 && y2 >= y1) rotation = PI - rotation;
}

void Game::ComposeFrame()
{	
	drawHealth();

	if (!playerShip.ifHit) gfx.Sprite_Transform_Draw(playerShipTexture, playerShip.x - ((PLAYERDIMENSION * PLAYERSHIPSCALE) / 2), playerShip.y - ((PLAYERDIMENSION * PLAYERSHIPSCALE) / 2), 250, 250, 0, 1, playerShip.rotation, PLAYERSHIPSCALE);
	else gfx.Sprite_Transform_Draw(playerShipTexture, playerShip.x - (PLAYERDIMENSION * PLAYERSHIPSCALE) / 2, playerShip.y - (PLAYERDIMENSION * PLAYERSHIPSCALE) / 2, 250, 250, 0, 1, playerShip.rotation, PLAYERSHIPSCALE, SHIPFLASHCOLOR);

	for (int i = 0; i < enemyShipFleet.size(); i++) {
		//MAYBE ORDER DIFFERENTLY. HE IS NOT HIT more THAN HE IS HIT.
		//actually makes not one bit of difference prety sure because 1 condition is evualuated either way. Stupid trying to be too efficient when we dont truly know much about efficiency yet anyway.
		if (enemyShipFleet[i].ifHit){
			gfx.Sprite_Transform_Draw(enemyShipTexture, enemyShipFleet[i].x - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2, enemyShipFleet[i].y - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2, ENEMYDIMENSION, ENEMYDIMENSION, 0, 1, enemyShipFleet[i].rotation, ENEMYSHIPSCALE, D3DCOLOR_XRGB(255, 0, 200));
		}
		else gfx.Sprite_Transform_Draw(enemyShipTexture, enemyShipFleet[i].x - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2, enemyShipFleet[i].y - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2, ENEMYDIMENSION, ENEMYDIMENSION, 0, 1, enemyShipFleet[i].rotation, ENEMYSHIPSCALE);
	}

	////////MAKE CENTRES OF ENEMYSHIPS AND INSECTS THEIR X AND Y

	for (int i = 0; i < insectKamikazeFleet.size(); i++) {
		if (insectKamikazeFleet[i].ifHit){
			gfx.Sprite_Transform_Draw(insectKamikazeTexture, insectKamikazeFleet[i].x - (ENEMYDIMENSION * INSECTSCALE) / 2, insectKamikazeFleet[i].y - (ENEMYDIMENSION * INSECTSCALE) / 2, ENEMYDIMENSION, ENEMYDIMENSION, 0, 1, insectKamikazeFleet[i].rotation, INSECTSCALE, D3DCOLOR_XRGB(255, 0, 200));
		}
		else gfx.Sprite_Transform_Draw(insectKamikazeTexture, insectKamikazeFleet[i].x - (ENEMYDIMENSION * INSECTSCALE) / 2, insectKamikazeFleet[i].y - (ENEMYDIMENSION * INSECTSCALE) / 2, ENEMYDIMENSION, ENEMYDIMENSION, 0, 1, insectKamikazeFleet[i].rotation, INSECTSCALE);
	}

	for (int i = 0; i < AMOUNTOFVECTORS; i++) {
		for (int j = 0; j < arrayOfVectors[i].size(); j++) {
			//gfx.Sprite_Transform_Draw(plasmaBullet, arrayOfVectors[i].at(j).x, arrayOfVectors[i].at(j).y, 30, 30, arrayOfVectors[i].at(j).frame, 4, arrayOfVectors[i].at(j).rotation, 2.0f, D3DCOLOR_XRGB(arrayOfVectors[i].at(j).r, arrayOfVectors[i].at(j).g, arrayOfVectors[i].at(j).b));
			gfx.DrawDisc(arrayOfVectors[i].at(j).x, arrayOfVectors[i].at(j).y, 5, arrayOfVectors[i].at(j).r, arrayOfVectors[i].at(j).g, arrayOfVectors[i].at(j).b);
		}
	}

	for (int i = 0; i < enemyBulletVector.size(); i++) {
		gfx.DrawDisc(enemyBulletVector[i].x, enemyBulletVector[i].y, 5, enemyBulletVector[i].r, enemyBulletVector[i].g, enemyBulletVector[i].b);
	}

	for (int i = 0; i < missileVector.size(); i++) {
		gfx.DrawCircle(missileVector[i].x, missileVector[i].y, 14, 0, 200, 100);
	}



	//gfx.DrawLine(cursorX, cursorY, x + 125, y + 125, 0, 255, 0);
	//gfx.DrawDisc(playerShip.x, playerShip.y, 12, 255, 255, 0);
}


/*
TO DO::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
(general order that we should do it in)

- missile lock on system. 1 missile to kill most enemy. Missile texture.
- rotation of player isnt super accurate coz of centre. Also bullets aren't going exactly to right place maybe for a different reason.
- explosions should be easy. But use list or stack (to get familiar with them, since vector is overkill) for the list of explosions, where they are and frame and frame exposure.
- make it more top orientatated so you dont have to go very far up. So you only move backwards or so.
- better ai.
- Releases and memory
- Initial level1 function with 5-10 waves and 5 testwave functiosn to try A LOT of different thigns to make the game exciting. Even just making the enemies follow in funny patterns each different wave.
- 5 Level functions (e.g.) and inside each maybe 5-10 Wave functions. Or even a parameterisable function to avoid 50 functions. Parameters (int insects, int enemyship1s, int timeBetweenWaves, int timeBetweengroups, int insectGroupsize, int enemyship1Groupsize)
- sound effects should be easy with chilis sound.cpp?
- get a weapon on the hud showing if missile or plasma is on.
- fix boundaries so they never get stuck. Maybe just make them move away from them or something.
- Try simple laser aswell.
- make enemys y equal x^2 or something or log(2). Eventually find some function that always has a base point of the playership so we can make rotate around player.
- Get Text in game with funny and informative comments. If gameover or on next wave etc.
- shaking if hit
- fullscreen
- Make enemies come at different times or if a certain amount of them have been killed. Different for each wave maybe?
- Make enemyship1s circle you anticlockwise and clockwise. Or even do a semi circle.
- Combine velocities to avoid every other ship as well as go to the direction that ship wants to go in the end. DIFFICULT.
- Better collision between bullets
- Crashing causes damage to both ships. 
- The Hud (shield, hull, 4 weapons, radar(minimap), powerups, Writing saying level+wave + other writing,) 
- Leave boss till level 5, focus more on everything else.
- Have Enemyship1s do swarming formations and keep their distance firing. Every enemyship should avoid every other enemy ship. Might be able to fix with conditions similar to boundary code.
- have intro screen where bullets or things come out of 100s of random places in random directions then game starts. Use pretty much crazy colours and patterns and randomness for story, intros, cutscenes whatever we can.
- http://www.gamedev.net/page/resources/_/technical/directx-and-xna/pixel-perfect-collision-detection-in-directx-r2939
- UP and down scrolling and maybe only this? Ah maybe the arcade mode and adventure mode. Arcade is up and down. Adventure is exploring everywhere on a map.
- start thinking about resolution


//INTERESTING IDEAS:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
- Scrolling backwards just an endless horde of them coming towards you, 
- Asteroids you have to avoid.
- Your ship gets damaged (looks like its falling apart). 3 DIFFERENT varying versions of each ship and maybe slight variations on enemy ones so they arent all the same.
- Insect is called WASP. WEaponised auto space predator.
- mazes.
- When play is clicked for first time and first wave. introduces first two enemies and what to do with them with a picture, picture is animated scaled up and stops scaling so looks pro :).
- for one boss maybe have it like alien and you see him on the minimap but you have to avoid him in this big maze or something and he scares the shit out of you. The faster you avoid him the more he dies? 
	Think of an actual reason for this or actually tell the player "fuck you, it doesnt have to make sense"
- multiplayer with 360 controllers or eventually lan.

//STORY AND TEXT TO PUT IN:
- Big twist is to kill yourself. You are the boss. Maybe mirrors or some game mechanic like that to kill yourself.
- You are your own worst enemy.
- loads of clues throughout the game hinting that you are the enemy. Enemies give hints that you are a bad bad man. Keep your friends close but your enemies even closer. And you cant get closer than yourself. look in the mirror.
- In space I can hear you scream.
- round one: survive. Round two: survive again. Round three: Ur shit. Round four: ah for feck sake.
- we need reinforcements
- speech bubbles?

WEEKLY LOG / PROGRESS REPORT:
	BEFORE WEEK 1: Only ship 3 in game with old sprite code and was reticle game before with vectors of bullets ready.

	WEEK 1: 17th of September: Bullets going in any direction, which pretty much unlocks all angle work to be done. Clean up of all reticle game shite.
	WEEK 2: 24nd of September: Rotation almost working but slightly off with 3 ships finally in game.
	WEEK 3:	1st of October: Major breakthrough week. Got all ships rotating perfectly with ROTATION FUNCTION. Got vectors of enemy ships and made them move. If wave killed, it respawns.
	WEEK 4: 8th of October: Flashing if any ship hit. Game over with health bar and enemy ships shooting bullets so officially could be called a game. Began work on scaling.
	WEEK 5: 15th of October: Scaling with rotation+collision+boundaries 100% perfect!!! Makes it possible for anything to be drawn in any pixel dimensions and then be scaled. Initial missile following mouse cursor implemented. Kamikaze hurts player and can super officially call it a game.

	WEEK 6: 22nd of October: Plan: Explosions, missile maybe, better ai, Weapon switch on hud. Top orientated. Better collision. Text into game.


//TINY LITTLE SMALL NOTES::::::::::::::::::::::::::::::::::::::::::::::
*/