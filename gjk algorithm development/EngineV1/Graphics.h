#pragma once
#include <windows.h>
#include<stdint.h>
#include"Window.h"
#include<stdio.h>
#include<string>
#include<vector>
#include"Input.h"
#include "commonStuff.h"

//error flags
#define FUNCTION_INVALID_MEMORY 100
#define FUNCTION_SUCCESS 101
#define FUNCTION_ALLOCATION_ERROR 103
#define FUNCTION_ERROR 104


#define MakeColor(r,g,b,a) {a,b,g,r}

#define red		MakeColor(255, 0, 0, 255)
#define blue	MakeColor(0, 0, 255, 255)
#define green	MakeColor(0, 255, 0, 255)
#define black	MakeColor(0, 0, 0, 255)
#define white	MakeColor(255, 255, 255, 255)
#define gray	MakeColor(125, 125, 125, 255)


int clearFrameBuffer(G_PIXELSURFACE backbuffer, G_COLOR32 color = { 0,0,0,0 });

int displayBuffer(G_PIXELSURFACE frameBuffer_t, G_WINDOW window_t, bool resizeAndStretch = false);

int allocateBufferMemory(G_PIXELSURFACE *buffer, int width, int height);

int deallocateBufferMemory(G_PIXELSURFACE* frameBuffer_t);


int putPixel(G_PIXELSURFACE frameBuffer, G_VECTOR2<float> position, G_COLOR32 color, bool clipping = false);

G_COLOR32  getPixel(G_PIXELSURFACE frameBuffer, G_VECTOR2<int> position, bool clipping = false);


G_SPRITE loadSprite(const char* filename);

int drawSprite(G_PIXELSURFACE backBuffer, G_SPRITE* sprite, G_VECTOR2<float> pos, G_VECTOR2<float> size, float teta);


int DrawRect(G_PIXELSURFACE frameBuffer, G_VECTOR2<float> XYmin, G_VECTOR2<float> XYmax, G_COLOR32 color);

int DrawCircle(G_PIXELSURFACE backBuffer, G_VECTOR2<float> pos, float rad, G_COLOR32 color, bool fill);

int DrawLine(G_PIXELSURFACE backBuffer, G_VECTOR2<float> pos1, G_VECTOR2<float> pos2, G_COLOR32 color);

void displayPolygon(G_PIXELSURFACE backBuffer, std::vector<G_VECTOR2<float>> nodes, G_VECTOR2<float> pos, G_COLOR32 vertexColor, G_COLOR32 edgeColor, float vertexRadius, float normalLength);


#pragma pack(push, 1)//prevents padding

//FONT
struct INFO
{
	short fontSize;

	char smooth : 1;//0
	char unicode : 1;//1
	char italic : 1;//2
	char bold : 1;//3
	char fixedHeight : 1;//4
	char reserved : 3;//5-7

	u_char charSet;
	u_char stretchH;
	u_char aa;
	u_char paddingUp;
	u_char paddingRight;
	u_char paddingDown;
	u_char paddingLeft;
	u_char spacingHoriz;
	u_char spacingVert;
	u_char outline;

	char* fontName = NULL;//can be ignored
};

struct COMMON
{
	u_short lineHeight;
	u_short base;
	u_short scaleW;
	u_short scaleH;
	u_short pages;

	char reserved : 7;
	char packet : 1;

	u_char alphaChnl;
	u_char redChnl;
	u_char greenChnl;
	u_char blueChnl;
};

struct PAGES
{
	char* pageNames = NULL;
	G_SPRITE fontBitmap;
};

struct CHARS
{
	u_int id;
	u_short x;
	u_short y;
	u_short width;
	u_short height;
	u_short xoffset;
	u_short yoffset;
	u_short xadvance;
	u_char page;
	u_char chnl;
};

struct KERNING
{
	u_int first;
	u_int second;
	short amount;
};

struct FONT
{
	INFO info;
	COMMON common;

	u_short nPages;
	PAGES *pages;
	
	u_short nChars;
	CHARS *chars;

	u_short nKerning;
	KERNING *kerning;
};

#pragma pack(pop)

bool loadFont(FONT* font,const char* filename);


void drawFontChar(G_PIXELSURFACE frameBuffer, FONT* font, u_int index, G_VECTOR2<float> pos);

void printText(G_PIXELSURFACE frameBuffer, FONT* font, G_VECTOR2<float> pos_t, std::string text);
