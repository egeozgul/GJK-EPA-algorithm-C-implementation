#include "Physics.h"


/**
if point M is in rect ABCD, true is returned. false otherwise
*/
bool isInRect(G_VECTOR2<float> A, G_VECTOR2<float> B, G_VECTOR2<float> C, G_VECTOR2<float> D, G_VECTOR2<float> M)
{
	B = B - A;
	D = D - A;
	C = C - A;
	M = M - A;

	if ((M.y - D.y)*(D.x) <(D.y)*(M.x - D.x) &&
		(M.y - B.y)*(D.x) >(D.y)*(M.x - B.x) &&
		(M.y - B.y)*(B.x) >(B.y)*(M.x - B.x) &&
		(M.y - D.y)*(B.x) <(B.y)*(M.x - D.x))
		return true;

	return false;
}


G_VECTOR2<float> scanline(G_SPRITE* sprite, G_VECTOR2<float> pos1, G_VECTOR2<float> center, G_COLOR32 CA, G_COLOR32 CB)
{
	float slope = (pos1.y - center.y) / (pos1.x - center.x);

	G_VECTOR2<float> point;

	if (slope > 1 || slope < -1)
	{
		if (pos1.y < center.y)
		{
			//iterate by y axis
			for (int iy = pos1.y; iy < center.y; iy++)
			{
				int valX = 0.5f + pos1.x + (iy - pos1.y) / slope;

				if (valX < 0 || valX >= sprite->width)
					continue;

				if (iy < 0 || iy >= sprite->height)
					continue;

				G_COLOR32 pixel = sprite->memory[valX + iy * sprite->width];

				if ((pixel.a <= max(CA.a, CB.a)) && (pixel.a >= min(CA.a, CB.a)) &&
					(pixel.r <= max(CA.r, CB.r)) && (pixel.r >= min(CA.r, CB.r)) && 
					(pixel.g <= max(CA.g, CB.g)) && (pixel.g >= min(CA.g, CB.g)) && 
					(pixel.b <= max(CA.b, CB.b)) && (pixel.b >= min(CA.b, CB.b)))
					return { (float)valX, (float)iy };
			}
		}
		else
		{
			//iterate by y axis
			for (int iy = pos1.y; iy > center.y; iy--)
			{
				int valX = 0.5f + pos1.x + (iy - pos1.y) / slope;

				if (valX < 0 || valX >= sprite->width)
					continue;

				if (iy < 0 || iy >= sprite->height)
					continue;

				G_COLOR32 pixel = sprite->memory[valX + iy * sprite->width];

				if ((pixel.a <= max(CA.a, CB.a)) && (pixel.a >= min(CA.a, CB.a)) &&
					(pixel.r <= max(CA.r, CB.r)) && (pixel.r >= min(CA.r, CB.r)) &&
					(pixel.g <= max(CA.g, CB.g)) && (pixel.g >= min(CA.g, CB.g)) &&
					(pixel.b <= max(CA.b, CB.b)) && (pixel.b >= min(CA.b, CB.b)))
					return { (float)valX, (float)iy };
			}
		}
	}
	else
	{
		if (pos1.x < center.x)
		{
			//iterate by x axis
			for (int ix = pos1.x; ix < center.x; ix++)
			{
				int valY = 0.5f + (ix - pos1.x)*slope + pos1.y;

				if (ix < 0 || ix >= sprite->width)
					continue;

				if (valY < 0 || valY >= sprite->height)
					continue;

				G_COLOR32 pixel = sprite->memory[ix + valY * sprite->width];

				if ((pixel.a <= max(CA.a, CB.a)) && (pixel.a >= min(CA.a, CB.a)) &&
					(pixel.r <= max(CA.r, CB.r)) && (pixel.r >= min(CA.r, CB.r)) &&
					(pixel.g <= max(CA.g, CB.g)) && (pixel.g >= min(CA.g, CB.g)) &&
					(pixel.b <= max(CA.b, CB.b)) && (pixel.b >= min(CA.b, CB.b)))
					return { (float)ix, (float)valY };
			}
		}
		else
		{
			//iterate by x axis
			for (int ix = pos1.x; ix > center.x; ix--)
			{
				int valY = 0.5f + (ix - pos1.x)*slope + pos1.y;

				if (ix < 0 || ix >= sprite->width)
					continue;

				if (valY < 0 || valY >= sprite->height)
					continue;

				G_COLOR32 pixel = sprite->memory[ix + valY * sprite->width];
				if ((pixel.a <= max(CA.a, CB.a)) && (pixel.a >= min(CA.a, CB.a)) &&
					(pixel.r <= max(CA.r, CB.r)) && (pixel.r >= min(CA.r, CB.r)) &&
					(pixel.g <= max(CA.g, CB.g)) && (pixel.g >= min(CA.g, CB.g)) &&
					(pixel.b <= max(CA.b, CB.b)) && (pixel.b >= min(CA.b, CB.b)))
					return { (float)ix, (float)valY };
			}
		}
	}

	return { (float)(-1), (float)(-1) };
}


