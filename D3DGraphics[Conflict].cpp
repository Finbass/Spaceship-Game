/****************************************************************************************** 
 *	Chili DirectX Framework Version 12.04.24											  *	
 *	D3DGraphics.cpp																		  *
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
#include "D3DGraphics.h"
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "Bitmap.h"
#include <gdiPlus.h>
#pragma comment(lib,"gdiplus.lib")

using std::string;

LPDIRECT3D9 pDirect3D = NULL;
LPDIRECT3DDEVICE9 pDevice = NULL;
LPDIRECT3DSURFACE9 pBackBuffer = NULL;
D3DLOCKED_RECT		backRect;
LPD3DXSPRITE spriteobj;

void LoadSprite( Sprite* sprite, const char* filename,
				unsigned int width, unsigned int height, D3DCOLOR key) {
	sprite->surface = (D3DCOLOR*)malloc( sizeof( D3DCOLOR ) * width * height);
	LoadBmp(filename, sprite->surface);
	sprite->height = height;
	sprite->width = width;
	sprite->key = key;
}

void FreeSprite( Sprite* sprite ) {
	free(sprite->surface);
}

void D3DGraphics::drawSprite( int xoff, int yoff, Sprite* sprite) {
	for (int y = 0; y < ((*sprite).height); y++) {
		for (int x = 0; x < sprite->width; x++) {
			D3DCOLOR c = sprite->surface[ x + y * sprite->width ];
			if ( c != sprite->key ) {
				PutPixel(x + xoff, y + yoff, c );
			}
		}
	}
}

D3DGraphics::D3DGraphics( HWND hWnd )
{
	HRESULT result;

	backRect.pBits = NULL;
	
	pDirect3D = Direct3DCreate9( D3D_SDK_VERSION );
	assert( pDirect3D != NULL );

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp,sizeof( d3dpp ) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    //d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	//d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	//// HAAAAAHAAAAA CHECK THIS OUT AGAIN MAKES FPS 1000
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.EnableAutoDepthStencil = 1;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth = 1920;
	d3dpp.BackBufferHeight = 1080;

   // result = pDirect3D->CreateDevice( D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,
		//D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,&d3dpp,&pDevice );
	result = pDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDevice);
	assert( !FAILED( result ) );

	result = pDevice->GetBackBuffer( 0,0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer );
	assert( !FAILED( result ) );

	D3DXCreateSprite(pDevice, &spriteobj);
}

void D3DGraphics::Sprite_Transform_Draw(LPDIRECT3DTEXTURE9 image, int x, int y, int width, int height,
	int frame, int columns, float rotation, float scaling, D3DCOLOR color)
{
	//create a scale vector
	D3DXVECTOR2 scale(scaling, scaling);

	//create a translate vector
	D3DXVECTOR2 trans(x, y);

	//set center by dividing width and height by two
	D3DXVECTOR2 center((float)(width * scaling) / 2, (float)(height * scaling) / 2);
	D3DXVECTOR2 scalingCenter((float)(width) / 2, (float)(height) / 2);

	//create 2D transformation matrix
	D3DXMATRIX mat;
	D3DXMatrixTransformation2D(&mat, &scalingCenter, 0, &scale, &center, rotation, &trans);

	//tell sprite object to use the transform
	spriteobj->SetTransform(&mat);

	//calculate frame location in source image
	int fx = (frame % columns) * width;
	int fy = (frame / columns) * height;
	RECT srcRect = { fx, fy, fx + width, fy + height };

	//draw the sprite frame
	spriteobj->Draw(image, &srcRect, NULL, NULL, color);
	
}

LPDIRECT3DTEXTURE9 D3DGraphics::LoadTexture(std::string filename, D3DCOLOR transcolor)
{
	LPDIRECT3DTEXTURE9 texture = NULL;

	//get width and height from bitmap file
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
	if (result != D3D_OK) return NULL;

	//create the new texture by loading a bitmap image file
	D3DXCreateTextureFromFileEx(
		pDevice,               //Direct3D device object
		filename.c_str(),      //bitmap filename
		info.Width,            //bitmap image width
		info.Height,           //bitmap image height
		1,                     //mip-map levels (1 for no chain)
		D3DPOOL_DEFAULT,       //the type of surface (standard)
		D3DFMT_UNKNOWN,        //surface format (default)
		D3DPOOL_DEFAULT,       //memory class for the texture
		D3DX_DEFAULT,          //image filter
		D3DX_DEFAULT,          //mip filter
		transcolor,            //color key for transparency
		&info,                 //bitmap file info (from loaded file)
		NULL,                  //color palette
		&texture);             //destination texture

	//make sure the bitmap textre was loaded correctly
	if (result != D3D_OK) return NULL;

	return texture;
}

D3DGraphics::~D3DGraphics()
{
	if( pDevice )
	{
		pDevice->Release();
		pDevice = NULL;
	}
	if( pDirect3D )
	{
		pDirect3D->Release();
		pDirect3D = NULL;
	}
	if( pBackBuffer )
	{
		pBackBuffer->Release();
		pBackBuffer = NULL;
	}
	//GET SOME MORE RELEASE
}

void D3DGraphics::PutPixel( int x,int y,int r,int g,int b )
{	
	assert( x >= 0 );
	assert( y >= 0 );
	//assert( x < 800 );
	//assert( y < 600 );
	((D3DCOLOR*)backRect.pBits)[ x + (backRect.Pitch >> 2) * y ] = D3DCOLOR_XRGB( r,g,b );
}

void D3DGraphics::PutPixel(int x, int y, D3DCOLOR c) {
	assert(x >= 0);
	assert(y >= 0);
	//assert( x < 800 );
	//assert( y < 600 );
	((D3DCOLOR*)backRect.pBits)[x + (backRect.Pitch >> 2) * y] = c;
}

void D3DGraphics::BeginFrame()
{
	HRESULT result;

	//result = pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 100), 1.0f, 0);
	//assert( !FAILED( result ) );

	result = pBackBuffer->LockRect( &backRect,NULL,NULL );
	assert( !FAILED( result ) );

	spriteobj->Begin(D3DXSPRITE_ALPHABLEND);
}

void D3DGraphics::EndFrame()
{
	HRESULT result;

	result = pBackBuffer->UnlockRect();
	assert( !FAILED( result ) );

	spriteobj->End();

	pDevice->EndScene();

	result = pDevice->Present( NULL,NULL,NULL,NULL );
	assert( !FAILED( result ) );
}

void D3DGraphics::DrawDisc( int cx,int cy,int r,int rd,int g,int b )
{
	for( int x = cx - r; x < cx + r; x++ )
	{
		for( int y = cy - r; y < cy + r; y++ )
		{
			if( sqrt( (float)( (x - cx)*(x - cx) + (y - cy)*(y - cy) ) ) < r )
			{
				PutPixel( x,y,rd,g,b );
			}
		}
	}
}

void D3DGraphics::DrawLine( int x1,int y1,int x2,int y2,int r,int g,int blu )
{
	int dx = x2 - x1;
	int dy = y2 - y1;

	if( dy == 0 && dx == 0 )
	{
		PutPixel( x1,y1,r,g,blu );
	}
	else if( abs( dy ) > abs( dx ) )
	{
		if( dy < 0 )
		{
			int temp = x1;
			x1 = x2;
			x2 = temp;
			temp = y1;
			y1 = y2;
			y2 = temp;
		}
		float m = (float)dx / (float)dy;
		float b = x1 - m*y1;
		for( int y = y1; y <= y2; y = y + 1 )
		{
			int x = (int)(m*y + b + 0.5f);
			PutPixel( x,y,r,g,blu );
		}
	}
	else
	{
		if( dx < 0 )
		{
			int temp = x1;
			x1 = x2;
			x2 = temp;
			temp = y1;
			y1 = y2;
			y2 = temp;
		}
		float m = (float)dy / (float)dx;
		float b = y1 - m*x1;
		for( int x = x1; x <= x2; x = x + 1 )
		{
			int y = (int)(m*x + b + 0.5f);
			PutPixel( x,y,r,g,blu );
		}
	}
}

void D3DGraphics::DrawCircle( int centerX,int centerY,int radius,int r,int g,int b )
{
	int rSquared = radius*radius;
	int xPivot = (int)(radius * 0.707107f + 0.5f);
	for( int x = 0; x <= xPivot; x++ )
	{
		int y = (int)(sqrt( (float)( rSquared - x*x ) ) + 0.5f);
		PutPixel( centerX + x,centerY + y,r,g,b );
		PutPixel( centerX - x,centerY + y,r,g,b );
		PutPixel( centerX + x,centerY - y,r,g,b );
		PutPixel( centerX - x,centerY - y,r,g,b );
		PutPixel( centerX + y,centerY + x,r,g,b );
		PutPixel( centerX - y,centerY + x,r,g,b );
		PutPixel( centerX + y,centerY - x,r,g,b );
		PutPixel( centerX - y,centerY - x,r,g,b );
	}
}