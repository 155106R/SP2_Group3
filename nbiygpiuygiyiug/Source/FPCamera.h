#ifndef FPCAMERA
#define FPCAMERA

#include "Camera.h"

class FPCamera : public Camera
{
public:
	//Vector3 position;
	//Vector3 target;
	//Vector3 up;

	Vector3 defaultPosition;
	Vector3 defaultTarget;
	Vector3 defaultUp;

	FPCamera();
	~FPCamera();
	virtual void Init(const Vector3& pos, const Vector3& target, const Vector3& up);
	virtual void Update(double dt);
	virtual void Reset();
};

#endif FPCAMERA