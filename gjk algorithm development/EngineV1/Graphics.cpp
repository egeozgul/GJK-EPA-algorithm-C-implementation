#include"Graphics.h"
#include <math.h>
#include <algorithm>


int clearFrameBuffer(G_PIXELSURFACE frameBuffer, G_COLOR32 color)
{
	if (frameBuffer.memory == NULL)
		return FUNCTION_INVALID_MEMORY;

	unsigned int fillColor = (color.b | (color.g << 8) | (color.r << 16) | (color.a << 24));

	memset(frameBuffer.memory,fillColor, frameBuffer.size);

	return FUNCTION_SUCCESS;
}

int displayBuffer(G_PIXELSURFACE frameBuffer, G_WINDOW window_t, bool resizeAndStretch)
{
	if (frameBuffer.memory == NULL)
		return FUNCTION_INVALID_MEMORY;

	int nResult;

	if (resizeAndStretch == true)
	{
		nResult = StretchDIBits(window_t.DeviceContext,
			0, 0, window_t.clientAreaWidth, window_t.clientAreaHeight,
			0, 0, frameBuffer.width, frameBuffer.height,
			frameBuffer.memory, &frameBuffer.BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
		
		if (nResult == 0)
			return FUNCTION_ERROR;
	}

	nResult = StretchDIBits(window_t.DeviceContext,//GetDC(NULL),
		0, 0, frameBuffer.width, frameBuffer.height,
		0, 0, frameBuffer.width, frameBuffer.height,
		frameBuffer.memory, &frameBuffer.BitmapInfo, DIB_RGB_COLORS, SRCCOPY);

	if (nResult == 0)
		return FUNCTION_ERROR;

	return FUNCTION_SUCCESS;
}

int allocateBufferMemory(G_PIXELSURFACE *frameBuffer ,int width, int height)
{
	int bytePerPixel = 4;

	frameBuffer->width = width;
	frameBuffer->height = height;
	frameBuffer->size = width * height * bytePerPixel;
	frameBuffer->memory = (int*)VirtualAlloc(0, frameBuffer->size, MEM_COMMIT, PAGE_READWRITE);

	if (frameBuffer->memory == NULL)
		return FUNCTION_ALLOCATION_ERROR;

	frameBuffer->BitmapInfo = { 0 };

	frameBuffer->BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);// BitmapInfo.bmiHeader);
	frameBuffer->BitmapInfo.bmiHeader.biWidth = width;
	frameBuffer->BitmapInfo.bmiHeader.biHeight = height;
	frameBuffer->BitmapInfo.bmiHeader.biPlanes = 1;
	frameBuffer->BitmapInfo.bmiHeader.biBitCount = bytePerPixel * 8;
	frameBuffer->BitmapInfo.bmiHeader.biCompression = BI_RGB;

	return FUNCTION_SUCCESS;;
}

int deallocateBufferMemory(G_PIXELSURFACE* frameBuffer)
{
	if (frameBuffer == NULL)
		return FUNCTION_INVALID_MEMORY;

	VirtualFree((void*)frameBuffer->memory, 0, MEM_RELEASE);
	
	return FUNCTION_SUCCESS;
}


static struct BitmapFileHeader
{
	u_int fileSize;
	u_short reserved1, reserved2;
	u_int offsetToPixelData;
};

static struct BitmapInfoHeader
{
	unsigned int headerSize;
	int width, height;
	unsigned short planes;
	unsigned short bits;
	unsigned int compression;
	unsigned int imageSize;
	int xResolution, yResolution;
	unsigned int nColors;
	unsigned int importantColours;
};

