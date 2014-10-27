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
#include <gdiPlus.h>
#pragma comment(lib,"gdiplus.lib")

using std::string;

LPDIRECT3DDEVICE9 pDevice;
LPD3DXSPRITE spriteobj;
LPD3DXSPRITE spriteobj2;

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
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	//d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.EnableAutoDepthStencil = 1;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;	
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;

	d3dpp.BackBufferWidth = RIGHTBOUNDARY + 5;
	d3dpp.BackBufferHeight = LOWERBOUNDARY + 5;

	//d3dpp. do this to see all parts of struct. same with any other class etc.

   // result = pDirect3D->CreateDevice( D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,
		//D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,&d3dpp,&pDevice );
	result = pDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDevice);
	assert( !FAILED( result ) );

	result = pDevice->GetBackBuffer( 0,0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer );
	assert( !FAILED( result ) );

	D3DXCreateSprite(pDevice, &spriteobj);
	D3DXCreateSprite(pDevice, &spriteobj2);
}

void D3DGraphics::Sprite_Transform_Draw(LPDIRECT3DTEXTURE9 image, int x, int y, int width, int height,
	int frame, int columns, float rotation, float scaling, D3DCOLOR color)
{
	//create a scale vector
	D3DXVECTOR2 scale(scaling, scaling);

	//create a translate vector
	D3DXVECTOR2 trans(x, y);

	//set center by dividing width and height by two
	D3DXVECTOR2 center((float)((width * scaling) / 2), (float)((height * scaling) / 2));
	D3DXVECTOR2 scalingCenter((float)((width * scaling) / 2), (float)((height * scaling) / 2));

	//create 2D transformation matrix
	D3DXMATRIX mat;
	D3DXMatrixTransformation2D(&mat, NULL, 0, &scale, &center, rotation, &trans);

	//tell sprite object to use the transform
	spriteobj->SetTransform(&mat);

	//calculate frame location in source image
	int fx = (frame % columns) * width;
	int fy = (frame / columns) * height;
	RECT srcRect = { fx, fy, fx + width, fy + height};

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
	//if problem with c_str its multibyte in general menu

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
	if (spriteobj) {
		spriteobj->Release();
		spriteobj = NULL;
	}
	if (spriteobj2) {
		spriteobj2->Release();
		spriteobj2 = NULL;
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

void D3DGraphics::rendering() {

}

LPDIRECT3DDEVICE9* D3DGraphics::getDevice() {
	return &pDevice;
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

LPD3DXFONT D3DGraphics::MakeFont(string name, int size)
{
	LPD3DXFONT font = NULL;

	D3DXFONT_DESC desc = {
		size,                   //height
		0,                      //width
		0,                      //weight
		0,                      //miplevels
		false,                  //italic
		DEFAULT_CHARSET,        //charset
		OUT_TT_PRECIS,          //output precision
		CLIP_DEFAULT_PRECIS,    //quality
		DEFAULT_PITCH,          //pitch and family
		""                      //font name
	};

	strcpy(desc.FaceName, name.c_str());

	D3DXCreateFontIndirect(pDevice, &desc, &font);

	return font;
}

void D3DGraphics::FontPrint(LPD3DXFONT font, int x, int y, string text, D3DCOLOR color)
{
	//figure out the text boundary
	RECT rect = { x, y, 0, 0 };
	font->DrawText(NULL, text.c_str(), text.length(), &rect, DT_CALCRECT, color);

	//print the text
	font->DrawText(spriteobj2, text.c_str(), text.length(), &rect, DT_LEFT, color);
}

/*bool D3DGraphics::CollisionD(float x1, float y1, float x2, float y2, int dimension1, int dimension2)
{
	double radius1, radius2;

	//calculate radius 1
	if (sprite1.width > sprite1.height)
		radius1 = (sprite1.width * sprite1.scaling) / 2.0;
	else
		radius1 = (sprite1.height * sprite1.scaling) / 2.0;

	//center point 1
	double x1 = sprite1.x + radius1;
	double y1 = sprite1.y + radius1;
	D3DXVECTOR2 vector1(x1, y1);

	//calculate radius 2
	if (sprite2.width > sprite2.height)
		radius2 = (sprite2.width * sprite2.scaling) / 2.0;
	else
		radius2 = (sprite2.height * sprite2.scaling) / 2.0;

	//center point 2
	double x2 = sprite2.x + radius2;
	double y2 = sprite2.y + radius2;
	D3DXVECTOR2 vector2(x2, y2);

	//calculate distance
	double deltax = vector1.x - vector2.x;
	double deltay = vector2.y - vector1.y;
	double dist = sqrt((deltax * deltax) + (deltay * deltay));

	//radius1 /= 2;
	//radius2 /= 2;

	//return distance comparison
	return (dist < radius1 + radius2);
}*/