std::vector<G_VECTOR2<float>> circularConcaveHullScanner(G_SPRITE* sprite, float deltaAngle, bool display_scanlines, G_COLOR32 A, G_COLOR32 B)
{
	std::vector<G_VECTOR2<float>> nodes;
	G_VECTOR2<float> point;
	G_VECTOR2<float> center;
	center.x = sprite->width / 2;
	center.y = sprite->height / 2;

	float radius = sqrtf((sprite->height / 2)*(sprite->height / 2) + (sprite->width / 2)*(sprite->width / 2));

	for (float angle = 0; angle < 2 * PI; angle += deltaAngle)
	{
		G_VECTOR2<float> outerPoint;
		outerPoint.x = radius * cosf(angle);
		outerPoint.y = radius * sinf(angle);
		outerPoint = center + outerPoint;

		point = scanline(sprite, outerPoint, center, A,B);

		//DrawLine(backBuffer, { outerPoint.x,300 - outerPoint.y }, { center.x, 300 - center.y }, blue);

		if (point.x > 0 && point.y > 0)
		{
			nodes.push_back(point);
		}
	}

	for (int i = 0; i < nodes.size(); i++)
	{
		nodes[i].y = sprite->height - nodes[i].y;
		nodes[i] = nodes[i]-center;
	}

	return nodes;
}


std::vector<G_VECTOR2<float>> convertConcaveToConvexHull(std::vector<G_VECTOR2<float>> Vertices)
{
	//find the greatest y valued point in Vertices list
	G_VECTOR2<float> referencePoint = MakeVector(0, -9999999999999); //greatest oordinate value

	int referencePointIndex;
	for (int i = 0; i < Vertices.size(); i++)
	{
		if (Vertices[i].y > referencePoint.y)
		{
			referencePoint = Vertices[i];
			referencePointIndex = i;
		}
	}

	for (int i = 0; i <= referencePointIndex; i++)
	{
		Vertices.push_back(Vertices[0]);
		Vertices.erase(Vertices.begin());
	}
	
	std::vector<G_VECTOR2<float>> convexHull;

	if (Vertices.size() < 2)
	{
		return Vertices;
	}
	else
	{
		convexHull.push_back(Vertices[0]);
		convexHull.push_back(Vertices[1]);
		convexHull.push_back(Vertices[2]);

		int listSize = Vertices.size();

		for (int i = 3; i < listSize; i++)
		{
			while ((Vertices[i].y - convexHull[convexHull.size() - 1].y) *	(convexHull[convexHull.size() - 1].x - convexHull[convexHull.size() - 2].x) >=
				(convexHull[convexHull.size() - 1].y - convexHull[convexHull.size() - 2].y) * (Vertices[i].x - convexHull[convexHull.size() - 1].x))
			{
				convexHull.pop_back();

				if (convexHull.size() < 2)
					break;

			}

			convexHull.push_back(Vertices[i]);
		}
	}
	
	return convexHull;
}