G_SPRITE loadSprite(const char* filename)
{
	G_SPRITE sprite_t;

	FILE* bmpFile;
	fopen_s(&bmpFile, filename, "rb");

	char signature[2];
	fread(signature, sizeof(char), 2, bmpFile);
	
	BitmapFileHeader fileHeader;
	fread(&fileHeader, sizeof(fileHeader), 1, bmpFile);

	BitmapInfoHeader infoHeader;
	fread(&infoHeader, sizeof(infoHeader), 1, bmpFile);

	fseek(bmpFile, fileHeader.offsetToPixelData, SEEK_SET);

	sprite_t.height = infoHeader.height;
	sprite_t.width = infoHeader.width;

	sprite_t.memory = (G_COLOR32*)malloc(sprite_t.height* sprite_t.width * sizeof(G_COLOR32));

	fread(sprite_t.memory, sizeof(G_COLOR32), sprite_t.height* sprite_t.width, bmpFile);

	fclose(bmpFile);

	return sprite_t;
}


int drawSprite(G_PIXELSURFACE frameBuffer, G_SPRITE* sprite, G_VECTOR2<float> pos, G_VECTOR2<float> size, float teta)
{
	if (frameBuffer.memory == NULL || sprite->memory == NULL)
		return FUNCTION_INVALID_MEMORY;

	if (teta == 0)
	{
		for (int iy = 0; iy < sprite->height; iy++)
		{
			for (int ix = 0; ix < sprite->width; ix++)
			{
				G_COLOR32 pixel = sprite->memory[ix + (sprite->height-1-iy) * sprite->height];

				G_VECTOR2<float> pos2 = MakeVector(pos.x + ix- sprite->width/2, pos.y + iy - sprite->height/2 +2);

				putPixel(frameBuffer, pos2, pixel, true);
			}
		}

		return FUNCTION_SUCCESS;
	}
	else if (teta == 180)
	{
		for (int iy = 0; iy < sprite->height; iy++)
		{
			for (int ix = 0; ix < sprite->width; ix++)
			{
				G_COLOR32 pixel = sprite->memory[ix + (iy) * sprite->height];

				G_VECTOR2<float> pos2 = MakeVector(pos.x + ix - sprite->width / 2, pos.y + iy - sprite->height / 2 + 2);

				putPixel(frameBuffer, pos2, pixel, true);
			}
		}

		return FUNCTION_SUCCESS;
	}


	float diagonal = size.lenght();
	float tangentAngle = atanf(size.y /size.x);

	G_VECTOR2<float> A = { 0,0 };
	G_VECTOR2<float> B = { size.y*cosf(PI / 2 + teta),-size.y * sinf(PI / 2 + teta) };
	G_VECTOR2<float> C = { diagonal*cosf(tangentAngle + teta), -diagonal * sinf(teta + tangentAngle) };
	G_VECTOR2<float> D = { size.x*cosf(teta), -size.x * sinf(teta) };

	G_VECTOR2<float> minVertx;
	minVertx.x = min(A.x, min(B.x, min(C.x, D.x)));
	minVertx.y = min(A.y, min(B.y, min(C.y, D.y)));

	G_VECTOR2<float> maxVertx;
	maxVertx.x = max(A.x, max(B.x, max(C.x, D.x)));
	maxVertx.y = max(A.y, max(B.y, max(C.y, D.y)));

	for (float ty = minVertx.y; ty < maxVertx.y; ty++)
	{
		G_VECTOR2<float> M;
		float txMin;
		float txMax;

		if (ty + (pos.y - C.y / 2) < 0)
		{
			continue;
		}
		else if (ty + (pos.y - C.y / 2) >= frameBuffer.height)
		{
			break;
		}
		else
		{
			M.y = ty;
		}

		for (txMin = minVertx.x; txMin< maxVertx.x; txMin++)
		{
			M.x = txMin;

			if ((M.y - D.y)*(D.x) < (D.y)*(M.x - D.x) &&
				(M.y - B.y)*(D.x) > (D.y)*(M.x - B.x) &&
				(M.y - B.y)*(B.x) >(B.y)*(M.x - B.x) &&
				(M.y - D.y)*(B.x) < (B.y)*(M.x - D.x))
			{
				if (txMin + (pos.x - C.x / 2) >= 0)
					break;
			}
		}

		for (txMax = maxVertx.x; txMax> minVertx.x; txMax--)
		{
			M.x = txMax;

			if ((M.y - D.y)*(D.x) < (D.y)*(M.x - D.x) &&
				(M.y - B.y)*(D.x) > (D.y)*(M.x - B.x) &&
				(M.y - B.y)*(B.x) > (B.y)*(M.x - B.x) &&
				(M.y - D.y)*(B.x) < (B.y)*(M.x - D.x))
			{
				if (txMax + (pos.x - C.x / 2)  < frameBuffer.width)
					break;
			}
		}

		for (float i = txMin; i <= txMax; i++)
		{
			int yy = 0.5f + (int)(abs(-D.y*i + D.x*M.y)*((float)sprite->height / size.y) / size.x);
			int xx = 0.5f + (int)(abs(-B.y*i + B.x*M.y)*((float)sprite->width/ size.x) / size.y);

			G_COLOR32 pix = sprite->memory[xx + yy * sprite->width];

			G_VECTOR2<float> pixLoc;
			pixLoc.x = 0.5f+ i + (pos.x - C.x / 2);
			pixLoc.y = 0.5f + M.y + (pos.y - C.y / 2);

			if (pix.g > 50)
			{
				int tt = 0;
			}
			putPixel(frameBuffer, pixLoc, pix, false);
		}
	}

	return FUNCTION_SUCCESS;
}


