
#ifndef CAMERA_DRILL_H
#define CAMERA_DRILL_H

#include "Camera.h"
#include "MyMath.h"

using Math::DegreeToRadian;

class Camera_drill : public Camera
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
	Vector3 tempPosition;
	Vector3 nextPosition; 

	float cameraRotationX, cameraRotationY;
	int Counter;
	int screenMidX;
	int screenMidY;

	bool colliding;
	int move;
	float maxjump;
	float gravityForce;
	float jumpForce;
	bool _jump;
	float walkSpeed;

	Camera_drill();
	~Camera_drill();
	virtual void Init(const Vector3& pos, const Vector3& target, const Vector3& up);
	virtual void Update(double dt);
	virtual void Reset();
	bool Camera_drill::checkCamera(int minX, int minZ, int maxX, int maxZ);


	enum movement
	{
		FOWARD,
		BACK,
		LEFT,
		RIGHT,

		NUM_MOVEMENT



	};
};

#endif