bool convexHullCollision_SAT(G_OBJECT *object_A, G_OBJECT *object_B)
{
	//SAT algoritm:

	int sizeConvex = object_A->convexHull.size();
	int sizeB = object_B->convexHull.size();

	for (int i = 0; i < sizeConvex; i++)
	{
		//consecutive vertices on object_A's convex hull: a and b
		G_VECTOR2<float> a = object_A->newPos + object_A->convexHull[i];
		G_VECTOR2<float> b = object_A->newPos + object_A->convexHull[(1 + i) % sizeConvex];

		//the normal to the vector of a-b
		G_VECTOR2<float> normal = (b - a).perp_right();

		//projection distance is the distance of the shadow when a vector is projected onto another vector https://www.youtube.com/watch?v=LyGKycYT2v0 (time 2:19) see length of projected
		//the projection distance of the vertex of object_B onto the normal vector defined above
		float projectionDistance_A_min = normal.dot(object_A->newPos + object_A->convexHull[0]);
		float projectionDistance_A_max = projectionDistance_A_min;

		//find the min and max projection distance values of the vertices of object_A on the current normal:
		for (int i1 = 1; i1 < sizeConvex; i1++)
		{
			projectionDistance_A_min = min(projectionDistance_A_min, normal.dot(object_A->newPos + object_A->convexHull[i1]));// / normal.lenght());
			projectionDistance_A_max = max(projectionDistance_A_max, normal.dot(object_A->newPos + object_A->convexHull[i1]));// / normal.lenght());
		}

		float projectionDistance_B_min = normal.dot(object_B->newPos + object_B->convexHull[0]);
		float projectionDistance_B_max = projectionDistance_B_min;

		//find the min and max projection distance values of the vertices of object_B on the current normal:
		for (int i2 = 1; i2 < sizeB; i2++)
		{
			projectionDistance_B_min = min(projectionDistance_B_min, normal.dot(object_B->newPos + object_B->convexHull[i2]));// / normal.lenght());
			projectionDistance_B_max = max(projectionDistance_B_max, normal.dot(object_B->newPos + object_B->convexHull[i2]));// / normal.lenght());
		}

		if (projectionDistance_B_min > projectionDistance_A_min && projectionDistance_B_min < projectionDistance_A_max ||
			projectionDistance_B_max > projectionDistance_A_min && projectionDistance_B_max < projectionDistance_A_max ||
			projectionDistance_B_min < projectionDistance_A_min && projectionDistance_B_max > projectionDistance_A_max)
		{
			//there might be a collision
		}
		else
		{
			// no collision for sure!!
			return false;
		}
	}


	return true;
}


int generateConvexHull(G_OBJECT *object,float deltaAngle,G_COLOR32 A, G_COLOR32 B)
{
	if (object->image.memory == NULL)
		return - 1;

	object->concaveHull = circularConcaveHullScanner(&(object->image), deltaAngle,false,A,B);
	object->convexHull = convertConcaveToConvexHull(object->concaveHull);

	return 1;
}

/*
//make the list unique
std::unique(Vertices.begin(), Vertices.end(), areEqual);

//find the greatest y valued point in Vertices list
referencePoint = MakeVector(0,0); //greatest oordinate value

int referencePointIndex;
for (int i = 0; i < Vertices.size(); i++)
{
if (Vertices[i].y > referencePoint.y)
{
referencePoint = Vertices[i];
referencePointIndex = i;
}
}

//remove the referece point from Vertices list
Vertices.erase(Vertices.begin() + referencePointIndex);

//sort the list using reference point
std::sort(Vertices.begin(), Vertices.end(),y_IsGreater);

*/

