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
#include <ctime>

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
	keysPressedLastFrame = false;
	gameOver = false;
	invincible = true;
	paused = false;

	srand(static_cast <unsigned> (time(0)));

	plasmaShot = audio.CreateSound("tinkle.wav");
	tune = audio.CreateSound("Dark Trance for Kearney.wav");
	//use last few bars of song for menu music or gameover.
	//tune.Play();

	playerShip.x = 900;
	playerShip.y = 600;
	playerShip.lives = PLAYERSHIPLIVES;
	playerShip.rotation = 0.0f;
	playerShip.framesSinceHit = 0;
	playerShip.ifHit = 0;
	playerShip.missileAmmunition = 15;
	speed = 8.0f;
	weaponChoice = BULLET;
	for (int i = 0; i < playerShip.missileAmmunition; i++) {
		createMissile(true, NULL);
	}

	currentlyLocking = false;
	noOfLockedOn = 0;
	specificTarget = NULL;

	start = time(0);
	wave = 1;
	waveTimer = 0;
	nextSquadTimer = 0;	
	doneIt = false; //rename
	initialTimeBetweenWaves = 1;
	minTimeBetweenSquad = 6;
	minTimeBetweenWaves = 4;
	timeTillNextSquad = initialTimeBetweenWaves;
	totalEnemyShipCreatedLevel = 0;
	squadsCreatedLevel = 0;
	squadsDefeatedLevel = 0;
	squadsCreatedWave = 0;
	squadsDefeatedWave = 0;
	squadsPerWave = 15;
	totalInsectCreatedLevel = 0;

	//hahaaa very interesting. Explain reservations. vectors dynamic memory. Massive bug.
	insectKamikazeFleet.reserve(100);
	//ahaa not need for deque coz
	//http://stackoverflow.com/questions/15524475/deque-how-come-reserve-doesnt-exist
	//enemyShipFleet.reserve(100);
	//or even does deque solve our pointer/address problem????
	//ahaa deques dont reallocate when move elements or add or remove or insert.
	//http://stackoverflow.com/questions/13235372/will-stl-deque-reallocate-my-elements-c
	//does avoid us having to change a thousand .'s in to ->'s. Plus I havent thought of a a way of making the stack pointer in makeenemyship without using new.
	pShipTargets.reserve(50);
	squadVector.reserve(50);

	totalEnemyShipKillsLevel = 0;
	totalInsectKillsLevel = 0;

	totalShipCreatedLevel = 0;
	totalShipCreatedWave = 0;
	totalShipKillsLevel = 0;
	totalShipKillsWave = 0;

	wavesPerLevel = 3;

	fontArial24 = gfx.MakeFont("Arial", 24);
	fontGaramond36 = gfx.MakeFont("Garamond", 36);
	fontTimesNewRoman40 = gfx.MakeFont("Times new Roman", 40);

	playerShipTexture = gfx.LoadTexture("Player ship 3.bmp", D3DCOLOR_XRGB(0, 0, 0));
	enemyShipTexture = gfx.LoadTexture("Ship 3.bmp");
	insectKamikazeTexture = gfx.LoadTexture("insectAnm 15 frame bmp.bmp", D3DCOLOR_XRGB(0, 148, 255));
	plasmaBulletTexture = gfx.LoadTexture("Blue ball bullet 1.bmp", D3DCOLOR_XRGB(0, 0, 0));
	missileTexture = gfx.LoadTexture("Bomb 1 (cropped).bmp");
	missileLockTexture = gfx.LoadTexture("Missilelock1.bmp..bmp");
	explosionTexture = gfx.LoadTexture("Explosion 2 sprite sheet (2) shaded.bmp", D3DCOLOR_XRGB(33, 198, 255));
	//missileIconTexture = gfx.LoadTexture("Bomb Icon 1.bmp");
	//bulletIconTexture = gfx.LoadTexture("Laser Icon 1.bmp");
	missileIconTexture = gfx.LoadTexture("missileicon.hud.bmp", D3DCOLOR_XRGB(255, 255, 255));
	bulletIconTexture = gfx.LoadTexture("lasericon.hud.bmp", D3DCOLOR_XRGB(255, 255, 255));
	weaponHudTexture = gfx.LoadTexture("Weaponhud.bmp", D3DCOLOR_XRGB(255, 255, 255));
}

Game::~Game() {
	//get some releases in here for all textures and maybe more	
	playerShipTexture->Release();
	enemyShipTexture->Release();
	insectKamikazeTexture->Release();

	plasmaBulletTexture->Release();
	missileTexture->Release();

	missileIconTexture->Release();
	bulletIconTexture->Release();
	explosionTexture->Release();

	fontArial24->Release();
	fontArial24 = NULL;
	fontGaramond36->Release();
	fontGaramond36 = NULL;
	fontTimesNewRoman40->Release();
	fontTimesNewRoman40 = NULL;
}

void Game::Go()
{
	frameCount++;

	if (kbd.SpaceIsPressed() && !keysPressedLastFrame) {
		if (paused) paused = false;
		else paused = true;
		keysPressedLastFrame = true;
	}	

	//wow to prove exactly when just simple computation (not rendering) can slow shit down.
	/*for (int i = 0; i < 5800000; i++)
	{
		;
	}*/
	/*for (int i = 0; i < 10000000; i++)
	{
		;
	}*/
	/*for (int i = 0; i < 100000000; i++)
	{
		;
	}*/


	if (!paused) {
		if (!gameOver) {
			secondsSinceStart = difftime(time(0), start);
			waveTimer = secondsSinceStart;
			nextSquadTimer = secondsSinceStart;

			setUpShips();
			playerShipLogic();
			bulletLogic();
			updateEnemy();	
		}
	}

	

	//why isnt this working?
	//ahh wasnt working because there was a separate make false for if enter wasnt pressed in bullet logic
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
	const int yBar = 1000;

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

	char buffer[64];
	sprintf(buffer, "Health: %d", playerShip.lives);
	gfx.FontPrint(fontArial24, 50, 950, buffer, D3DCOLOR_XRGB(0, 255, 0));
}

