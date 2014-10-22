/****************************************************************************************** 
 *	Chili DirectX Framework Version 12.04.24											  *	
 *	Game.h																				  *
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
#pragma once

#include "D3DGraphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Sound.h"
#include <vector>

using std::vector;

/*#define LEFTBOUNDARY 5
#define RIGHTBOUNDARY 1361
#define UPPERBOUNDARY 5
#define LOWERBOUNDARY 763*/

#define ROTATIONAMOUNT 0.0001f
#define PI 3.14159265

#define SIZEOFENEMYSHIPFLEET 15
#define SIZEOFINSECTFLEET 15

#define PLAYERBULLETSPEED 10.0f
#define ENEMYBULLETSPEED 2.3f
#define PLAYERMSSILESPEED 7

#define PLAYERSHIPLIVES 20
#define FRAMESPERFLASH 5
#define SHIPFLASHCOLOR D3DCOLOR_XRGB(255, 0, 200)

#define ENEMYDIMENSION 150
#define PLAYERDIMENSION 250

#define INSECTSCALE 0.5f
#define ENEMYSHIPSCALE 0.7f
//make it 2.0f just to prove the point about constants and how shit hardcoding is
#define PLAYERSHIPSCALE 0.5f

const int AMOUNTOFVECTORS = 1;

struct bullet 
{
	float x, y;
	int r,g,b; //keep these and draw multicoloured plasma at certain stages of the game or always
	float vx, vy;
	float rotation;  //maybe not needed just set once but then again maybe needed coz everything has to stay rotated
	int frame;
	int frameExposure;
};

struct missile
{
	float x, y;
	float vx, vy;
	float rotation;  //maybe not needed just set once but then again maybe needed coz everything has to stay rotated
	int frame;
	int frameExposure;
};

struct Ship
{
	float x, y;
	float vx, vy;
	int lives;
	float rotation;
	bool ifHit;
	short framesSinceHit;
	//D3DCOLOR color; Maybe doesnt have to be part of each ship for flash hit effect. But then again maybe it does.
};

enum weaponChoice{
	BULLET,
	MISSILE,
	LASER
};

class Game
{
public:
	//ALL FUNCTIONS:
	Game( HWND hWnd,const KeyboardServer& kServer,const MouseServer& mServer);
	~Game();
	void Go();
	int LEFTBOUNDARY;
	int RIGHTBOUNDARY;
	int UPPERBOUNDARY;
	int LOWERBOUNDARY;
	bool murrayPlaying = false;
private:
	//DRAWING
	void ComposeFrame();
	void drawRectangle(int TopCornerX, int TopCornerY, int width, int height, int r, int g, int b);
	void drawReticle(int x, int y, int r, int g, int b);
	void drawHealth();

	//LOGIC
	void playerShipLogic();
	void updateEnemy();
	void bulletLogic();
	void createBullet(bool ifPlayer);
	void createMissile(bool ifPlayer);
	void setUpShips();
	void findRotation(int x1, int y1, int x2, int y2, float& rotation);
private:
	//ALL VARIABLES:
	D3DGraphics gfx;
	KeyboardClient kbd;
	MouseClient mouse;
	DSound audio;

	LPDIRECT3DTEXTURE9 playerShipTexture;
	LPDIRECT3DTEXTURE9 enemyShipTexture;
	LPDIRECT3DTEXTURE9 insectKamikazeTexture;

	LPDIRECT3DTEXTURE9 plasmaBullet;
	LPDIRECT3DTEXTURE9 testBallTexture;

	weaponChoice weaponChoice;
	float speed;	

	Ship playerShip;
	vector<Ship> enemyShipFleet;
	vector<Ship> insectKamikazeFleet;

	vector<bullet> arrayOfVectors[AMOUNTOFVECTORS];
	vector<missile> missileVector;

	vector<bullet> enemyBulletVector;

	//sprinkler
	float xVel, yVel;
	bool changeDirection;
	bool clockwise;

	int wave;

	bool keysPressedLastFrame;
	bool gameOver;
	bool paused;
	bool invincible;
	int frameCount;
};