int putPixel(G_PIXELSURFACE frameBuffer, G_VECTOR2<float> position, G_COLOR32 color, bool clipping)
{
	if (frameBuffer.memory == NULL)
		return FUNCTION_INVALID_MEMORY;

	if (color.a == 0) 
		return FUNCTION_SUCCESS;

	if (isnan<float>(position.y) || isnan<float>(position.x))
		return -1;

	position.y = frameBuffer.height - position.y;

	if (clipping)
	{
		if (position.x < 0 || position.y < 0 || position.y >= frameBuffer.height || position.x >= frameBuffer.width)
			return FUNCTION_SUCCESS;
	}

	int baseC = frameBuffer.memory[(int)position.x + (int)position.y * frameBuffer.width];

	unsigned char baseBlue = baseC >> 0;
	unsigned char baseGreen = baseC >> 8;
	unsigned char baseRed = baseC >> 16;
	//unsigned char baseAlpha = baseC >> 24;

	color.b = (color.a)*(color.b - baseBlue) / 255 + baseBlue; // ((color.b)*(color.a) + (baseBlue)*(255 - color.a)) / (255);
	color.g = (color.a)*(color.g - baseGreen) / 255 + baseGreen;//((color.g)*(color.a) + (baseGreen)* (255 - color.a)) / (255);
	color.r = (color.a)*(color.r - baseRed) / 255 + baseRed;//((color.r)*(color.a) + (baseRed) * (255 - color.a)) / (255);

	frameBuffer.memory[(int)position.x + (int)position.y * frameBuffer.width] = (color.b | (color.g << 8) | (color.r << 16) | (color.a << 24)); //b, g, r, a
	
	return FUNCTION_SUCCESS;
}

G_COLOR32  getPixel(G_PIXELSURFACE frameBuffer, G_VECTOR2<int> position, bool clipping)
{
	position.y = frameBuffer.height - position.y;
	G_COLOR32 c = { 0,0,0,0 };
	
	if (position.x < 0 || position.y < 0 || position.y >= frameBuffer.height || position.x >= frameBuffer.width)
			return c;
	
	unsigned int k = frameBuffer.memory[position.x + position.y * frameBuffer.width];//b, g, r, a
	
	c.b = k;
	c.g = k >> 8;
	c.r = k >> 16;
	c.a = k >> 24;

	return c;
}