G_VECTOR2<float> support(G_VECTOR2<float> normal, std::vector<G_VECTOR2<float>> A, std::vector<G_VECTOR2<float>> B, int* index_A = NULL, int* index_B = NULL)
{
	int maxIndex_A = 0;
	float maxDotProduct = normal.dot(A[0]);

	for (int i = 1; i < A.size(); i++)
	{
		float dotProduct = normal.dot(A[i]);

		if (dotProduct > maxDotProduct)
		{
			maxIndex_A = i;
			maxDotProduct = dotProduct;
		}
	}

	if (index_A != NULL)
		*index_A = maxIndex_A;

	int maxIndex_B = 0;
	maxDotProduct = normal.dot(B[0]);

	for (int i = 1; i < B.size(); i++)
	{
		float dotProduct = normal.dot(B[i]);

		if (dotProduct < maxDotProduct)
		{
			maxIndex_B = i;
			maxDotProduct = dotProduct;
		}
	}

	if (index_B != NULL)
		*index_B = maxIndex_B;

	G_VECTOR2<float> point = A[maxIndex_A] - B[maxIndex_B];

	return point;
}

int findFurthestPoint(std::vector<G_VECTOR2<float>> *A, G_VECTOR2<float> normal)
{
	int maxDistIndex = 0;
	float maxDist = A->at(0).dot(normal);

	int listSize = A->size();
	for (int i = 1; i < listSize; i++)
	{
		float dist = A->at(i).dot(normal);
		
		if (dist > maxDist)
		{
			maxDist = dist;
			maxDistIndex = i;
		}
	}

	return maxDistIndex;
}

bool funct(G_VECTOR2<float> A , G_VECTOR2<float> B)
{
	return (A.lenght() < B.lenght());
}


G_VECTOR2<float> intersectionOfLines(G_VECTOR2<float> c, G_VECTOR2<float> b, G_VECTOR2<float> point)
{
	G_VECTOR2<float> m;

	if (b.x - c.x == 0)
	{
		m.x = b.x;
		m.y = b.x* point.y / point.x;
		return m;
	}
	
	if(c.y - b.y == 0)
	{
		m.y = b.y;
		m.x = b.y * point.x/ point.y ;
		return m;
	}

	float partA = point.x * (b.x*c.y- b.y*c.x);
	float partB = ((b.x - c.x)*point.y + (c.y - b.y)*point.x);

	
	m.x = partA/partB;
	
	if (partB == 0)
	{
		int yy = 0;
	}

	if (point.x == 0)
	{
		m.y = point.y;
	}
	else
	{
		m.y = point.y*m.x / point.x;
	}	

	return m;
}

//#define G_DEBUG