void Game::setUpShips() {
	const int maxAmountInSquad = 3;
	if (wave == 1) {
		if (nextSquadTimer % timeTillNextSquad == 0 && nextSquadTimer != 0 && !doneIt && squadsCreatedWave < squadsPerWave) {
			int shipType;
			int noOfShipsInSquad = rand() % maxAmountInSquad + 1;
			squadVector.push_back(Squad());
			for (int i = 0; i < noOfShipsInSquad; i++) {
				shipType = rand() % 2;		
				makeEnemy(shipType, squadVector.size() - 1);		
			}
			timeTillNextSquad = rand() % 4 + minTimeBetweenSquad;
			doneIt = true;
			totalShipCreatedLevel = totalShipCreatedLevel + totalEnemyShipCreatedLevel + totalInsectCreatedLevel;
			//totalShipCreatedWave = ;	
			squadsCreatedLevel++;
			squadsCreatedWave++;
		}
		else if (nextSquadTimer % timeTillNextSquad != 0 && doneIt) doneIt = false;
		if (squadsDefeatedWave >= squadsPerWave) {
			wave++;
			totalShipCreatedWave = 0; //not needed but fuck it for now
			squadsCreatedWave = 0;
			squadsDefeatedWave = 0;
		}
	}
	
	if (wave == 2) {

	}

	/*if (nextSquadTimer % 2 == 0 && !doneIt) {
		int a = rand() % 2;
		makeEnemy(a, 0);
		doneIt = true;
	}
	else doneIt = false;*/	
}

