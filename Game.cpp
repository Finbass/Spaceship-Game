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

	//sprinkler stuff, stupid maybe
	/*
	changeDirection = false;
	clockwise = true;
	xVel = 0;
	yVel = 0;
	*/

	srand(static_cast <unsigned> (time(0)));

	plasmaShot = audio.CreateSound("tinkle.wav");

	playerShip.x = 900;
	playerShip.y = 600;
	playerShip.lives = PLAYERSHIPLIVES;
	playerShip.rotation = 0.0f;
	playerShip.framesSinceHit = 0;
	playerShip.ifHit = 0;
	speed = 8.0f;
	weaponChoice = BULLET;

	fontArial24 = gfx.MakeFont("Arial", 24);
	fontGaramond36 = gfx.MakeFont("Garamond", 36);
	fontTimesNewRoman40 = gfx.MakeFont("Times new Roman", 40);

	wave = 1;
	waveTimer = 0;
	nextSquadTimer = 0;
	nextEnemyTimer = 0;
	start = time(0);	
	doneIt = false; //rename
	minTimeBetweenWaves = 3;
	timeTillNextSquad = minTimeBetweenWaves;
	totalEnemyShipCreatedLevel = 0;
	totalEnemyShipCreatedWave = 0;
	squadsCreatedLevel = 0;
	squadsDefeatedLevel = 0;
	squadsCreatedWave = 0;
	squadsDefeatedWave = 0;
	squadsPerWave = 15;
	totalInsectCreatedLevel = 0;
	totalInsectCreatedWave = 0;

	//hahaaa very interesting. Explain reservations. vectors dynamic memory. Massive bug.
	insectKamikazeFleet.reserve(30);
	enemyShipFleet.reserve(30);

	totalEnemyShipKillsLevel = 0;
	totalEnemyShipKillsWave = 0;
	totalInsectKillsLevel = 0;
	totalInsectKillsWave = 0;

	totalShipCreatedLevel = 0;
	totalShipCreatedWave = 0;
	totalShipKillsLevel = 0;
	totalShipKillsWave = 0;

	wavesPerLevel = 3;
	//setUpShips();

	playerShipTexture = gfx.LoadTexture("Player ship 3.bmp", D3DCOLOR_XRGB(0, 0, 0));
	enemyShipTexture = gfx.LoadTexture("Ship 3.bmp");
	insectKamikazeTexture = gfx.LoadTexture("insectAnm 15 frame bmp.bmp", D3DCOLOR_XRGB(0, 148, 255));
	plasmaBulletTexture = gfx.LoadTexture("Blue ball bullet 1.bmp", D3DCOLOR_XRGB(0, 0, 0));
	missileTexture = gfx.LoadTexture("Bomb 1 (cropped).bmp");
	explosionTexture = gfx.LoadTexture("Explosion 2 sprite sheet (2) shaded.bmp", D3DCOLOR_XRGB(33, 198, 255));
	missileIconTexture = gfx.LoadTexture("Bomb Icon 1.bmp");
	bulletIconTexture = gfx.LoadTexture("Laser Icon 1.bmp");
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
	/*for (int i = 0; i < 100000000; i++)
	{
		;
	}*/
	/*for (int i = 0; i < 10000000; i++)
	{
		;
	}*/

	if (!paused) {
		if (!gameOver) {
			secondsSinceStart = difftime(time(0), start);
			waveTimer = secondsSinceStart;
			nextSquadTimer = secondsSinceStart;
			nextEnemyTimer = secondsSinceStart;

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
	const int maxAmountInSquad = 4;
	if (wave == 1) {
		if (nextSquadTimer % timeTillNextSquad == 0 && nextSquadTimer != 0 && !doneIt && squadsCreatedWave < squadsPerWave) {
			int shipType;
			int noOfShipsInSquad = rand() % maxAmountInSquad + 1;
			Squad squad;
			squad.shipArray.clear();
			squadVector.push_back(squad);
			//squadVector.push_back(Squad());
			for (int i = 0; i < noOfShipsInSquad; i++) {
				shipType = rand() % 2;
				shipType = 1;
				if (shipType == 0) { totalEnemyShipCreatedLevel++; totalEnemyShipCreatedWave++;}
				else if (shipType == 1) { totalInsectCreatedLevel++; totalInsectCreatedWave++; }
				totalShipCreatedLevel = totalEnemyShipCreatedLevel + totalInsectCreatedLevel;
				totalShipCreatedWave = totalEnemyShipCreatedWave + totalInsectCreatedWave;	
				makeEnemy(shipType, squadsCreatedWave);
			}
			timeTillNextSquad = rand() % 4 + 4;
			doneIt = true;
			squadsCreatedLevel++;
			squadsCreatedWave++;
		}
		else if (nextSquadTimer % timeTillNextSquad != 0 && doneIt) doneIt = false;
		if (squadsDefeatedWave >= squadsPerWave) {
			wave++;
			totalShipCreatedWave = 0; //not needed but fuck it for now
			totalEnemyShipCreatedWave = 0;
			totalInsectCreatedWave = 0;
			squadsCreatedWave = 0;
			squadsDefeatedWave = 0;
		}
	}
	
	
	if (wave == 2) {


	}

	/* have a laugh at this
	if (nextSquadTimer % 2 == 0 && !doneIt) {
		int a = rand() % 2;
		makeEnemy(a);
		doneIt = true;
	}
	else doneIt = false;
	*/

	/*for (int i = 0; i < SIZEOFENEMYSHIPFLEET; i++) {
		Ship enemyShip;
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
	}

	for (int i = 0; i < SIZEOFINSECTFLEET; i++) {
		Ship insect;
		insect.ifHit = false;
		insect.framesSinceHit = 0;
		insect.lives = 15;
		insect.speed = INSECTSPEED;
		insect.y = (ENEMYDIMENSION * INSECTSCALE) / 2 + 20;
		insect.x = rand() % (RIGHTBOUNDARY - ENEMYDIMENSION);
		insect.frame = 0;
		insect.frameExposure = 0;
		insectKamikazeFleet.push_back(insect);
	}*/
}

void Game::makeEnemy(int whichShip, int squadCreatedCount) {
	squadCreatedCount = 0;
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
		//squadVector[squadCreatedCount].shipArray.push_back(&enemyShipFleet.back());
		//squadVector[squadCreatedCount].shipArray.push_back(&enemyShipFleet[(enemyShipFleet.size() - 1)]);
		EnemyShip1* pEnemyShip = &enemyShipFleet[(enemyShipFleet.size() - 1)];
		squadTest.shipArray.push_back(pEnemyShip);
		//squadTest.shipArray.push_back(&enemyShipFleet[(enemyShipFleet.size() - 1)]);
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
		//squadVector[squadCreatedCount].shipArray.push_back(&insectKamikazeFleet.back());
		//squadVector[squadCreatedCount].shipArray.push_back(&insectKamikazeFleet[(insectKamikazeFleet.size() - 1)]);
		//squadTest.shipArray.push_back(&insectKamikazeFleet[(insectKamikazeFleet.size() - 1)]);
		Insect* pInsectShip = &insectKamikazeFleet[(insectKamikazeFleet.size() - 1)];
		//Insect* pInsectShip = &insect;
		squadTest.shipArray.push_back(pInsectShip);
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

	if (insectKamikazeFleet.size() == 0 && enemyShipFleet.size() == 0) {
		//setUpShips();
		//wave++;
		//different now.
	}

	int i;
	int k;
	for (i = 0; i < squadVector.size(); i++) {
		bool squadDefeated = false;
		int allShipsInSquadDefeated = 0;
		int shipsInSquad = squadVector[i].shipArray.size();
		for (k = 0; k < squadVector[i].shipArray.size(); k++) {
			if (squadVector[i].shipArray[k]->lives <= 0) {
				allShipsInSquadDefeated++;
				//squadVector[i].shipArray.erase(squadVector[i].shipArray.begin() + k);
			}
		}
		//if (allShipsInSquadDefeated == squadVector[i].shipArray.size()) squadDefeated = true;
		if (allShipsInSquadDefeated == shipsInSquad) {
		//if (squadDefeated) {
			squadsDefeatedLevel++;
			squadsDefeatedWave++;
			squadVector.erase(squadVector.begin() + i);
		//}
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
	/*********************************************************************************************

	ENEMY SHIP 1S

	***********************************************************************************************/

	//actually we should just make updateInsect and update enemyship functions here

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
					if (enemyShipFleet[k].lives > 1) {
						enemyShipFleet[k].lives--;
						enemyShipFleet[k].ifHit = true;
						enemyShipFleet[k].framesSinceHit = 0;
					}
					else {
						createExplosion(explosion(), enemyShipFleet[k].x, enemyShipFleet[k].y, enemyShipFleet[k].rotation, 0.6f, rand() % 255, rand() % 255, rand() % 255);
						for (int i = 0; i < lockedOnVector.size(); i++) {
							if (lockedOnVector[i] == &enemyShipFleet[k]) lockedOnVector.erase(lockedOnVector.begin());
						}
						enemyShipFleet.erase(enemyShipFleet.begin() + k);
						totalEnemyShipKillsLevel++;
						totalEnemyShipKillsWave++;
						break;
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
				missileVector.erase(missileVector.begin() + i);
				if (enemyShipFleet[k].lives > 1) {
					enemyShipFleet[k].lives = enemyShipFleet[k].lives - 35;
					enemyShipFleet[k].ifHit = true;
					enemyShipFleet[k].framesSinceHit = 0;
				}
				if (enemyShipFleet[k].lives < 1) {
					createExplosion(explosion(), enemyShipFleet[k].x, enemyShipFleet[k].y, enemyShipFleet[k].rotation, 0.6f, 255, 255, 255);
					for (int i = 0; i < lockedOnVector.size(); i++) {
						if (lockedOnVector[i] == &enemyShipFleet[k]) lockedOnVector.erase(lockedOnVector.begin());
					}
					enemyShipFleet.erase(enemyShipFleet.begin() + k);
					totalEnemyShipKillsLevel++;
					totalEnemyShipKillsWave++;
					break;
				}
			}
		}
	}

	if (frameCount % 60 == 0 && enemyShipFleet.size() > 0) createBullet(false);
}

void Game::updateInsects() {
	/*********************************************************************************************

	INSECTS!

	***********************************************************************************************/
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
					if (insectKamikazeFleet[k].lives > 1) {
						insectKamikazeFleet[k].lives--;
						insectKamikazeFleet[k].ifHit = true;
						insectKamikazeFleet[k].framesSinceHit = 0;
					}
					else {
						createExplosion(explosion(), insectKamikazeFleet[k].x, insectKamikazeFleet[k].y, insectKamikazeFleet[k].rotation, 0.55f, 0, 255, 0);
						insectKamikazeFleet.erase(insectKamikazeFleet.begin() + k);
						totalInsectKillsLevel++;
						totalInsectKillsWave++;
						break;
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
				if (insectKamikazeFleet[k].lives > 1) {
					insectKamikazeFleet[k].lives = insectKamikazeFleet[k].lives - 35;
					insectKamikazeFleet[k].ifHit = true;
					insectKamikazeFleet[k].framesSinceHit = 0;
				}
				if (insectKamikazeFleet[k].lives < 1) {
					createExplosion(explosion(), insectKamikazeFleet[k].x, insectKamikazeFleet[k].y, insectKamikazeFleet[k].rotation, 0.55f, 0, 255, 0);
					for (int i = 0; i < lockedOnVector.size(); i++) {
						if (lockedOnVector[i] == &insectKamikazeFleet[k]) lockedOnVector.erase(lockedOnVector.begin());
					}
					insectKamikazeFleet.erase(insectKamikazeFleet.begin() + k);
					totalInsectKillsLevel++;
					totalInsectKillsWave++;
					break;

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
		if (mouse.LeftIsPressed() && !mouseClicked){
			if (lockedOnVector.size() > 0) createMissile(true);
			keysPressedLastFrame = true;
			mouseClicked = true;
		}
		if (mouse.RightIsPressed()){
			createLockOn();
		}
		else {
			Locking = false;
			for (int k = 0; k < lockedOnVector.size(); k++) lockedOnVector.erase(lockedOnVector.begin() + k);
		}
		if (!mouse.LeftIsPressed()) mouseClicked = false;
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
		if (lockedOnVector.size() > 0) findRotation(missileVector[j].x, missileVector[j].y, lockedOnVector[0]->x, lockedOnVector[0]->y, missileVector[j].rotation);
		else findRotation(missileVector[j].x, missileVector[j].y, mouse.GetMouseX(), mouse.GetMouseY(), missileVector[j].rotation);
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

void Game::createMissile(bool ifPlayer) {
	missile missile1;
	missile1.frameExposure = 0;
	missile1.frame = 0;

	if (ifPlayer) {
		missile1.x = playerShip.x;
		missile1.y = playerShip.y;
		if (lockedOnVector.size() > 0) findRotation(missile1.x, missile1.y, lockedOnVector[0]->x, lockedOnVector[0]->y, missile1.rotation);
		else findRotation(missile1.x, missile1.y, mouse.GetMouseX(), mouse.GetMouseY(), missile1.rotation);

		missile1.vx = cos(missile1.rotation) * PLAYERMSSILESPEED;
		missile1.vy = sin(missile1.rotation) * PLAYERMSSILESPEED;

		missileVector.push_back(missile1);
	}

	else {
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

void Game::createLockOn(){
	int lockOnTime = 3000;
	if (!Locking) {
		for (int i = 0; i < insectKamikazeFleet.size(); i++) {
			if (Distance(mouse.GetMouseX(), mouse.GetMouseY(), insectKamikazeFleet[i].x, insectKamikazeFleet[i].y) < 100) {
				Locking = true;
				lockedOnVector.push_back(&insectKamikazeFleet[i]);
				startTime = (int)GetTickCount();
				break;
			}
		}
		if (!Locking) {
			for (int i = 0; i < enemyShipFleet.size(); i++) {
				if (Distance(mouse.GetMouseX(), mouse.GetMouseY(), enemyShipFleet[i].x, enemyShipFleet[i].y) < 100) {
					Locking = true;
					lockedOnVector.push_back(&enemyShipFleet[i]);
					startTime = (int)GetTickCount();
					break;
				}
			}
		}
	}
	else{
		if (lockedOnVector.size() > 0){
			if (Distance(mouse.GetMouseX(), mouse.GetMouseY(), lockedOnVector[0]->x, lockedOnVector[0]->y) < 100){
				if ((int)GetTickCount() - startTime > 3000) {
					lockedOnVector[0]->lockedOnTo = true;
				}
			}
			else{
				startTime = (int)GetTickCount();
				lockedOnVector[0]->lockedOnTo = false;
				for (int k = 0; k < lockedOnVector.size(); k++) lockedOnVector.erase(lockedOnVector.begin() + k);
				Locking = false;
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

//void Game::writeText(Font font, x, y, D3DCOLOR_XRGB)

void Game::ComposeFrame()
{	
	spriteobj2->Begin(D3DXSPRITE_ALPHABLEND);
	drawHealth();

	const int baseY = 900;
	//quick function that does this char buffer thing. UP ABOVE
	char buffer[64];
	sprintf(buffer, "Seconds elapsed: %.2f", secondsSinceStart);
	gfx.FontPrint(fontGaramond36, 50, baseY, buffer, D3DCOLOR_XRGB(255, 255, 255));

	char buffer2[64];
	sprintf(buffer2, "Squads defeated: %d/%d", squadsDefeatedWave, squadsPerWave);
	gfx.FontPrint(fontTimesNewRoman40, 50, baseY - 50, buffer2, D3DCOLOR_XRGB(255, 255, 255));

	char buffer8[64];
	sprintf(buffer8, "Squads created: %d/%d", squadsCreatedWave, squadsPerWave);
	gfx.FontPrint(fontTimesNewRoman40, 50, baseY - 100, buffer8, D3DCOLOR_XRGB(255, 255, 255));

	char buffer3[64];
	sprintf(buffer3, "InsectsNoLevel: %d enemyShipNoLevel: %d",totalInsectCreatedLevel, totalEnemyShipCreatedLevel);
	gfx.FontPrint(fontTimesNewRoman40, 50, baseY - 150, buffer3, D3DCOLOR_XRGB(255, 255, 255));

	char buffer4[64];
	sprintf(buffer4, "InsectsNoWave: %d enemyShipNoWave: %d", totalInsectCreatedWave, totalEnemyShipCreatedWave);
	gfx.FontPrint(fontTimesNewRoman40, 50, baseY - 200, buffer4, D3DCOLOR_XRGB(255, 255, 255));

	char buffer5[64];
	sprintf(buffer5, "No. Ships Killed Level: %d/%d", totalShipKillsLevel, totalShipCreatedLevel);
	gfx.FontPrint(fontTimesNewRoman40, 50, baseY - 250, buffer5, D3DCOLOR_XRGB(255, 255, 255));

	char buffer6[64];
	sprintf(buffer6, "No. Ships Killed Wave: %d/%d", totalShipKillsWave, totalShipCreatedWave);
	gfx.FontPrint(fontTimesNewRoman40, 50, baseY - 300, buffer6, D3DCOLOR_XRGB(255, 255, 255));

	char buffer7[64];
	sprintf(buffer7, "Wave Count : %d/%d", wave, wavesPerLevel);
	gfx.FontPrint(fontTimesNewRoman40, 1600, 800, buffer7, D3DCOLOR_XRGB(255, 255, 255));

	for (int i = 0; i < squadTest.shipArray.size(); i++) {
		char buffer9[64];
		sprintf(buffer7, "Address: %d", squadTest.shipArray[i], wavesPerLevel);
		gfx.FontPrint(fontTimesNewRoman40, 1100, 400 + i*40, buffer7, D3DCOLOR_XRGB(255, 255, 255));
	}

	for (int i = 0; i < insectKamikazeFleet.size(); i++) {
		char buffer9[64];
		sprintf(buffer7, "Address: %d", &insectKamikazeFleet[i], wavesPerLevel);
		gfx.FontPrint(fontTimesNewRoman40, 1500, 500 + i*40, buffer7, D3DCOLOR_XRGB(255, 255, 255));
	}
	


	/*char buffer[64];
	sprintf(buffer, "Insect Count: %d", insectKamikazeFleet.size());
	gfx.FontPrint(fontGaramond36, 35, 20, buffer, D3DCOLOR_XRGB(255, 255, 255));

	char buffer2[64];
	sprintf(buffer2, "Enemyship1 Count: %d", enemyShipFleet.size());
	gfx.FontPrint(fontTimesNewRoman40, 700, 20, buffer2, D3DCOLOR_XRGB(255, 255, 255));*/

	if (!playerShip.ifHit) gfx.Sprite_Transform_Draw(playerShipTexture, playerShip.x - ((PLAYERDIMENSION * PLAYERSHIPSCALE) / 2), playerShip.y - ((PLAYERDIMENSION * PLAYERSHIPSCALE) / 2), 250, 250, 0, 1, playerShip.rotation, PLAYERSHIPSCALE);
	else gfx.Sprite_Transform_Draw(playerShipTexture, playerShip.x - (PLAYERDIMENSION * PLAYERSHIPSCALE) / 2, playerShip.y - (PLAYERDIMENSION * PLAYERSHIPSCALE) / 2, 250, 250, 0, 1, playerShip.rotation, PLAYERSHIPSCALE, SHIPFLASHCOLOR);

	for (int i = 0; i < enemyShipFleet.size(); i++) {
		//MAYBE ORDER DIFFERENTLY. HE IS NOT HIT more THAN HE IS HIT.
		//actually makes not one bit of difference pretty sure because 1 condition is evualuated either way. Stupid trying to be too efficient when we dont truly know much about efficiency yet anyway.
		if (enemyShipFleet[i].ifHit){
			gfx.Sprite_Transform_Draw(enemyShipTexture, enemyShipFleet[i].x - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2, enemyShipFleet[i].y - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2, ENEMYDIMENSION, ENEMYDIMENSION, 0, 1, enemyShipFleet[i].rotation, ENEMYSHIPSCALE, D3DCOLOR_XRGB(255, 0, 200));
		}
		else gfx.Sprite_Transform_Draw(enemyShipTexture, enemyShipFleet[i].x - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2, enemyShipFleet[i].y - (ENEMYDIMENSION * ENEMYSHIPSCALE) / 2, ENEMYDIMENSION, ENEMYDIMENSION, 0, 1, enemyShipFleet[i].rotation, ENEMYSHIPSCALE);
	}

	if (weaponChoice == BULLET) gfx.Sprite_Transform_Draw(bulletIconTexture, 1700, 800, 456, 588, 0, 1, 0.0f, 0.5f);
	else if (weaponChoice == MISSILE) gfx.Sprite_Transform_Draw(missileIconTexture, 1700, 800, 456, 588, 0, 1, 0.0f, 0.5f);

	for (int i = 0; i < insectKamikazeFleet.size(); i++) {
		if (insectKamikazeFleet[i].ifHit) gfx.Sprite_Transform_Draw(insectKamikazeTexture, insectKamikazeFleet[i].x - (ENEMYDIMENSION * INSECTSCALE) / 2, insectKamikazeFleet[i].y - (ENEMYDIMENSION * INSECTSCALE) / 2, ENEMYDIMENSION, ENEMYDIMENSION, insectKamikazeFleet[i].frame, 15, insectKamikazeFleet[i].rotation - PI / 2, INSECTSCALE, D3DCOLOR_XRGB(255, 0, 200));
		else gfx.Sprite_Transform_Draw(insectKamikazeTexture, insectKamikazeFleet[i].x - (ENEMYDIMENSION * INSECTSCALE) / 2, insectKamikazeFleet[i].y - (ENEMYDIMENSION * INSECTSCALE) / 2, ENEMYDIMENSION, ENEMYDIMENSION, insectKamikazeFleet[i].frame, 15, insectKamikazeFleet[i].rotation - PI / 2, INSECTSCALE);
	}

	for (int i = 0; i < AMOUNTOFVECTORS; i++) {
		for (int j = 0; j < arrayOfVectors[i].size(); j++) {
			//gfx.Sprite_Transform_Draw(plasmaBullet, arrayOfVectors[i].at(j).x, arrayOfVectors[i].at(j).y, 10, 10, arrayOfVectors[i].at(j).frame, 5, arrayOfVectors[i].at(j).rotation, 5.0f, D3DCOLOR_XRGB(arrayOfVectors[i].at(j).r, arrayOfVectors[i].at(j).g, arrayOfVectors[i].at(j).b));
			gfx.Sprite_Transform_Draw(plasmaBulletTexture, arrayOfVectors[i].at(j).x, arrayOfVectors[i].at(j).y, 10, 10, arrayOfVectors[i].at(j).frame, 5, arrayOfVectors[i].at(j).rotation, 5.0f, D3DCOLOR_XRGB(255,255,255));
		}
	}
	for (int i = 0; i < lockedOnVector.size(); i++){
		if (lockedOnVector[i]->lockedOnTo) gfx.DrawCircle(lockedOnVector[i]->x, lockedOnVector[i]->y, (ENEMYDIMENSION * INSECTSCALE) / 2 + 10, 255, 0, 0);
		else gfx.DrawCircle(lockedOnVector[i]->x, lockedOnVector[i]->y, (ENEMYDIMENSION * INSECTSCALE) / 2 + 10, 0, 255, 0);
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

- animate text function.
- collision detection and collision response function. will make it easy to change collision to be better with dot product, bounding box, and distance. 
- colision response with clever parameters.
- updateenemyship1sfunction plus updateinsectsfunction
- abstract some thing that can kill an enemy into a function with variables and inside it evaluateds collision aswell. Bounding box maybe and dot product. Just so when new weapons or new enemys are put in its easy to try add in everything. Makes it only one function to change.
- missile lock on system. 1 missile to kill most enemy. Missile texture.
- Update Insects and update enemyship1 function
- rotation of player isnt super accurate coz of centre. Also bullets aren't going exactly to right place maybe for a different reason.
- Text saying GameOver and an intro text saying round one wave one level one stage one
- insects move in bursts.
- make bounding box collision function. work same as what we have now. so many things collide so very important to have a function. Then for more accurate cases after collision is detected use pixel perfect or distance.
- Acceleration for both player and enemyships. Takes a while to go full speed. more realistic and can solve other problems in the game. int speed variable instead of constant ahaaaa
- bullets come out of each side separately.
- press button to activate shield for certain amount of time and bullets  and time make it go down too
- make it more top orientatated so you dont have to go very far up. So you only move backwards or so.
- better ai.
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

WEEKLY LOG / PROGRESS REPORT:
	BEFORE WEEK 1: Only ship 3 in game with old sprite code and was reticle game before with vectors of bullets ready.

	WEEK 1: 17th of September: Bullets going in any direction, which pretty much unlocks all angle work to be done. Clean up of all reticle game shite.
	WEEK 2: 24nd of September: Rotation almost working but slightly off with 3 ships finally in game.
	WEEK 3:	1st of October: Major breakthrough week. Got all ships rotating perfectly with ROTATION FUNCTION. Got vectors of enemy ships and made them move. If wave killed, it respawns.
	WEEK 4: 8th of October: Flashing if any ship hit. Game over with health bar and enemy ships shooting bullets so officially could be called a game. Began work on scaling.
	WEEK 5: 15th of October: Scaling with rotation+collision+boundaries 100% perfect!!! Makes it possible for anything to be drawn in any pixel dimensions and then be scaled. Initial missile following mouse cursor implemented. Kamikaze hurts player and can super officially call it a game.
	WEEK 6: 22nd of October: Explosions with different colours and rotation. Weapon switch on hud. Text into game. Github working.
	WEEK 6: (same week as last): 26th of October: createExplosion Function. Missile lock on looking good almost there, more specifics to do. Releases. inheritance with all objects.

	WEEK 7: Over the week: Enemies come at different times, wave+squads+timer+hundreds of helpful variables. Squads. Polymorphism and more inheritance. Make Enemy Function.
	Gameover and intro text maybe animated intro too. Make 2 sounds to try(maybe). Top or everywhere orientated. better ai+Better collision avoidance.
		
//TINY LITTLE SMALL NOTES::::::::::::::::::::::::::::::::::::::::::::::
*/