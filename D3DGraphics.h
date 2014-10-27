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
<<<<<<< HEAD
extern LPD3DXSPRITE spriteobj;
extern LPD3DXSPRITE spriteobj2;
//extern LPDIRECT3DSURFACE9 pBackBuffer;
//extern D3DLOCKED_RECT		backRect;*/
//bool murrayPlaying = false;

#define MURRAYPLAYING 2

//get rid of the 5 difference, shouldnt matter and do it in the cases it does matter. Therefore we can use these for the mouse, collision, backbuffer and all resolutions.
#if (MURRAYPLAYING == 1)
#define LEFTBOUNDARY 5
#define RIGHTBOUNDARY 1361
#define UPPERBOUNDARY 5
#define LOWERBOUNDARY 763
#else 
#define LEFTBOUNDARY 5
#define RIGHTBOUNDARY 1915
#define UPPERBOUNDARY 5
#define LOWERBOUNDARY 1075
#endif
=======
//extern LPDIRECT3DSURFACE9 pBackBuffer;
//extern D3DLOCKED_RECT		backRect;*/
>>>>>>> 570942f798fda6f6209c985806d7b944f5151d7e

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
<<<<<<< HEAD
	LPD3DXFONT MakeFont(std::string name, int size);
	void FontPrint(LPD3DXFONT font, int x, int y, std::string text, D3DCOLOR color);
=======
>>>>>>> 570942f798fda6f6209c985806d7b944f5151d7e
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
<<<<<<< HEAD
	


=======
	LPD3DXSPRITE spriteobj;
>>>>>>> 570942f798fda6f6209c985806d7b944f5151d7e
};