int DrawRect(G_PIXELSURFACE frameBuffer, G_VECTOR2<float> MinCorner, G_VECTOR2<float> MaxCorner, G_COLOR32 color)
{
	if (frameBuffer.memory == NULL)
		return FUNCTION_INVALID_MEMORY;

	if (MinCorner.x < 0)
		MinCorner.x = 0;

	if (MinCorner.y< 0)
		MinCorner.y = 0;

	if (MaxCorner.x> frameBuffer.width - 1)
		MaxCorner.x = frameBuffer.width - 1;

	if (MaxCorner.y> frameBuffer.height - 1)
		MaxCorner.y = frameBuffer.height - 1;

	for (int cY = MinCorner.y; cY <= MaxCorner.y; cY++)
		for (int cX = MinCorner.x; cX <= MaxCorner.x; cX++)
		{
			frameBuffer.memory[cX + frameBuffer.width * (frameBuffer.height - cY - 1)] = (color.b | (color.g << 8) | (color.r << 16) | (color.a << 24));
		}
	return FUNCTION_SUCCESS;
}

int DrawCircle(G_PIXELSURFACE backBuffer, G_VECTOR2<float> pos, float rad, G_COLOR32 color, bool fill)
{
	if (backBuffer.memory == NULL)
		return FUNCTION_INVALID_MEMORY;

	if (rad <= 0.0f)
		return FUNCTION_SUCCESS;
	
	int range = 0.5 + rad / 1.3;

	if (fill)
	{
		for (int tx = 0; tx <= range; tx++)
		{
			int ty = 0.5 + sqrt(rad*rad - tx * tx);

			for (float ix = -tx + pos.x; (ix <= tx + pos.x); ix++)
			{
				putPixel(backBuffer, { ix, ty + pos.y }, color, true);
				putPixel(backBuffer, { ix, -ty + pos.y }, color, true);
			}

			for (float ix = -ty + pos.x; (ix <= ty + pos.x); ix++)
			{
				putPixel(backBuffer, { ix, tx + pos.y }, color, true);
				putPixel(backBuffer, { ix, -tx + pos.y }, color, true);
			}
		}
	}
	else
	{
		for (float tx = 0; tx <= range; tx++)
		{
			float ty = sqrt(rad*rad - tx * tx);

			putPixel(backBuffer, { tx + pos.x, ty + pos.y }, color, true);
			putPixel(backBuffer, { -tx + pos.x, ty + pos.y }, color, true);

			putPixel(backBuffer, { tx + pos.x, -ty + pos.y }, color, true);
			putPixel(backBuffer, { -tx + pos.x, -ty + pos.y }, color, true);

			putPixel(backBuffer, { ty + pos.x, tx + pos.y }, color, true);
			putPixel(backBuffer, { -ty + pos.x, tx + pos.y }, color, true);

			putPixel(backBuffer, { ty + pos.x, -tx + pos.y }, color, true);
			putPixel(backBuffer, { -ty + pos.x, -tx + pos.y }, color, true);
		}
	}
	return FUNCTION_SUCCESS;
}

int DrawLine(G_PIXELSURFACE backBuffer, G_VECTOR2<float> pos1, G_VECTOR2<float> pos2, G_COLOR32 color)
{
	if (backBuffer.memory == NULL)
		return FUNCTION_INVALID_MEMORY;

	float slope = (pos1.y - pos2.y) / (pos1.x - pos2.x);

	if (slope == INFINITY)
		return -1;
	if (slope == INFINITE)
		return -1;

	G_COLOR32 result = color;
	if (slope > 1 || slope < -1)
	{
		//iterate by y axis
		float iy = min(pos1.y, pos2.y);
		float iyMax = max(pos1.y, pos2.y);
		
		for (; iy < iyMax; iy++)
		{
			float valX = 0.5f + pos1.x + (iy - pos1.y) / slope;
			float diff = 255.0f*(valX -floor(valX));
			
			result.a = (unsigned char)(diff);

			putPixel(backBuffer, { valX+1,iy }, result, true);

			result.a = (unsigned char)(255-diff);
			putPixel(backBuffer, {valX,iy}, result, true);
		}
	}
	else
	{
		//terate by x axis
		float ix = min(pos1.x, pos2.x );
		float ixMax = max(pos1.x, pos2.x);
		
		for (; ix < ixMax; ix++)
		{
			float valY = 0.5f+(ix-pos1.x)*slope + pos1.y;
			float diff = 255.0f*(valY - floor(valY));

			result.a = (unsigned char)(diff);
			putPixel(backBuffer, { ix, valY+1 }, result, true);

			result.a = (unsigned char)(255 - diff);
			putPixel(backBuffer, { ix, valY }, result, true);
		}
	}

	return FUNCTION_SUCCESS;
}