void collisionResponse(G_PIXELSURFACE backBuffer, G_OBJECT *object_A, G_OBJECT *object_B)
{
	G_VECTOR2<float> intr;

	G_VECTOR2<float> dist_A = object_A->newPos - object_A->oldPos;
	G_VECTOR2<float> dist_B = object_B->newPos - object_B->oldPos;
	G_VECTOR2<float> dist_R = dist_A - dist_B;
	
	if (dist_R.x == 0 && dist_R.y == 0) 
		return;

	int size_A = object_A->convexHull.size();
	int size_B = object_B->convexHull.size();

	int indexA, indexB;
	G_VECTOR2<float> firstScanPoint = support(dist_R,object_A->convexHull, object_B->convexHull,&indexA, &indexB);
	
	std::vector<G_VECTOR2<float>> B = object_B->convexHull;
	for (int i = 0; i < size_B; i++)
		B[i] = B[i].negative();


#ifdef G_DEBUG
	G_VECTOR2<float> rr = MakeVector(340, 240);
	DrawLine(backBuffer, dist_R + rr, rr, blue);
	
	G_VECTOR2<float> origin = object_A->newPos - object_B->newPos + rr;

	displayPolygon(backBuffer, object_A->convexHull, origin, white, white, 1,4);
	displayPolygon(backBuffer, B, origin + object_A->convexHull[indexA], white, white, 1,4);
	//DrawCircle(backBuffer, origin + firstScanPoint , 4, blue, true);
	//DrawCircle(backBuffer, origin + object_A->convexHull[indexA], 4, green, true);
#endif

	float regionTest = (object_A->newPos - object_B->newPos + firstScanPoint).dot(dist_R.perp_right());
	
	if (regionTest >= 0 )//go to left
	{
		G_VECTOR2<float> a1;
		if(indexA == 0)
			a1= object_A->convexHull[size_A-1];
		else
			a1 = object_A->convexHull[(indexA - 1) % size_A];

		G_VECTOR2<float> a2 = object_A->convexHull[(indexA) % size_A];

		G_VECTOR2<float> normal = (a2 - a1).perp_right();
		
		int index_B_first = findFurthestPoint(&B, normal);
		int index_B_last;

#ifdef G_DEBUG
		DrawCircle(backBuffer, origin + object_A->convexHull[indexA] + B[index_B_first], 6, white, true);
		DrawCircle(backBuffer, origin + a1, 6, white, true);
		DrawCircle(backBuffer, origin + a2, 6, black, true);
#endif
		G_VECTOR2<float> oldPoint = firstScanPoint;
		G_VECTOR2<float> newPoint;

		for (int i = indexA; i < size_A + indexA; i++)
		{
			//edge i: a1->a2
			a1 = object_A->convexHull.at((i) % (size_A));
			a2 = object_A->convexHull.at((i + 1) % (size_A));

			//normal of edge i
			normal = (a2 - a1).perp_right();

				index_B_last = findFurthestPoint(&B, normal);

				int target = index_B_last;
		
				if (target < index_B_first)
					target += size_B;

				for (int t = index_B_first; t <= target; t++)
				{
					newPoint = object_A->newPos - object_B->newPos + a1 + B.at(t%size_B);
				
					regionTest = (newPoint).dot(dist_R.perp_right());
				
					if (regionTest < 0)
						goto outOfLoops1;

				oldPoint = newPoint;
			}
			
			index_B_first = index_B_last;
		}

	outOfLoops1:

		intr = intersectionOfLines(newPoint, oldPoint, dist_R);
		G_VECTOR2<float> surface = newPoint - oldPoint;
		
		float verticalDepth = intr.dot(surface.perp_right());

		if (verticalDepth < 0)
		{
			//no collision
		//	return;
		}

		float surafeLength = surface.lenght();

		verticalDepth /= surafeLength;

		G_VECTOR2<float> pen = (surface.perp_right());
		pen.toUnitVect();
		pen = pen * verticalDepth;

		object_B->newPos = object_B->newPos + pen;
		 
#ifdef G_DEBUG
		DrawCircle(backBuffer, rr + oldPoint, 4, red, true);
		DrawCircle(backBuffer, rr + newPoint, 4, white, true);
		DrawCircle(backBuffer, rr + intr, 4, white, true);
#endif
	}
	else if (regionTest < 0)//go to right
	{
		//DrawCircle(backBuffer, origin, 15, green, true);
		
		G_VECTOR2<float> a1 = object_A->convexHull[(indexA + 1)%size_A];
		G_VECTOR2<float> a2 = object_A->convexHull[(indexA) % size_A];

		//DrawCircle(backBuffer, origin + a1, 6, white, true);
		//DrawCircle(backBuffer, origin + a2, 6, black, true);

		G_VECTOR2<float> normal = (a2 - a1).perp_left();

		int index_B_first = findFurthestPoint(&B, normal);
		int index_B_last;

		//DrawCircle(backBuffer, origin + object_A->convexHull[indexA] + B[index_B_first], 6, white, true);

		G_VECTOR2<float> oldPoint = firstScanPoint;
		G_VECTOR2<float> newPoint;

		for (int i = indexA + size_A; i > indexA; i--)
		{
			//edge i: a1->a2
			int k2 = i;
			while(k2 < 0) k2 += size_A ;

			a1 = object_A->convexHull.at( k2 % (size_A));
			
			k2 = i-1;
			while (k2 < 0) k2 += size_A;

			a2 = object_A->convexHull.at( k2 % (size_A));
			
			//normal of edge i
			normal = (a2 - a1).perp_left();

			index_B_last = findFurthestPoint(&B, normal);

			int target = index_B_last;

			if (target > index_B_first)
				target = target - size_B;

			for (int t = index_B_first; t >= target; t--)
			{
				int k = t;
				while (k < 0) k += size_B;

				newPoint = object_A->newPos - object_B->newPos + B.at(k%size_B) + a1;
				//DrawCircle(backBuffer, rr + newPoint , 4, black, true);

				regionTest = (newPoint).dot(dist_R.perp_left());

				if (regionTest < 0)
					goto outOfLoops2;
	
				oldPoint = newPoint;
			}		
			index_B_first = index_B_last;
		}
	outOfLoops2:

		intr = intersectionOfLines(newPoint, oldPoint, dist_R);

		G_VECTOR2<float> surface = newPoint - oldPoint;

		float verticalDepth = intr.dot( surface.perp_right());

		if (verticalDepth > 0)
		{
			//no collision
		//	return;
		}

		float surafeLength = surface.lenght();
		
		verticalDepth /= surafeLength;

		G_VECTOR2<float> pen = (surface.perp_right());
		
		pen.toUnitVect();
				
		pen = pen * verticalDepth;
		object_B->newPos = object_B->newPos + pen;		

#ifdef G_DEBUG
		DrawCircle(backBuffer, rr + newPoint, 4, green, true);
		DrawCircle(backBuffer, rr + oldPoint, 4, red, true);
		DrawCircle(backBuffer, rr + intr, 4, white, true);
#endif
	}
	else
	{
		//firstScanPoint;
		return;
	}
}


