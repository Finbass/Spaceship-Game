/****************************************************************************************** 
 *	Chili DirectX Framework Version 12.04.24											  *	
 *	D3DGraphics.h																		  *
 *	Copyright 2012 PlanetChili <http://www.planetchili.net>								  *
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

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include "string.h"

//extern LPD3DXSPRITE spriteobj;
//extern LPDIRECT3D9 pDirect3D;
extern LPDIRECT3DDEVICE9 pDevice;
//extern LPDIRECT3DSURFACE9 pBackBuffer;
//extern D3DLOCKED_RECT		backRect;*/

class D3DGraphics
{
public:
	D3DGraphics( HWND hWnd );
	~D3DGraphics();
	LPDIRECT3DTEXTURE9 LoadTexture(std::string filename, D3DCOLOR transcolor = D3DCOLOR_XRGB(0, 0, 0));
	void Sprite_Transform_Draw(LPDIRECT3DTEXTURE9 image, int x, int y, int width, int height,
		int frame = 0, int columns = 1, float rotation = 0.0f, float scaling = 1.0f,
		D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255));
	void PutPixel(int x, int y, int r, int g, int b);
	void PutPixel( int x, int y, D3DCOLOR c);
	void DrawLine( int x1,int y1,int x2,int y2,int r,int g,int b );
	void DrawCircle( int cx,int cy,int radius,int r,int g,int b );
	void DrawDisc( int cx,int cy,int r,int rd,int g,int b );
	void BeginFrame();
	void EndFrame();
	void rendering();
	LPDIRECT3DDEVICE9* getDevice();

private:
	//IDirect3D9*			pDirect3D;
	//IDirect3DDevice9*	pDevice;
	//IDirect3DSurface9*	pBackBuffer;
	//D3DLOCKED_RECT		backRect;
	//LPD3DXSPRITE		spriteobj;
	LPDIRECT3D9 pDirect3D;
	LPDIRECT3DSURFACE9 pBackBuffer;
	D3DLOCKED_RECT		backRect;
	LPD3DXSPRITE spriteobj;
};