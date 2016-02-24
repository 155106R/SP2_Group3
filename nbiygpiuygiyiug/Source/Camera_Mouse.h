#ifndef CAMERA_MOUSE_H
#define CAMERA_MOUSE_H

#include "Camera.h"
#include "MyMath.h"

using Math::DegreeToRadian;

class Camera_Mouse : public Camera
{
public:
	//Vector3 position;
	//Vector3 target;
	//Vector3 up;
	Vector3 target, up, right, view;
	Vector3 defaultPosition;
	Vector3 defaultTarget;
	Vector3 defaultUp;
	Vector3 frontTarget;
	float cameraRotationX, cameraRotationY;
	int Counter;
	int screenMidX;
	int screenMidY;

	float maxjump;
	float gravityForce;
	float jumpForce;
	bool _jump;

	Camera_Mouse();
	~Camera_Mouse();
	virtual void Init(const Vector3& pos, const Vector3& target, const Vector3& up);
	virtual void Update(double dt);
	virtual void Reset();
	bool Camera_Mouse::checkCamera(int minX, int minZ, int maxX, int maxZ);
	virtual void boundCheck(float minX, float minZ, float maxX, float maxZ);
	virtual void boundCheck2(float minX, float minZ, float maxX, float maxZ);
	void jump(float dt);
};

#endif