void Game::makeEnemy(int whichShip, int squadCreatedCount) {
	//whichShip = 0;
	if (whichShip == 0) {
		EnemyShip1 enemyShip;
		enemyShip.ifHit = false;
		enemyShip.framesSinceHit = 0;
		enemyShip.lives = 20;
		enemyShip.y = (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2 + 10;
		//this has got to be fixed
		enemyShip.x = rand() % RIGHTBOUNDARY - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2;
		enemyShip.speed = 1.0f;
		//enemyShip.x = LEFTBOUNDARY + 30 + (i*100);
		enemyShip.vx = 0;
		//enemyShip.vy = 1.0f;
		enemyShip.vy = 0.0f;
		enemyShipFleet.push_back(enemyShip);
		totalEnemyShipCreatedLevel++;
		//squadVector[squadCreatedCount - squadsDefeatedWave].shipArray.push_back(&enemyShipFleet.back());
		squadVector[squadCreatedCount].shipArray.push_back(&enemyShipFleet.back());
		lockOnCandidate joe;
		joe.ship = &enemyShipFleet.back();
		joe.startTime = GetTickCount();
		joe.frame = 0;
		joe.lockedOn = false;
		joe.lockingOn = false;
		joe.inMouseRange = false;
		joe.lockOnDelay = INITIALLOCKDELAY;
		pShipTargets.push_back(joe);	
	}

	else if (whichShip == 1) {
		Insect insect;
		insect.ifHit = false;
		insect.framesSinceHit = 0;
		insect.lives = 15;
		insect.speed = INSECTSPEED;
		insect.y = (ENEMYDIMENSION * INSECTSCALE) / 2 + 20;
		insect.x = rand() % RIGHTBOUNDARY - (ENEMYDIMENSION * INSECTSCALE) / 2;
		insect.frame = 0;
		insect.frameExposure = 0;
		insectKamikazeFleet.push_back(insect);
		//squadVector[squadCreatedCount - squadsDefeatedWave].shipArray.push_back(&insectKamikazeFleet.back());
		squadVector[squadCreatedCount].shipArray.push_back(&insectKamikazeFleet.back());
		totalInsectCreatedLevel++;
	}
}

void Game::playerShipLogic() {
	if (kbd.RightIsPressed()) {
		playerShip.x = playerShip.x + speed;
	}

	if (kbd.LeftIsPressed()) {
		playerShip.x = playerShip.x - speed;
	}

	if (kbd.UpIsPressed()) {
		playerShip.y = playerShip.y - speed;
	}

	if (kbd.DownIsPressed()) {
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
	updateEnemyShip1s();
	updateInsects();

	int i;
	int k;
	for (i = 0; i < squadVector.size(); i++) {
		int allShipsInSquadDefeated = 0;
		int shipsInSquad = squadVector[i].shipArray.size();
		for (k = 0; k < squadVector[i].shipArray.size(); k++) {
			if (squadVector[i].shipArray[k]->lives <= 0) {
				if (k == 0) {
					allShipsInSquadDefeated++;
				}
				else {
					allShipsInSquadDefeated++;
				}
				//squadVector[i].shipArray.erase(squadVector[i].shipArray.begin() + k);
			}
		}
		if (allShipsInSquadDefeated >= shipsInSquad) {
			squadsDefeatedLevel++;
			squadsDefeatedWave++;
			squadVector.erase(squadVector.begin() + i);
		}
	}
	
	for (int k = 0; k < explosionVector.size(); k++) {
		explosionVector.at(k).frameExposure++;
		if (explosionVector.at(k).frameExposure > EXPLOSIONSPEED) {
			explosionVector.at(k).frame++;
			explosionVector.at(k).frameExposure = 0;
			if (explosionVector.at(k).frame > 12) explosionVector.erase(explosionVector.begin() + k);
		}
	}
}

void Game::updateEnemyShip1s() {
	//rotation, movement, boundaries, bullet and ship collision
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
		if (enemyShipFleet[i].ifHit) {
			enemyShipFleet[i].framesSinceHit++;
			if (enemyShipFleet[i].framesSinceHit > FRAMESPERFLASH) {
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
					arrayOfVectors[i].at(j).y < enemyShipFleet[k].y + (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2) {
					arrayOfVectors[i].erase(arrayOfVectors[i].begin() + j);
					enemyShipFleet[k].lives -= BULLETDAMAGE;
					if (enemyShipFleet[k].lives <= 0) {
						createExplosion(explosion(), enemyShipFleet[k].x, enemyShipFleet[k].y, enemyShipFleet[k].rotation, 0.6f, rand() % 255, rand() % 255, rand() % 255);
						for (int a = 0; a < pShipTargets.size(); a++) {
							;//if (pShipTargets[a].ship == &enemyShipFleet[k]) pShipTargets.erase(pShipTargets.begin() + a);
						}			
						enemyShipFleet.erase(enemyShipFleet.begin() + k);
						totalEnemyShipKillsLevel++;
						break;
					}
					else {
						enemyShipFleet[k].ifHit = true;
						enemyShipFleet[k].framesSinceHit = 0;
					}
				}
			}
		}
	}

	for (int k = 0; k < enemyShipFleet.size(); k++) {
		for (int i = 0; i < missileVector.size(); i++) {
			if (missileVector[i].x > enemyShipFleet[k].x - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2 &&
				missileVector[i].x < enemyShipFleet[k].x + (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2 &&
				missileVector[i].y > enemyShipFleet[k].y - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2 &&
				missileVector[i].y < enemyShipFleet[k].y + (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2) {
				missileVector[i].target = NULL;
				missileVector.erase(missileVector.begin() + i);
				enemyShipFleet[k].lives -= MISSILEDAMAGE;
				if (enemyShipFleet[k].lives <= 0) {
					createExplosion(explosion(), enemyShipFleet[k].x, enemyShipFleet[k].y, enemyShipFleet[k].rotation, 0.6f, 255, 255, 255);
					for (int a = 0; a < pShipTargets.size(); a++) {
						if (pShipTargets[a].ship == &enemyShipFleet[k]) {
							//pShipTargets[a].ship = NULL;
							//pShipTargets.erase(pShipTargets.begin() + a);
							noOfLockedOn--;
							break;
						}
					}
					//pShipTargets[k].ship = NULL;
					pShipTargets.erase(pShipTargets.begin() + k); //on the way to getting there. Fixes killing 1 guy and another green lock on showing hmm ///wont work with insects
					enemyShipFleet.erase(enemyShipFleet.begin() + k);
					for (int i = 0; i < pShipTargets.size(); i++) {
						pShipTargets[i].ship = &enemyShipFleet[i];
					}
					totalEnemyShipKillsLevel++;
					break;
				}
				else {
					enemyShipFleet[k].ifHit = true;
					enemyShipFleet[k].framesSinceHit = 0;
				}
			}
		}
	}

	if (frameCount % 60 == 0 && enemyShipFleet.size() > 0) createBullet(false);
}

void Game::updateInsects() {
	//insect kamikaze ship rotation, movement, boundaries, bullet and ship collision
	for (int i = 0; i < insectKamikazeFleet.size(); i++) {
		findRotation(insectKamikazeFleet[i].x, insectKamikazeFleet[i].y, playerShip.x, playerShip.y, insectKamikazeFleet[i].rotation);
		insectKamikazeFleet[i].vx = cos(insectKamikazeFleet[i].rotation) * insectKamikazeFleet[i].speed;
		insectKamikazeFleet[i].vy = sin(insectKamikazeFleet[i].rotation) * insectKamikazeFleet[i].speed;
		insectKamikazeFleet[i].x += insectKamikazeFleet[i].vx;
		insectKamikazeFleet[i].y += insectKamikazeFleet[i].vy;
		insectKamikazeFleet[i].frameExposure++;
		if (insectKamikazeFleet[i].frameExposure > 4) {
			insectKamikazeFleet[i].frame++;
			insectKamikazeFleet[i].frameExposure = 0;
			if (insectKamikazeFleet[i].frame > 14) insectKamikazeFleet[i].frame = 0;
		}

		if (insectKamikazeFleet[i].x + (ENEMYDIMENSION * INSECTSCALE) / 2 > RIGHTBOUNDARY) insectKamikazeFleet[i].x = RIGHTBOUNDARY - (ENEMYDIMENSION * INSECTSCALE) / 2;
		else if (insectKamikazeFleet[i].x - (ENEMYDIMENSION * INSECTSCALE) / 2 < LEFTBOUNDARY) insectKamikazeFleet[i].x = LEFTBOUNDARY + 5;
		if (insectKamikazeFleet[i].y + (ENEMYDIMENSION * INSECTSCALE) / 2 > LOWERBOUNDARY) insectKamikazeFleet[i].y = LOWERBOUNDARY - (ENEMYDIMENSION * INSECTSCALE) / 2;
		else if (insectKamikazeFleet[i].y - (ENEMYDIMENSION * INSECTSCALE) / 2 < UPPERBOUNDARY) insectKamikazeFleet[i].y = UPPERBOUNDARY + 5;

		if (insectKamikazeFleet[i].ifHit) {
			insectKamikazeFleet[i].framesSinceHit++;
			if (insectKamikazeFleet[i].framesSinceHit > FRAMESPERFLASH) {
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
					arrayOfVectors[i].at(j).y < insectKamikazeFleet[k].y + (ENEMYDIMENSION * INSECTSCALE) / 2) {
					arrayOfVectors[i].erase(arrayOfVectors[i].begin() + j);
					insectKamikazeFleet[k].lives -= BULLETDAMAGE;	
					if (insectKamikazeFleet[k].lives <= 0) {
						createExplosion(explosion(), insectKamikazeFleet[k].x, insectKamikazeFleet[k].y, insectKamikazeFleet[k].rotation, 0.55f, 0, 255, 0);
						insectKamikazeFleet.erase(insectKamikazeFleet.begin() + k);
						totalInsectKillsLevel++;
						break;
					}
					else {
						insectKamikazeFleet[k].ifHit = true;
						insectKamikazeFleet[k].framesSinceHit = 0;
					}
				}
			}
		}
	}

	/*




	Make function that takes in x,y of bullet/weapon and ship, full width and height of both bullet and ship, What type of ship,
	will make much easier to change other stuff then.







	*/


	for (int k = 0; k < insectKamikazeFleet.size(); k++) {
		for (int i = 0; i < missileVector.size(); i++) {
			if (missileVector[i].x > insectKamikazeFleet[k].x - (ENEMYDIMENSION * INSECTSCALE) / 2 &&
				missileVector[i].x < insectKamikazeFleet[k].x + (ENEMYDIMENSION * INSECTSCALE) / 2 &&
				missileVector[i].y > insectKamikazeFleet[k].y - (ENEMYDIMENSION * INSECTSCALE) / 2 &&
				missileVector[i].y < insectKamikazeFleet[k].y + (ENEMYDIMENSION * INSECTSCALE) / 2) {
				missileVector.erase(missileVector.begin() + i);
				insectKamikazeFleet[k].lives -= MISSILEDAMAGE;
				if (insectKamikazeFleet[k].lives <= 0) {		
					createExplosion(explosion(), insectKamikazeFleet[k].x, insectKamikazeFleet[k].y, insectKamikazeFleet[k].rotation, 0.55f, 0, 255, 0);
					insectKamikazeFleet.erase(insectKamikazeFleet.begin() + k);
					totalInsectKillsLevel++;
					break;
				}
				else {
					insectKamikazeFleet[k].ifHit = true;
					insectKamikazeFleet[k].framesSinceHit = 0;
				}
			}
		}
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
		if (frameCount % 8 == 0) //perfect for slowing bullets down
		createBullet(true);
		keysPressedLastFrame = true;
	}

	else if (weaponChoice == MISSILE/*&& !keysPressedLastFrame*/) {
		allLocking();
	}

	
	//if (!(mouse.LeftIsPressed())) keysPressedLastFrame = false;
	//if (!(kbd.EnterIsPressed())) keysPressedLastFrame = false;

	//movement and boundaries of bullets and animation
	for (int i = 0; i < AMOUNTOFVECTORS; i++) {
		for (int j = 0; j < arrayOfVectors[i].size(); j++) {
			arrayOfVectors[i].at(j).x += arrayOfVectors[i].at(j).vx;
			arrayOfVectors[i].at(j).y += arrayOfVectors[i].at(j).vy;
			arrayOfVectors[i].at(j).frameExposure++;
			if (arrayOfVectors[i].at(j).frameExposure > 4) {
				arrayOfVectors[i].at(j).frame++;
				if (arrayOfVectors[i].at(j).frame > 4) arrayOfVectors[i].at(j).frame = 0;
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

		enemyBulletVector[a].frameExposure++;
		if (enemyBulletVector[a].frameExposure > 4) {
			enemyBulletVector[a].frame++;
			if (enemyBulletVector[a].frame > 4) enemyBulletVector[a].frame = 0;
			enemyBulletVector[a].frameExposure = 0;
		}

		if (enemyBulletVector[a].y > LOWERBOUNDARY || enemyBulletVector[a].y < UPPERBOUNDARY || enemyBulletVector[a].x > RIGHTBOUNDARY || enemyBulletVector[a].x < LEFTBOUNDARY)
		{
			enemyBulletVector.erase(enemyBulletVector.begin() + a);
		}
	}

	for (int j = 0; j < missileVector.size(); j++) {
		if (missileVector[j].target) findRotation(missileVector[j].x, missileVector[j].y, missileVector[j].target->x, missileVector[j].target->y, missileVector[j].rotation);
		else missileVector[j].target = NULL;
		//not needed should be done elsewhere instead of all the time
		missileVector[j].vx = cos(missileVector[j].rotation) * PLAYERMSSILESPEED;
		missileVector[j].vy = sin(missileVector[j].rotation) * PLAYERMSSILESPEED;

		missileVector[j].x += missileVector[j].vx;
		missileVector[j].y += missileVector[j].vy;

		if (missileVector[j].y > LOWERBOUNDARY || missileVector[j].y < UPPERBOUNDARY || missileVector[j].x > RIGHTBOUNDARY || missileVector[j].x < LEFTBOUNDARY) {
			missileVector[j].target = NULL;
			missileVector.erase(missileVector.begin() + j);
		}
	}
}

void Game::createExplosion(explosion& a, float x, float y, float rotation, float scale, int r, int g, int b) {
	a.x = x;
	a.y = y;
	a.rotation = rotation;
	a.frameExposure = 0;
	a.frame = 0;
	a.scale = scale - 0.05 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.05 - (-1 * 0.05))));
	a.r = r;
	a.g = g;
	a.b = b;
	explosionVector.push_back(a);
}

void Game::createBullet(bool ifPlayer) {
	for (int i = 0; i < AMOUNTOFVECTORS; i++) {
		bullet bullet1;
		/*bullet1.r = rand() % 255;
		bullet1.g = rand() % 255;
		bullet1.b = rand() % 255;*/

		bullet1.frameExposure = 0;
		bullet1.frame = 0;

		if (ifPlayer) {
			bullet1.x = playerShip.x;
			bullet1.y = playerShip.y;
			findRotation(bullet1.x, bullet1.y, mouse.GetMouseX(), mouse.GetMouseY(), bullet1.rotation);

			bullet1.vx = cos(bullet1.rotation) * PLAYERBULLETSPEED;
			bullet1.vy = sin(bullet1.rotation) * PLAYERBULLETSPEED;

			arrayOfVectors[i].push_back(bullet1);
			//plasmaShot.Play();
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

void Game::createMissile(bool ifPlayer, Ship* targetLockOn) {
	//hmmm this wont work
	//make it so that if you collect ammo after locking on then you can fire missiles at enemy
	//or just make it so you cant lock on without ammo?
	//makes more sense?
	//what if you lock on to three guys, use your two missiles. collect ammo shoot third missile at 3rd guy?
	//or just never lock onto more than you have ammo.
	//most importantly if missile.target == null lose lock on

	//if you found ammo add missiles to ammo vector with no target
	if (targetLockOn == NULL) {	
		missile missile1;
		missile1.target = NULL;
		missileAmmoVector.push_back(missile1);
	}
	//else you are setting the target for the last item in the vector that doesn't have a target.
	else {
		for (int i = missileAmmoVector.size() - 1; i >= 0; i--) {
			if (missileAmmoVector[i].target == NULL) {
				missileAmmoVector[i].target = targetLockOn;
				return; //or break, both same?
			}
		}
	}
}

void Game::fireMissile(bool ifPlayer) {
	if (ifPlayer && missileAmmoVector.back().target != NULL) {
		missileAmmoVector.back().x = playerShip.x;
		missileAmmoVector.back().y = playerShip.y;
		missileAmmoVector.back().frame = 0;

		findRotation(missileAmmoVector.back().x, missileAmmoVector.back().y, missileAmmoVector.back().target->x, missileAmmoVector.back().target->y, missileAmmoVector.back().rotation);
		missileAmmoVector.back().vx = cos(missileAmmoVector.back().rotation) * PLAYERMSSILESPEED;
		missileAmmoVector.back().vy = sin(missileAmmoVector.back().rotation) * PLAYERMSSILESPEED;

		missileVector.push_back(missileAmmoVector.back());
		missileAmmoVector.erase(missileAmmoVector.begin() + missileAmmoVector.size() - 1);
		playerShip.missileAmmunition--;
	}

	else {
	}
}

void Game::findLockOn() {
	for (int i = 0; i < pShipTargets.size(); i++) { //this made it so much easier. Equally likely to be an insect or whatever other ship.
		if (Distance(mouse.GetMouseX(), mouse.GetMouseY(), pShipTargets[i].ship->x, pShipTargets[i].ship->y) < 100 && !pShipTargets[i].lockedOn /*&& !pShipTargets[i].lockingOn*/) {
			pShipTargets[i].lockingOn = true;
			pShipTargets[i].frame = 8;
			pShipTargets[i].lockOnDelay = LOCKINGDELAY;
			pShipTargets[i].startTime = GetTickCount();
			specificTarget = &pShipTargets[i];
			currentlyLocking = true;
			return;
		}
	}
}

void Game::createLockOn() {
	//all this function does is check if the guy you are locking on stays within range of your mouse. If you lose him, you lose lock.
	if (Distance(mouse.GetMouseX(), mouse.GetMouseY(), specificTarget->ship->x, specificTarget->ship->y) > 100) {
		currentlyLocking = false;
		specificTarget->lockingOn = false;
		specificTarget->lockedOn = false;
		specificTarget->frame = 0;
		specificTarget->lockOnDelay = INITIALLOCKDELAY;
		specificTarget->startTime = GetTickCount();
		specificTarget = NULL;
	}
}

void Game::allLocking() {
	if (mouse.LeftIsPressed() && !mouseClicked) {
		//if (noOfLockedOn > 0) 
		if (playerShip.missileAmmunition > 0) fireMissile(true);
		mouseClicked = true;
	}

	if (mouse.RightIsPressed() && playerShip.missileAmmunition > 0) { //for now or maybe forever? Ok Ive decided forever, its better.
		if (!currentlyLocking && specificTarget == NULL && noOfLockedOn < playerShip.missileAmmunition) {
			findLockOn();
		} 
		else if (currentlyLocking && specificTarget) createLockOn();
	}
	else {
		if (specificTarget) { //coz accessing specificTarget->lockingOn when its null is impossible
			specificTarget->lockingOn = false;
			specificTarget->frame = 0;
			specificTarget->startTime = GetTickCount();
			specificTarget = NULL;
		}
		specificTarget = NULL;
		currentlyLocking = false;
	}

	if (!mouse.LeftIsPressed()) mouseClicked = false;

	for (int i = 0; i < pShipTargets.size(); i++) {
		if ((GetTickCount() - pShipTargets[i].startTime) > pShipTargets[i].lockOnDelay && !pShipTargets[i].lockedOn) {
			if (pShipTargets[i].ship == NULL) pShipTargets.erase(pShipTargets.begin() + i);
			pShipTargets[i].startTime = GetTickCount();
			pShipTargets[i].frame++;
			//pShipTargets[i].lockOnDelay make this a variable so slower for first 8. different speed for 8-37
			//pShipTargets[i].frame = rand() % 8; so they all dont rotate the same; //doesnt matter anymore since it only shows sometimes.
			if (/*!currentlyLocking &&*/ //hmmm keep this? Want other ships to have things over them while locking onto another?
				Distance(mouse.GetMouseX(), mouse.GetMouseY(), pShipTargets[i].ship->x, pShipTargets[i].ship->y) < 100) {
				pShipTargets[i].inMouseRange = true;
				
			}
			else pShipTargets[i].inMouseRange = false;

			if (pShipTargets[i].frame >= 8 && !pShipTargets[i].lockingOn) pShipTargets[i].frame = 0;
			if (pShipTargets[i].frame >= 38 && pShipTargets[i].lockingOn) {
				noOfLockedOn++;
				pShipTargets[i].lockedOn = true;
				pShipTargets[i].lockingOn = false;
				pShipTargets[i].frame = 39;
				currentlyLocking = false;
				createMissile(true, pShipTargets[i].ship);
				specificTarget = NULL;
			}
		}
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

void Game::collisionDetection(int width1, int height1, int width2, int height2, int x1, int y1, int x2, int y2, 
	int vx1, int vy1, int vx2, int vy2, int typeOfCollision) {

}

/*void Game::collisionResponse() {}*/

void Game::writeText(LPD3DXFONT font, int x, int y, D3DCOLOR c, char * str) {
	char buffer[64];
	gfx.FontPrint(fontGaramond36, x, y, str, D3DCOLOR_XRGB(255, 255, 255));
}
/*




IF YOU ERASE, EVERYTHING IS MOVED BACK ONE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 


NEVER EVER REFER A POINTER TO SOMETHING IN AN ARRAY OR VECTOR, UNLESS IT IS AN VECTOR OF POINTERS.








*/

void Game::ComposeFrame()
{	
	spriteobj2->Begin(D3DXSPRITE_ALPHABLEND);
	drawHealth();

	const int baseY = 900;
	//quick function that does this char buffer thing. UP ABOVE
	char buffer[64];
	sprintf(buffer, "Seconds elapsed: %.2f", secondsSinceStart);
	//gfx.FontPrint(fontGaramond36, 50, baseY, buffer, D3DCOLOR_XRGB(255, 255, 255));
	writeText(fontGaramond36, 50, baseY, D3DCOLOR_XRGB(255, 255, 255), buffer);

	char buffer2[64];
	sprintf(buffer, "Squads defeated: %d/%d", squadsDefeatedWave, squadsPerWave);
	gfx.FontPrint(fontTimesNewRoman40, 50, baseY - 50, buffer, D3DCOLOR_XRGB(255, 255, 255));

	char buffer8[64];
	sprintf(buffer, "Squads created: %d/%d", squadsCreatedWave, squadsPerWave);
	gfx.FontPrint(fontTimesNewRoman40, 50, baseY - 100, buffer, D3DCOLOR_XRGB(255, 255, 255));

	char buffer3[64];
	sprintf(buffer, "InsectsNoLevel: %d enemyShipNoLevel: %d", totalInsectCreatedLevel, totalEnemyShipCreatedLevel);
	gfx.FontPrint(fontTimesNewRoman40, 50, baseY - 150, buffer, D3DCOLOR_XRGB(255, 255, 255));

	char buffer5[64];
	sprintf(buffer, "No. Ships Killed Level: %d/%d", totalShipKillsLevel, totalShipCreatedLevel);
	gfx.FontPrint(fontTimesNewRoman40, 50, baseY - 200, buffer, D3DCOLOR_XRGB(255, 255, 255));

	char buffer6[64];
	sprintf(buffer, "No. Ships Killed Wave: %d/%d", totalShipKillsWave, totalShipCreatedWave);
	gfx.FontPrint(fontTimesNewRoman40, 50, baseY - 250, buffer, D3DCOLOR_XRGB(255, 255, 255));

	//char buffer7[64];
	//sprintf(buffer, "Wave Count : %d/%d", wave, wavesPerLevel);
	//gfx.FontPrint(fontTimesNewRoman40, 1600, 800, buffer, D3DCOLOR_XRGB(255, 255, 255));

	//sprintf(buffer, "Missile Ammo: %d", playerShip.missileAmmunition);
	//gfx.FontPrint(fontGaramond36, 1500, 980, buffer, D3DCOLOR_XRGB(255, 255, 255));

	//sprintf(buffer, "Locked on to: %d", noOfLockedOn);
	//gfx.FontPrint(fontTimesNewRoman40, 1200, 1000, buffer, D3DCOLOR_XRGB(255, 255, 255));

	if (!playerShip.ifHit) gfx.Sprite_Transform_Draw(playerShipTexture, playerShip.x - ((PLAYERDIMENSION * PLAYERSHIPSCALE) / 2), playerShip.y - ((PLAYERDIMENSION * PLAYERSHIPSCALE) / 2), 250, 250, 0, 1, playerShip.rotation, PLAYERSHIPSCALE);
	else gfx.Sprite_Transform_Draw(playerShipTexture, playerShip.x - (PLAYERDIMENSION * PLAYERSHIPSCALE) / 2, playerShip.y - (PLAYERDIMENSION * PLAYERSHIPSCALE) / 2, 250, 250, 0, 1, playerShip.rotation, PLAYERSHIPSCALE, SHIPFLASHCOLOR);

	for (int i = 0; i < enemyShipFleet.size(); i++) {
		//MAYBE ORDER DIFFERENTLY. HE IS NOT HIT more THAN HE IS HIT.
		//actually makes not one bit of difference pretty sure because 1 condition is evualuated either way. Stupid trying to be too efficient when we dont truly know much about efficiency yet anyway.
		if (enemyShipFleet[i].ifHit){
			gfx.Sprite_Transform_Draw(enemyShipTexture, enemyShipFleet[i].x - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2, enemyShipFleet[i].y - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2, ENEMYDIMENSION, ENEMYDIMENSION, 0, 1, enemyShipFleet[i].rotation, ENEMYSHIPSCALE, D3DCOLOR_XRGB(255, 0, 200));
		}
		else gfx.Sprite_Transform_Draw(enemyShipTexture, enemyShipFleet[i].x - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2, enemyShipFleet[i].y - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2, ENEMYDIMENSION, ENEMYDIMENSION, 0, 1, enemyShipFleet[i].rotation, ENEMYSHIPSCALE);
		sprintf(buffer2, "%d", i);
		gfx.FontPrint(fontTimesNewRoman40, enemyShipFleet[i].x - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2, enemyShipFleet[i].y - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2, buffer2, D3DCOLOR_XRGB(255, 255, 255));
	}

	//if (weaponChoice == BULLET) gfx.Sprite_Transform_Draw(bulletIconTexture, 1700, 800, 456, 588, 0, 1, 0.0f, 0.5f);
	//else if (weaponChoice == MISSILE) gfx.Sprite_Transform_Draw(missileIconTexture, 1700, 800, 456, 588, 0, 1, 0.0f, 0.5f);
	
	gfx.Sprite_Transform_Draw(weaponHudTexture, 1500, 650, 388, 385, 0, 1, 0.0f, 1.0f);
	//if (weaponChoice == BULLET) gfx.Sprite_Transform_Draw(bulletIconTexture, 1700, 800, 456, 588, 0, 1, 0.0f, 0.5f);
	//else if (weaponChoice == MISSILE) gfx.Sprite_Transform_Draw(missileIconTexture, 1700, 800, 456, 588, 0, 1, 0.0f, 0.5f);

	for (int i = 0; i < insectKamikazeFleet.size(); i++) {
		if (insectKamikazeFleet[i].ifHit) gfx.Sprite_Transform_Draw(insectKamikazeTexture, insectKamikazeFleet[i].x - (ENEMYDIMENSION * INSECTSCALE) / 2, insectKamikazeFleet[i].y - (ENEMYDIMENSION * INSECTSCALE) / 2, ENEMYDIMENSION, ENEMYDIMENSION, insectKamikazeFleet[i].frame, 15, insectKamikazeFleet[i].rotation - PI / 2, INSECTSCALE, D3DCOLOR_XRGB(255, 0, 200));
		else gfx.Sprite_Transform_Draw(insectKamikazeTexture, insectKamikazeFleet[i].x - (ENEMYDIMENSION * INSECTSCALE) / 2, insectKamikazeFleet[i].y - (ENEMYDIMENSION * INSECTSCALE) / 2, ENEMYDIMENSION, ENEMYDIMENSION, insectKamikazeFleet[i].frame, 15, insectKamikazeFleet[i].rotation - PI / 2, INSECTSCALE);
	}

	for (int i = 0; i < AMOUNTOFVECTORS; i++) {
		for (int j = 0; j < arrayOfVectors[i].size(); j++) {
			gfx.Sprite_Transform_Draw(plasmaBulletTexture, arrayOfVectors[i].at(j).x, arrayOfVectors[i].at(j).y, 10, 10, arrayOfVectors[i].at(j).frame, 5, arrayOfVectors[i].at(j).rotation, 5.0f, D3DCOLOR_XRGB(255,255,255));
		}
	}




	if (weaponChoice == MISSILE) {
		for (int i = 0; i < pShipTargets.size(); i++) {
			/*
			Why / 4?????????????????????	
			*/
			sprintf(buffer2, "%d", i);
			gfx.FontPrint(fontTimesNewRoman40, enemyShipFleet[i].x - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2 + 20, enemyShipFleet[i].y - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2, buffer2, D3DCOLOR_XRGB(255, 0, 0));
			if (pShipTargets[i].inMouseRange) gfx.Sprite_Transform_Draw(missileLockTexture, pShipTargets[i].ship->x - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2 - (400 * LOCKONSCALE) / 4,
				pShipTargets[i].ship->y - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2 - (400 * LOCKONSCALE) / 4, 400, 400, pShipTargets[i].frame, 8, 0.0f, LOCKONSCALE);
			else if (pShipTargets[i].lockedOn) gfx.Sprite_Transform_Draw(missileLockTexture, pShipTargets[i].ship->x - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2 - (400 * LOCKONSCALE) / 4,
				pShipTargets[i].ship->y - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2 - (400 * LOCKONSCALE) / 4, 400, 400, pShipTargets[i].frame, 8, 0.0f, LOCKONSCALE);
		}
	}


	






	for (int i = 0; i < enemyBulletVector.size(); i++) {
		//gfx.DrawDisc(enemyBulletVector[i].x, enemyBulletVector[i].y, 5, enemyBulletVector[i].r, enemyBulletVector[i].g, enemyBulletVector[i].b);
		gfx.Sprite_Transform_Draw(plasmaBulletTexture, enemyBulletVector[i].x, enemyBulletVector[i].y, 10, 10, enemyBulletVector[i].frame, 5, enemyBulletVector[i].rotation, 5.0f, D3DCOLOR_XRGB(255, 50, 50));
	}

	for (int i = 0; i < missileVector.size(); i++) {
		gfx.Sprite_Transform_Draw(missileTexture, missileVector[i].x, missileVector[i].y, 215, 126, missileVector[i].frame, 5, missileVector[i].rotation, 0.5f);
	}

	for (int i = 0; i < explosionVector.size(); i++) {
		gfx.Sprite_Transform_Draw(explosionTexture, explosionVector[i].x - (EXPLOSIONDIMENSION * explosionVector[i].scale) / 2, explosionVector[i].y - (EXPLOSIONDIMENSION * explosionVector[i].scale) / 2, 300, 300, explosionVector[i].frame, 12, explosionVector[i].rotation, explosionVector[i].scale, D3DCOLOR_XRGB(explosionVector[i].r, explosionVector[i].g, explosionVector[i].b));
	}

	spriteobj2->End();

	//gfx.DrawLine(cursorX, cursorY, x + 125, y + 125, 0, 255, 0);
	//gfx.DrawDisc(playerShip.x, playerShip.y, 12, 255, 255, 0);
}


/*
TO DO::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
(general order that we should do it in)

- Have 3 levels with boss at end before christmas!!!! All generally main mechanics done. Make code easier to look at through abstraction through more functions or even classes.
- bullets come out of both sides
- fire 6 missiles at once
- animate text function.
- change a few vectors into deque's
- collision detection and collision response function. will make it easy to change collision to be better with dot product, bounding box, and distance. 
- colision response with clever parameters.
- updateenemyship1sfunction plus updateinsectsfunction
- abstract some thing that can kill an enemy into a function with variables and inside it evaluateds collision aswell. Bounding box maybe and dot product. Just so when new weapons or new enemys are put in its easy to try add in everything. Makes it only one function to change.
- missile lock on system. 1 missile to kill most enemy. Missile texture.
- Update Insects and update enemyship1 function
- rotation of player isnt super accurate coz of centre. Also bullets aren't going exactly to right place maybe for a different reason.
- Text saying GameOver and an intro text saying round one wave one level one stage one
- insects move in bursts.
- Top or everywhere orientated.
- make bounding box collision function. work same as what we have now. so many things collide so very important to have a function. Then for more accurate cases after collision is detected use pixel perfect or distance.
- Acceleration for both player and enemyships. Takes a while to go full speed. more realistic and can solve other problems in the game. int speed variable instead of constant ahaaaa
- bullets come out of each side separately.
- press button to activate shield for certain amount of time and bullets  and time make it go down too
- make it more top orientatated so you dont have to go very far up. So you only move backwards or so.
- Initial level1 function with 5-10 waves and 5 testwave functions to try A LOT of different thigns to make the game exciting. Even just making the enemies follow in funny patterns each different wave.
- 5 Level functions (e.g.) and inside each maybe 5-10 Wave functions. Or even a parameterisable function to avoid 50 functions. Parameters (int insects, int enemyship1s, int timeBetweenWaves, int timeBetweengroups, int insectGroupsize, int enemyship1Groupsize)
- get a weapon on the hud showing if missile or plasma is on.
- fix boundaries so they never get stuck. Maybe just make them move away from them or something.
- Try simple laser aswell.
- make enemys y equal x^2 or something or log(2). Eventually find some function that always has a base point of the playership so we can make rotate around player.
- Get Text in game with funny and informative comments. If gameover or on next wave etc.
- shaking if hit
- fullscreen
- powerups for better shield, hull, bullets, more ships to lock on to for missile, more ammunition.
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
- round one wave one level one stage one
- When play is clicked for first time and first wave. introduces first two enemies and what to do with them with a picture, picture is animated scaled up and stops scaling so looks pro :).
- for one boss maybe have it like alien and you see him on the minimap but you have to avoid him in this big maze or something and he scares the shit out of you. The faster you avoid him the more he dies? 
	Think of an actual reason for this or actually tell the player "fuck you, it doesnt have to make sense"
- multiplayer with 360 controllers or eventually lan.
- tutorial at the start explaining that bullets will go directly were you fire at that moment regardless of how you move.

//STORY AND TEXT TO PUT IN:
- Big twist is to kill yourself. You are the boss. Maybe mirrors or some game mechanic like that to kill yourself.
- You are your own worst enemy.
- loads of clues throughout the game hinting that you are the enemy. Enemies give hints that you are a bad bad man. Keep your friends close but your enemies even closer. And you cant get closer than yourself. look in the mirror.
- In space I can hear you scream.
- round one: survive. Round two: survive again. Round three: Ur shit. Round four: ah for feck sake.
- we need reinforcements
- speech bubbles?

THINGS THAT AFFECT THE DIFFICULTY/FUN BALANCE
1. What weapons you have, how much ammunition for each, how much damage does each do (keep plasma at 1 for a nice base value).
2. How much health and shield you have and how easy it is to get more through powerups
3. Amount of enemies, how much health they have, how fast they are.
4. Enemy rate of fire and speed of enemies and their bullets

WEEKLY LOG / PROGRESS REPORT:
	BEFORE WEEK 1: Only ship 3 in game with old sprite code and was reticle game before with vectors of bullets ready.

	WEEK 1: 17th of September: Bullets going in any direction, which pretty much unlocks all angle work to be done. Clean up of all reticle game shite.
	WEEK 2: 24nd of September: Rotation almost working but slightly off with 3 ships finally in game.
	WEEK 3:	1st of October: Major breakthrough week. Got all ships rotating perfectly with ROTATION FUNCTION. Got vectors of enemy ships and made them move. If wave killed, it respawns.
	WEEK 4: 8th of October: Flashing if any ship hit. Game over with health bar and enemy ships shooting bullets so officially could be called a game. Began work on scaling.
	WEEK 5: 15th of October: Scaling with rotation+collision+boundaries 100% perfect!!! Makes it possible for anything to be drawn in any pixel dimensions and then be scaled. Initial missile following mouse cursor implemented. Kamikaze hurts player and can super officially call it a game.
	WEEK 6: 22nd of October: Explosions with different colours and rotation. Weapon switch on hud. Text into game. Github working.
	WEEK 6: (same week as last): 26th of October: createExplosion Function. Missile lock on looking good almost there, more specifics to do. Releases. inheritance with all objects.
	WEEK 7: Week of 27th of October: Enemies come at different times, wave+squads+timer+more of helpful variables. Squads. Polymorphism and more inheritance. Make Enemy Function. Vector reserve. 
	WEEK 8: Week of 3rd of November: Made squads better. 2 Lock on sessions and finally got it 90%. Understood erase bug, as to why both squads and lock on arent working. Will never make reference to vector mistake again.

	WEEK 9: Week of 17th of November: Plan: Hopefully get two sessions, missed last week. Bullets from both side of ship (easyyy). Finish lock on 100%. Gameover and intro text maybe animated intro too. better ai+Better collision avoidance.
	
		
//TINY LITTLE SMALL NOTES::::::::::::::::::::::::::::::::::::::::::::::
*/