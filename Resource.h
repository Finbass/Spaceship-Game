/****************************************************************************************** 
 *	Chili DirectX Framework Version 12.04.24											  *	
 *	Resource.h																			  *
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
#ifndef IDC_STATIC
#define IDC_STATIC (-1)
#endif

#define IDI_APPICON16                           101
#define IDI_APPICON32                           103

#define ROTATIONAMOUNT 0.0001f
#define PI 3.14159265

#define SIZEOFENEMYSHIPFLEET 15
#define SIZEOFINSECTFLEET 10

//#define PLAYERBULLETSPEED 3.0f
#define PLAYERBULLETSPEED 8.0f
#define ENEMYBULLETSPEED 2.3f
#define PLAYERMSSILESPEED 4.0f

#define BULLETDAMAGE 1
#define MISSILEDAMAGE 35

#define INSECTSPEED 1.5f

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
#define ENEMYSHIPSCALE 0.7f
//make it 2.0f just to prove the point about constants and how shit hardcoding is
#define PLAYERSHIPSCALE 0.4f

//keep this scale for player or something like it. Maybe 20 small explosions too
//#define EXPLOSIONSCALE 20.0f
#define EXPLOSIONSPEED 4
#define EXPLOSIONDIMENSION 300

#define LOCKONSCALE 0.6f

#define INITIALLOCKDELAY 30
#define LOCKINGDELAY 60