void displayPolygon(G_PIXELSURFACE backBuffer, std::vector<G_VECTOR2<float>> nodes, G_VECTOR2<float> pos, G_COLOR32 vertexColor, G_COLOR32 edgeColor, float vertexRadius,float normalLength)
{
	int nodeSize = nodes.size();
	
	for (int i = 0; i < nodeSize - 1; i++)
	{
		DrawLine(backBuffer, pos + nodes[i], pos + nodes[i + 1], edgeColor);
	}

	DrawLine(backBuffer, pos + nodes[nodeSize - 1], pos + nodes[0], edgeColor);

	if (vertexRadius > 0)
	{
		DrawCircle(backBuffer, pos + nodes[0], vertexRadius*2, vertexColor, true);

		for (int i = 1; i < nodeSize; i++)
			DrawCircle(backBuffer, pos + nodes[i], vertexRadius, vertexColor, true);
	}

	if (normalLength > 0)
	{
		for (int i = 0; i < nodeSize - 1; i++)
		{
			G_VECTOR2<float> normal = (nodes[i] - nodes[i + 1]).perp_left();
			normal.toUnitVect();
			normal = normal * 10;
			DrawLine(backBuffer, pos + (nodes[i] + nodes[i + 1]) / 2, pos + ((nodes[i] + nodes[i + 1]) / 2) + normal, edgeColor);
		}

		G_VECTOR2<float> normal = (nodes[nodeSize - 1] - nodes[0]).perp_left();
		normal.toUnitVect();
		normal = normal * 10;
		DrawLine(backBuffer, pos + (nodes[nodeSize - 1] + nodes[0]) / 2, pos + ((nodes[nodeSize - 1] + nodes[0]) / 2) + normal, edgeColor);

	}
}


bool loadFont(FONT* font, const char* filename)
{
	FILE* bmpFile;
	fopen_s(&bmpFile, filename, "rb");

	//read the file signature
	char signature[3];
	fread(signature, 1, 3, bmpFile);

	if (!(signature[0] == 'B' && signature[1] == 'M' && signature[2] == 'F'))
	{
		//not correct file format
		fclose(bmpFile);
		return false;
	}

	//read format version number
	char version;
	fread(&version, sizeof(char), 1, bmpFile);

	/*
	Version 1 (introduced with application version 1.8).

	Version 2 (introduced with application version 1.9)
	--->added the outline field in the infoBlock and the encoded field
		in the commonBlock.

	Version 3 (introduced with application version 1.10)
	--->removed the encoded field in the commonBlock, and added
		the alphaChnl, redChnl, greenChnl, blueChnl instead.
		The size of each block is now stored without accounting
		for the size field itself. The character id in
		the charsBlock and the kerningPairsBlock was increased to 4
		bytes to support the full unicode range.
	*/

	if (font == NULL)
	{
		fclose(bmpFile);
		return false;	
	}

	//font = (FONT*)malloc(sizeof(FONT));
	char blockInfo;
	int blockSize;

	//INFO
	fread(&blockInfo, 1, 1, bmpFile);
	fread(&blockSize, 4, 1, bmpFile);
	fread(&font->info, 14, 1, bmpFile);
	font->info.fontName = (char*)malloc(blockSize - 14);
	fread(font->info.fontName, blockSize - 14,1, bmpFile);

	//COMMON
	fread(&blockInfo, 1, 1, bmpFile);
	fread(&blockSize, 4, 1, bmpFile);
	fread(&font->common, blockSize, 1, bmpFile);

	//PAGES
	fread(&blockInfo, 1, 1, bmpFile);
	fread(&blockSize, 4, 1, bmpFile);
	/*
	fpos_t position1,position2;
	fgetpos(bmpFile, &position1);
	char temp;
	do {
		fread(&temp,1,1,bmpFile);
	} while (temp != '\0');

	fgetpos(bmpFile, &position2);

	int fileName = position2 - position1;

	fsetpos(bmpFile, &position1);
	*/

	font->nPages = 1;
	font->pages = (PAGES*)malloc(font->nPages * sizeof(PAGES));
	font->pages[0].pageNames = (char*)malloc(blockSize);
	//for(int i= 0; i<font->nPages; i++)
		fread(font->pages[0].pageNames, blockSize, 1, bmpFile);

	//CHARS
	fread(&blockInfo, 1, 1, bmpFile);
	fread(&blockSize, 4, 1, bmpFile);
	font->nChars = blockSize / 20;
	font->chars = (CHARS*)malloc(blockSize);
	fread(font->chars, 20 , font->nChars, bmpFile);

	//KERNING
	fread(&blockInfo, 1, 1, bmpFile);
	fread(&blockSize, 4, 1, bmpFile);
	
	font->kerning = (KERNING*)malloc(blockSize);
	font->nKerning = blockSize / 10;
	fread(font->kerning, 10, font->nKerning , bmpFile);
	
	fclose(bmpFile);

	font->pages[0].fontBitmap.memory = NULL;

	char* name = font->pages[0].pageNames;

	std::string path = filename;
	for (int i = path.size(); i >= 0; i--)
	{
		if (path[i] == '\\')
		{
			break;
		}
		else
		{
			path.erase(i);
		}
	}

	path += name;

	font->pages[0].fontBitmap = loadSprite(path.c_str());
}


