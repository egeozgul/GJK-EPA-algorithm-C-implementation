//#pragma comment(linker, "/STACK:200000000") //to set limit of stack memory in bytes
//#pragma comment(linker, "/HEAP:200000000") //to set limit of heap memory in bytes

//library for windows audio stuff: not used yet
//#pragma comment(lib, "winmm.lib")

//standard libraries
#include <windows.h>
#include <stdint.h>
#include <tchar.h>
#include <math.h>
#include <vector>
#include <mmsystem.h>

//global header file: common stuff like vector ,color structs...
#include "commonStuff.h"

//utility header files
#include "Window.h"		//about preparing a window
#include "Input.h"		//input obvsly
#include "Graphics.h"	//grphx
#include "Physics.h"	//psx

//global variables
bool Running = true;
G_WINDOW gameWindow;		//memory portion about the window
G_INPUT ginput;				//memory portion about input
G_PIXELSURFACE frameBuffer;	
G_TIME gtime;				//memory portion about time

//objects
G_OBJECT object1;
G_OBJECT object2;

//fonts
FONT arial;
FONT oldEnglish;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//generate window
	//width: 900
	//height: 600
	gameWindow = createWindow(900, 600, "MyWindow", hInstance);
	
	//initialize input struct
	GInput_Initialize(&ginput);

	//allocate memory for frame buffer(display buffer)
	allocateBufferMemory(&frameBuffer, gameWindow.clientAreaWidth, gameWindow.clientAreaHeight);

	//load fonts from directory: fonts
	loadFont(&oldEnglish, "fonts\\eng.fnt");
	loadFont(&arial, "fonts\\myfont.fnt");

	//load image for the object1
	object1.image = loadSprite("aaa.bmp");
	
	//initialize the var
	object1.spriteAngle = 0;
	
	//set its size equal to the image loaded
	object1.size = MakeVector_toFloat(object1.image.width, object1.image.height);
	
	//using the loaded image, generate a concave polygon which will fit around the loaded figure
	object1.concaveHull = circularConcaveHullScanner(&(object1.image), 0.02f, false, MakeColor(0, 0, 200, 255), MakeColor(111, 111, 255, 255));
	
	//using the generated concave polygon, generate a convex polygon which will be used in collision checks and all other stuff
	object1.convexHull = convertConcaveToConvexHull(object1.concaveHull);

	//initialize position:
	//old pos is the pos in the previous frame, new is the recent pos. old and new pos values allow the algorithms to know the movement of the object similar to using velocity
	object1.oldPos = MakeVector(430, 240);
	object1.newPos = MakeVector(430, 240);

	//create a duplicate of object1
	object2 = object1;

	while (Running)
	{
		//windows messages such as close the window and for input events
		HandleWindowMessage();

		//fixes fps rate to 80fps and outputs the latest frame duration(unsigned int millisecond) and fps(float Hz) into gtime struct (put NULL if not needed).
		fixedFPS(&gtime, 80);
		
		clearFrameBuffer(frameBuffer, black);
				
		setPosition(&object1,ginput.mouse.Pos);
		
		//check collision using SAT algorithm
		bool a = convexHullCollision_SAT(&object2, &object1);
		bool b = convexHullCollision_SAT(&object1, &object2);
		if(a&&b)
		{
			//apply response (the arguement framebuffer is used for debugging purposes).
			collisionResponse(frameBuffer, &object2, &object1);
		}

		//use ginput structure for all input events
		if (ginput.keyboard.isDown('w'))
		{
			move(&object2, MakeVector(0,-1));
		}
		else if (ginput.keyboard.isDown('s'))
		{
			move(&object2, MakeVector(0, 1));
		}
		else if (ginput.keyboard.isDown('d'))
		{
			move(&object2, MakeVector(1, 0));
		}
		else if (ginput.keyboard.isDown('a'))
		{
			move(&object2, MakeVector(-1, 0));
		}


		//everyting related to painting is below:

		drawSprite(frameBuffer, &(object2.image), object2.newPos, object2.size, 0);
		displayPolygon(frameBuffer, object2.convexHull, object2.newPos, red, red, 1, 5);

		drawSprite(frameBuffer,&(object1.image), object1.newPos, object1.size, 0);
		displayPolygon(frameBuffer, object1.convexHull, object1.newPos,red, red, 1,5);
		
		//drawSprite(backbuffer, &f.pages[0].fontBitmap, MakeVector(128 + 25, 128 + 25), object.size, 0);
		//G_VECTOR2<float> tt = MakeVector(340, 240);
		//displayPolygon(backbuffer, minskowskiDifference(&(object2.convexHull), &(object.convexHull)), object2.newPos - object.newPos + tt, white, white, 1,4);
		

		//reduce the update frequency of the fps text displayed so that normal human beings can read it without becoming blind or psycho
		static float fps = 0;
		static int dur = 0;
		static int timer1 = 0;
		static float frq = 5;
		
		timer1 += gtime.frameDuration;
		if (timer1 > 1000/frq)
		{
			fps = gtime.currentFPS;
			dur = gtime.frameDuration;

			timer1 = 0;
		}
		
		//print the prepared fps text using eprint hoca
		eprint(frameBuffer, &arial, ginput.mouse.Pos, "FPS %f\nDUR %d", fps, dur);
		
		//everything is painted on the frameBuffer array so far. give the frameBuffer to windows abi so that he can get it displayed on your screen.
		displayBuffer(frameBuffer, gameWindow);
	}
	
	//when the programe terminates...
	deallocateBufferMemory(&frameBuffer);
	return 0;
}


//this funtion is a magical function to handle windows message events. msg is the message received from windows abi. depending on the message, do what is needed.
//hwnd is the hanle of the window, not really important.
//wParam and lParam are used to receive more specific values from the windows abi such as mouse position, etc.
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT Result = 0;
	switch (msg)
	{
		case WM_CLOSE:
		{
			Running = false;
		}break;
		case WM_DESTROY:
		{
			Running = false;
		}break;
		
		//window size is changed in this case, so reallocate the frameBuffer to the new size wich are received as well using lParam
		case WM_SIZE:
		{
			gameWindow.clientAreaWidth = GET_X_LPARAM(lParam);
			gameWindow.clientAreaHeight = GET_Y_LPARAM(lParam);
			
			deallocateBufferMemory(&frameBuffer);

			allocateBufferMemory(&frameBuffer, gameWindow.clientAreaWidth, gameWindow.clientAreaHeight);
			
		}break;
		default:
		{
			//keyboard and mouse inputs are obtained 
			GetMessages(&ginput,msg,wParam, lParam);

			Result = DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}
	return (Result);
}


