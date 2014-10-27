/****************************************************************************************** 
 *	Chili DirectX Framework Version 12.04.24											  *	
 *	Keyboard.cpp																		  *
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
#include "Keyboard.h"


KeyboardClient::KeyboardClient( const KeyboardServer& kServer )
	: server( kServer )
{}

bool KeyboardClient::UpIsPressed() const
{
	return server.upIsPressed;
}
bool KeyboardClient::DownIsPressed() const
{
	return server.downIsPressed;
}
bool KeyboardClient::LeftIsPressed() const
{
	return server.leftIsPressed;
}
bool KeyboardClient::RightIsPressed() const
{
	return server.rightIsPressed;
}
bool KeyboardClient::SpaceIsPressed() const
{
	return server.spaceIsPressed;
}
bool KeyboardClient::EnterIsPressed() const
{
	return server.enterIsPressed;
}
/*bool KeyboardClient::wIsPressed() const
{
	return server.wIsPressed;
}
bool KeyboardClient::sIsPressed() const
{
	return server.sIsPressed;
}
bool KeyboardClient::aIsPressed() const
{
	return server.aIsPressed;
}
bool KeyboardClient::dIsPressed() const
{
	return server.dIsPressed;
}*/

KeyboardServer::KeyboardServer()
:	upIsPressed( false ),
	downIsPressed( false ),
	leftIsPressed( false ),
	rightIsPressed( false ),
	spaceIsPressed( false ),
	enterIsPressed( false )
	/*wIsPressed(false),
	sIsPressed(false),
	aIsPressed(false),
	dIsPressed(false)*/
{}

void KeyboardServer::OnUpPressed()
{
	upIsPressed = true;
}
void KeyboardServer::OnDownPressed()
{
	downIsPressed = true;
}
void KeyboardServer::OnLeftPressed()
{
	leftIsPressed = true;
}
void KeyboardServer::OnRightPressed()
{
	rightIsPressed = true;
}

/*void KeyboardServer::OnWPressed()
{
	wIsPressed = true;
}
void KeyboardServer::OnSPressed()
{
	sIsPressed = true;
}
void KeyboardServer::OnAPressed()
{
	aIsPressed = true;
}
void KeyboardServer::OnDPressed()
{
	dIsPressed = true;
}
*/
void KeyboardServer::OnSpacePressed()
{
	spaceIsPressed = true;
}
void KeyboardServer::OnEnterPressed()
{
	enterIsPressed = true;
}

void KeyboardServer::OnUpReleased()
{
	upIsPressed = false;
}
void KeyboardServer::OnDownReleased()
{
	downIsPressed = false;
}
void KeyboardServer::OnLeftReleased()
{
	leftIsPressed = false;
}
void KeyboardServer::OnRightReleased()
{
	rightIsPressed = false;
}
void KeyboardServer::OnSpaceReleased()
{
	spaceIsPressed = false;
}
void KeyboardServer::OnEnterReleased()
{
	enterIsPressed = false;
}

/*void KeyboardServer::OnWReleased()
{
	wIsPressed = false;
}
void KeyboardServer::OnSReleased()
{
	sIsPressed = false;
}
void KeyboardServer::OnAReleased()
{
	aIsPressed = false;
}
void KeyboardServer::OnDReleased()
{
	dIsPressed = false;
}*/