void drawFontChar(G_PIXELSURFACE frameBuffer,FONT* font, u_int index, G_VECTOR2<float> pos)
{
	PAGES *page = &font->pages[font->chars[index].page];
	int height = font->chars[index].height;
	int width = font->chars[index].width;

	for (int iy = 0; iy < height; iy++)
	{
		for (int ix = 0; ix < width ; ix++)
		{
			int cx = ix + font->chars[index].x;
			int cy = iy + font->chars[index].y;

			G_COLOR32 pixel = page->fontBitmap.memory[ cx + (page->fontBitmap.height - cy -1) * page->fontBitmap.height];

			G_VECTOR2<float> pos2 = MakeVector(pos.x + ix, pos.y + iy);

			//putPixel(buffer, pos2, MakeColor(255, 0, 0, 80), true);

			putPixel(frameBuffer, pos2, pixel, true);
			
		}
	}

	
}


void printText(G_PIXELSURFACE frameBuffer, FONT* font, G_VECTOR2<float> pos_t, std::string text)
{
	G_VECTOR2<float> pos = pos_t;

	u_int charId = 0;
	CHARS kar;
	bool newLine = true;
	for (int i = 0; i < text.size(); i++)
	{
		//check for new line
		if (text[i] == '\n')
		{
			pos.y = pos.y + font->common.base;
			pos.x = pos_t.x;
			newLine = true;
			continue;
		}
		
		u_int lastCharId = charId;
		charId = (u_int)text[i];
		
		int index = 0;
		for (; index < font->nChars; index++)
		{
			if (font->chars[index].id == charId)
				break;
		}

		if (index == font->nChars)
		{
			// character not fount
			continue;
		}

		if (i != 0 && newLine == false)
		{
			//do kerning if necessary(if pairs are found)
			for (int ik = 0; ik < font->nKerning; ik++)
			{
				if (font->kerning[ik].first == lastCharId &&
					font->kerning[ik].second == charId)
				{
					pos.x = pos.x + font->kerning[ik].amount;
					break;
				}
			}

			G_VECTOR2<float> ad = MakeVector(kar.xadvance, 0);
			pos = pos + ad;
		}

		kar = font->chars[index];

		G_VECTOR2<float> prec = MakeVector(kar.xoffset, kar.yoffset);
		drawFontChar(frameBuffer,font,index,pos + prec);
		newLine = false;
	}
}

