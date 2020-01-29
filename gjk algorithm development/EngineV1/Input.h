#pragma once
#include <windows.h>
#include<Windowsx.h>
#include<stdint.h>
#include"Window.h"

#define KeyDown true
#define KeyUp false


struct G_MOUSE
{
	G_VECTOR2<float> Pos = { 0,0 };
	bool MiddleButton = false;
	bool LeftButton = false;
	bool RightButton = false;
	bool IsInWindow = true;
	int Wheel_Delta_Pos = 0;
};

struct G_KEYBOARD
{
	bool keyBuffer[256] = { false };

	bool isDown(unsigned char key) { return keyBuffer[key]; }

	bool Enter = false;
	bool Shift = false;
	bool Ctrl = false;
	bool Space = false;
	bool Esc = false;

	bool ArrowLeft = false;
	bool ArrowRight = false;
	bool ArrowDown = false;
	bool ArrowUp = false;

	bool IsKbHit = false;
	char lastPressed = 0;
};



struct G_INPUT
{
	G_MOUSE mouse;
	G_KEYBOARD keyboard;
};

void GInput_Initialize(G_INPUT* ginput_t);


void GetMessages(G_INPUT* input, UINT data, WPARAM wParam, LPARAM LParam);