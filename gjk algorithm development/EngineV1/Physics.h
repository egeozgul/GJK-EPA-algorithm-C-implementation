#pragma once
#include "commonStuff.h"
#include "Graphics.h"
#include <vector>
#include <algorithm> 

bool isInRect(G_VECTOR2<float> A, G_VECTOR2<float> B, G_VECTOR2<float> C, G_VECTOR2<float> D, G_VECTOR2<float> M);

std::vector<G_VECTOR2<float>> circularConcaveHullScanner(G_SPRITE* sprite, float deltaAngle, bool display_scanlines, G_COLOR32 A, G_COLOR32 B);


std::vector<G_VECTOR2<float>> convertConcaveToConvexHull(std::vector<G_VECTOR2<float>> Vertices);


struct G_OBJECT
{
	G_VECTOR2<float> oldPos;
	G_VECTOR2<float> newPos;

	G_VECTOR2<float> size;
	float spriteAngle;

	G_SPRITE image;

	std::vector<G_VECTOR2<float>> convexHull;
	std::vector<G_VECTOR2<float>> concaveHull;
};

void setPosition(G_OBJECT *object, G_VECTOR2<float> newPosition);
void move(G_OBJECT *object, G_VECTOR2<float> displacement);



int generateConvexHull(G_OBJECT *object, float deltaAngle, G_COLOR32 A, G_COLOR32 B);

/**
vertices of object_B is checked if any are inside object_A
but it is not checked if any vertex of object_A is inside object_B.
So use the function twice if necessary by swapping the arguments for high precision.
*/
bool convexHullCollision_SAT(G_OBJECT *object_A, G_OBJECT *object_B);

void collisionResponse(G_PIXELSURFACE backBuffer, G_OBJECT *object_A, G_OBJECT *object_B);

int findFurthestPoint(std::vector<G_VECTOR2<float>> *A, G_VECTOR2<float> normal);


std::vector<G_VECTOR2<float>> minskowskiSum(std::vector<G_VECTOR2<float>> *A, std::vector<G_VECTOR2<float>> *B);// B around A

std::vector<G_VECTOR2<float>> minskowskiDifference(std::vector<G_VECTOR2<float>> *A, std::vector<G_VECTOR2<float>> *B); // B around A
