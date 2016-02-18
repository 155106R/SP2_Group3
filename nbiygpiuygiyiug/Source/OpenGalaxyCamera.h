#ifndef OPEN_GALAXY_CAMERA_H
#define OPEN_GALAXY_CAMERA_H

#include "Camera.h"

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
};

#endif