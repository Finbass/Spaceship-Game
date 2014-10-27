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

#define ROTATIONAMOUNT 0.0001f
#define PI 3.14159265

#define SIZEOFENEMYSHIPFLEET 15
#define SIZEOFINSECTFLEET 10

//#define PLAYERBULLETSPEED 3.0f
#define PLAYERBULLETSPEED 8.0f
#define ENEMYBULLETSPEED 2.3f
#define PLAYERMSSILESPEED 7

#define PLAYERSHIPLIVES 20
#define FRAMESPERFLASH 5
//this or
//#define SHIPFLASHCOLOR D3DCOLOR_XRGB(255, 200, 200)
//this?
#define SHIPFLASHCOLOR D3DCOLOR_XRGB(255, 0, 200)

#define FRAMESPEREXPLOSION 5

#define ENEMYDIMENSION 150
#define PLAYERDIMENSION 250

#define INSECTSCALE 0.5f
#define ENEMYSHIPSCALE 0.8f
//make it 2.0f just to prove the point about constants and how shit hardcoding is
#define PLAYERSHIPSCALE 0.5f

//keep this scale for player or something like it. Maybe 20 small explosions too
//#define EXPLOSIONSCALE 20.0f
#define EXPLOSIONSPEED 4
#define EXPLOSIONDIMENSION 300

const int AMOUNTOFVECTORS = 1;

//INHERITANCE.
//make allthese inherit from object ahaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa just looks tight as fuck

struct object {
	float x, y;
	int frame;
	float rotation;
};


struct bullet : public object
{	
	int r,g,b; //keep these and draw multicoloured plasma at certain stages of the game or always
	float vx, vy;
	int frameExposure;
};

struct missile : public object
{
	float vx, vy;
	int frameExposure;
};

struct Ship : public object
{
	float vx, vy;
	int lives;
	bool ifHit;
	short framesSinceHit;
	float speed;
	float acceleration;
	bool lockedOnTo;
	int frameExposure;
	//D3DCOLOR color; Maybe doesnt have to be part of each ship for flash hit effect. But then again maybe it does.
};

struct PlayerShip : public Ship
{
	int shield;
};

struct explosion : public object
{
	int frameExposure;
	int r, g, b;
	float scale;
};

/*struct lockOnCandidate
{
	Ship ship;
	int timer;
};*/

enum weaponChoice{
	BULLET,
	MISSILE,
	LASER
};

class Game
{
public:
	//ALL FUNCTIONS-------------------------------------------------------------------:
	Game( HWND hWnd,const KeyboardServer& kServer,const MouseServer& mServer);
	~Game();
	void Go();
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
	void createExplosion(explosion& a, float x, float y, float rotation, float scale, int r, int g, int b);
	void createLockOn();
	void setUpShips();
	void findRotation(int x1, int y1, int x2, int y2, float& rotation);
private:
	//ALL VARIABLES--------------------------------------------------------------------:
	D3DGraphics gfx;
	KeyboardClient kbd;
	MouseClient mouse;
	DSound audio;

	Sound plasmaShot;

	LPDIRECT3DTEXTURE9 playerShipTexture;
	LPDIRECT3DTEXTURE9 enemyShipTexture;
	LPDIRECT3DTEXTURE9 insectKamikazeTexture;

	LPDIRECT3DTEXTURE9 plasmaBulletTexture;
	LPDIRECT3DTEXTURE9 missileTexture;

	LPDIRECT3DTEXTURE9 missileIconTexture;
	LPDIRECT3DTEXTURE9 bulletIconTexture;
	LPDIRECT3DTEXTURE9 explosionTexture;

	LPD3DXFONT fontArial24 = NULL;
	LPD3DXFONT fontGaramond36 = NULL;
	LPD3DXFONT fontTimesNewRoman40 = NULL;

	weaponChoice weaponChoice;
	float speed;	

	Ship playerShip;
	vector<Ship> enemyShipFleet;
	vector<Ship> insectKamikazeFleet;

	vector<bullet> arrayOfVectors[AMOUNTOFVECTORS];
	vector<bullet> enemyBulletVector;
	
	vector<missile> missileVector;
	vector<Ship*> lockedOnVector;

	vector<explosion> explosionVector;

	//sprinkler
	float xVel, yVel;
	bool changeDirection;
	bool clockwise;

	int wave;
	int startTime;
	bool mouseClicked = false;

	bool keysPressedLastFrame;
	bool gameOver;
	bool paused;
	bool invincible;
	int frameCount;
	bool Locking = false;
};