#pragma once

#define PI 3.14159265359

#define debugPrint(...) char *str = new char[100];  sprintf_s(str,100,__VA_ARGS__ );	OutputDebugString(str);

#define MakeVector(x,y) {x,y}
#define MakeVector_toInt(x,y) {(int)x,(int)y}
#define MakeVector_toFloat(x,y) {(float)x,(float)y}

#define eprint(frameBuffer, font,pos, formatString,...)		\
{															\
	char newString[50];										\
	sprintf_s(newString,formatString, __VA_ARGS__);			\
	printText(frameBuffer, font, pos,newString);			\
}															\

#include<Windows.h>

//DrawText(gameWindow.DeviceContext,"hello",NULL,NULL,NULL);
//PlaySoundA((LPCSTR)"turret_firing.wav", NULL, SND_FILENAME | SND_ASYNC);

template<typename Gtype>
struct G_VECTOR2
{
	Gtype x;
	Gtype y;

	G_VECTOR2 operator+(G_VECTOR2 rhs)
	{
		G_VECTOR2 result;
		result.x = rhs.x + this->x;
		result.y = rhs.y + this->y;
		return result;
	}
	G_VECTOR2 operator-(G_VECTOR2 rhs)
	{
		G_VECTOR2 result;
		result.x = this->x - rhs.x;
		result.y = this->y - rhs.y;
		return result;
	}

	G_VECTOR2 operator+(Gtype rhs)
	{
		G_VECTOR2 result;
		result.x = rhs + this->x;
		result.y = rhs + this->y;
		return result;
	}
	G_VECTOR2 operator-(Gtype rhs)
	{
		G_VECTOR2 result;
		result.x = this->x - rhs;
		result.y = this->y - rhs;
		return result;
	}
	G_VECTOR2 operator*(Gtype rhs)
	{
		G_VECTOR2 result;
		result.x = rhs * this->x;
		result.y = rhs * this->y;
		return result;
	}
	G_VECTOR2 operator/(Gtype rhs)
	{
		G_VECTOR2 result;
		result.x = this->x/rhs;
		result.y = this->y/rhs;
		return result;
	}

	float dot(G_VECTOR2 A)
	{
		return A.x*x + A.y*y;
	}

	float lenght()
	{
		return sqrtf(x*x + y*y);
	}

	G_VECTOR2 perp_right()
	{
		return MakeVector(-y,x);
	}
	
	G_VECTOR2 perp_left()
	{
		return MakeVector(y, -x);
	}

	G_VECTOR2 negative()
	{
		return MakeVector(-x, -y);
	}

	G_VECTOR2 negative_y()
	{
		return MakeVector(x, -y);
	}

	G_VECTOR2 negative_x()
	{
		return MakeVector(-x, y);
	}

	bool onTheSameSide(G_VECTOR2 v)
	{
		if (this->dot(v) > 0) 
			return true;
	}

	void toUnitVect()
	{
		float len = this->lenght();
		x = x / len;
		y = y / len;
	}
};

struct G_COLOR32
{
	unsigned int a : 8;
	unsigned int b : 8;
	unsigned int g : 8;
	unsigned int r : 8;

	G_COLOR32 operator*(float rhs)
	{
		G_COLOR32 result;
		result.r = rhs * this->r;
		result.g = rhs * this->g;
		result.b = rhs * this->b;
		result.a = rhs * this->a;
		return result;
	}
};


struct G_SPRITE
{
	G_COLOR32 * memory = NULL;
	int width;
	int height;
};

struct G_PIXELSURFACE
{
	int width;
	int height;
	int size;
	BITMAPINFO BitmapInfo;
	int *memory;
};
