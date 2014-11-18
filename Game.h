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
#include <deque> //start using these bad boys
#include "Resource.h"

using std::vector;
//using std::deque;

//everything moved to resource.h

const int AMOUNTOFVECTORS = 1;
//const int lockOndelay = 30;
//not needed coz its variable for each lockOnCandidate

struct object {
	float x, y;
	int frame;
	float rotation;
	//the big questions: add vx, vy here?
	//add width and height to each single object but thats wasteful? Static fixes this? Class width, height not object.
	//and our answer  http://stackoverflow.com/questions/1496629/do-static-members-help-memory-efficiency
	static int width, height; //in each derived class instead or maybe not and just initiliaze them like below
	//ahh dont think it works coz "static data member cannot be initialized via derived class"
	//this is only for the convenience of the collision detection function.
};



struct bullet : public object
{	
	int r,g,b; //keep these and draw multicoloured plasma at certain stages of the game or always
	float vx, vy;
	int frameExposure;
	//static int width, height; //still doesnt work
};

//doesnt work
//bullet::width = 5;

//struct Ship {};
//doesn't know what ship is in missile

struct explosion : public object
{
	int frameExposure;
	int r, g, b;
	float scale;
};

struct Ship : public object
{
	float vx, vy;
	int lives;
	bool ifHit;
	short framesSinceHit;
	float speed;
	float acceleration;
	int frameExposure;
	//D3DCOLOR color; Maybe doesnt have to be part of each ship for flash hit effect. But then again maybe it does.
};

struct missile : public object
{
	float vx, vy;
	Ship* target;
};

struct Insect : public Ship {};

struct EnemyShip1 : public Ship {};

struct PlayerShip : public Ship
{
	int shield;
	int missileAmmunition;
};

struct Squad
{
	vector<Ship*> shipArray;
};

struct lockOnCandidate
{
	Ship* ship;
	int startTime;
	int frame;
	int lockOnDelay;
	bool lockedOn;
	bool lockingOn;
	bool inMouseRange;
};

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
	void updateEnemyShip1s();
	void updateInsects();
	void bulletLogic();
	void createBullet(bool ifPlayer);
	void createMissile(bool ifPlayer, Ship* targetLockOn);
	void fireMissile(bool ifPlayer);
	void createExplosion(explosion& a, float x, float y, float rotation, float scale, int r, int g, int b);
	void createLockOn();
	void allLocking();
	void findLockOn();
	void setUpShips();
	void makeEnemy(int whichShip, int squadCreatedCount);
	void findRotation(int x1, int y1, int x2, int y2, float& rotation);
	void writeText(LPD3DXFONT font, int x, int y, D3DCOLOR c, char * str);
	void collisionDetection(int width1, int height1, int width2, int height2, int x1, int y1, int x2, int y2,
		int vx1, int vy1, int vx2, int vy2, int typeOfCollision);
	//maybe just (object1, object2 and thats it. hahaaa. from 8+ parameters to 2.)
	//maybe this shows advantage of inheritance the most. Instead of many different functions or different sections in the one function, it makes it concise. Maybe... feck width and height aren't involved in object.
	void collisionResponse();
private:
	//ALL VARIABLES--------------------------------------------------------------------:
	D3DGraphics gfx;
	KeyboardClient kbd;
	MouseClient mouse;
	DSound audio;

	Sound plasmaShot;
	Sound tune;

	LPDIRECT3DTEXTURE9 playerShipTexture;
	LPDIRECT3DTEXTURE9 enemyShipTexture;
	LPDIRECT3DTEXTURE9 insectKamikazeTexture;

	LPDIRECT3DTEXTURE9 plasmaBulletTexture;
	LPDIRECT3DTEXTURE9 missileTexture;

	LPDIRECT3DTEXTURE9 missileIconTexture;
	LPDIRECT3DTEXTURE9 bulletIconTexture;
	LPDIRECT3DTEXTURE9 explosionTexture;
	LPDIRECT3DTEXTURE9 missileLockTexture;
	LPDIRECT3DTEXTURE9 weaponHudTexture;

	LPD3DXFONT fontArial24 = NULL;
	LPD3DXFONT fontGaramond36 = NULL;
	LPD3DXFONT fontTimesNewRoman40 = NULL;

	//player Variables.
	PlayerShip playerShip;
	float speed;
	bool invincible;
	weaponChoice weaponChoice;

	vector<Squad> squadVector;

	//std::deque<EnemyShip1> enemyShipFleet;
	//std::deque<Insect> insectKamikazeFleet;
	//95% sure solves all problems.
	//alternatively
	//vector<EnemyShip1*> enemyShipFleet;
	//vector<Insect*> insectKamikazeFleet;
	//but effort and would be nice to avoid new/delete even if you can be safe about them

	vector<EnemyShip1> enemyShipFleet;
	vector<Insect> insectKamikazeFleet;

	vector<bullet> arrayOfVectors[AMOUNTOFVECTORS];
	vector<bullet> enemyBulletVector;
	
	vector<missile> missileVector;
	vector<missile> missileAmmoVector;

	vector<explosion> explosionVector;
	
	vector<lockOnCandidate> pShipTargets;
	lockOnCandidate* specificTarget;
	
	int noOfLockedOn;
	bool currentlyLocking;
	//vector<Ship*> lockedOnVector;
	//vector<Ship*> lockingOnVector;
	//vector<Ship*> notLockedOnVector;
	float lockRotation = 0.0f;

	time_t start;
	double secondsSinceStart;
	int waveTimer;
	int nextSquadTimer;
	int timeTillNextSquad;
	int minTimeBetweenSquad;
	int initialTimeBetweenWaves;
	int minTimeBetweenWaves;

	int wave;
	int wavesPerLevel;
	int squadsPerWave;
	int squadsCreatedLevel;
	int squadsCreatedWave;
	int squadsDefeatedLevel;
	int squadsDefeatedWave;

	int totalEnemyShipCreatedLevel;
	int totalEnemyShipKillsLevel;
	
	int totalInsectCreatedLevel;
	int totalInsectKillsLevel;

	int totalShipCreatedLevel;
	int totalShipCreatedWave;
	int totalShipKillsLevel;
	int totalShipKillsWave;

	bool doneIt;
	bool mouseClicked = false;
	bool keysPressedLastFrame;
	bool gameOver;
	bool paused;
	int frameCount;
};