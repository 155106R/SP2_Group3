#ifndef OPEN_GALAXY_CAMERA_H
#define OPEN_GALAXY_CAMERA_H

#include "Camera.h"
#include "MyMath.h"
using Math::DegreeToRadian;
class OpenGalaxyCamera : public Camera
{
public:
	Vector3 defaultPosition;
	Vector3 defaultTarget;
	Vector3 defaultUp;

	OpenGalaxyCamera();
	~OpenGalaxyCamera();
	virtual void Init(const Vector3& pos, const Vector3& target, const Vector3& up);
	virtual void Update(double dt);
	virtual void Reset();
	float getHordistance();
	float getVertdistance();

	void getcameraCoords(float h, float v);
	

private:
	float pitch;
	float distance;
	float yaw;
	

};

#endif
//Updated 22/2/2016 - Randall