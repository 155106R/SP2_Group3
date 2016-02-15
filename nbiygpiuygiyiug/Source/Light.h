#ifndef LIGHT_H
#define LIGHT_H

#include "Vertex.h"

struct Light
{
public:

	enum LIGHT_TYPE
	{
		LIGHT_POINT = 0,
		LIGHT_DIRECTIONAL,
		LIGHT_SPOT,
	};

	LIGHT_TYPE type;
	Vector3 spotDirection;
	Position position;
	Color color;

	float power;
	float kC, kL, kQ;
	float cosCutoff;
	float cosInner;
	float exponenet;
	Light()
	{
		position.Set(0, 0, 0);
		color.Set(0, 0, 0);
		power = 1.f;
		kC = 1.f;
		kL = 1.f;
		kQ = 1.f;
	}	
	~Light() {};
};

#endif