std::vector<G_VECTOR2<float>> minskowskiSum(std::vector<G_VECTOR2<float>> *A, std::vector<G_VECTOR2<float>> *B) // B around A
{
	std::vector<G_VECTOR2<float>> list;

	int size_A = A->size();
	int size_B = B->size();

	G_VECTOR2<float> a1 = A->at(size_A - 1);
	G_VECTOR2<float> a2 = A->at(0);

	G_VECTOR2<float> normal = (a2 - a1).perp_right();

	int index_B_first, index_B_last;
	index_B_first = findFurthestPoint(B, normal);

	for (int i = 0; i < size_A; i++)
	{
		a1 = A->at((i) % (size_A));
		a2 = A->at((i + 1) % (size_A));

		normal = (a2 - a1).perp_right();

		index_B_last = findFurthestPoint(B, normal);

		int target = index_B_last;

		if (target < index_B_first)
			target += size_B;

		for (int t = index_B_first; t <= target; t++)
		{
			G_VECTOR2<float> point = B->at(t%size_B) + a1;
			list.push_back(point);
		}

		index_B_first = index_B_last;
	}

	return list;
}

std::vector<G_VECTOR2<float>> minskowskiDifference(std::vector<G_VECTOR2<float>> *A, std::vector<G_VECTOR2<float>> *B) // B around A
{
	std::vector<G_VECTOR2<float>> list;

	std::vector<G_VECTOR2<float>> B_modified = *B;

	int size_A = A->size();
	int size_B = B_modified.size();

	for (int i = 0; i < size_B; i++)
		B_modified.at(i) = B_modified.at(i).negative();

	G_VECTOR2<float> a1 = A->at(size_A - 1);
	G_VECTOR2<float> a2 = A->at(0);

	G_VECTOR2<float> normal = (a2 - a1).perp_right();

	int index_B_first, index_B_last;
	index_B_first = findFurthestPoint(&B_modified, normal);

	for (int i = 0; i < size_A; i++)
	{
		//edge i: a1->a2
		a1 = A->at((i) % (size_A));
		a2 = A->at((i + 1) % (size_A));

		//normal of edge i
		normal = (a2 - a1).perp_right();

		index_B_last = findFurthestPoint(&B_modified, normal);

		int target = index_B_last;
		
		if (target < index_B_first)
			target += size_B;

		for (int t = index_B_first; t <= target; t++)
		{
			G_VECTOR2<float> point = B_modified.at(t%size_B) + a1;
			list.push_back(point);
		}

		index_B_first = index_B_last;
	}

	return list;
}

void setPosition(G_OBJECT *object, G_VECTOR2<float> newPosition)
{
	object->oldPos = object->newPos;
	object->newPos = newPosition;
}

void move(G_OBJECT *object, G_VECTOR2<float> displacement)
{
	object->oldPos = object->newPos;
	object->newPos = object->newPos + displacement;
}
