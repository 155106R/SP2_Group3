#include "FPCamera.h"
#include "Application.h"
#include "Mtx44.h"

FPCamera::FPCamera()
{
}


FPCamera::~FPCamera()
{
}

void FPCamera::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();
}

void FPCamera::Update(double dt){

	static const float MOVE_SPEED = 100.0f;
	static const float CAMERA_SPEED = MOVE_SPEED * 2;
	if (Application::IsKeyPressed('A'))
	{
		Vector3 view = (target - position).Normalized();
		Vector3 right = view.Cross(up);
		view = target - position;
		view.Normalize();
		position -= right * dt * MOVE_SPEED;
		target -= right * dt * MOVE_SPEED;
	}
	if (Application::IsKeyPressed('D'))
	{
		Vector3 view = (target - position).Normalized();
		Vector3 right = view.Cross(up);
		view = target - position;
		view.Normalize();
		position += right * dt * MOVE_SPEED;
		target += right * dt * MOVE_SPEED;
	}
	if (Application::IsKeyPressed('W'))
	{
		Vector3 view = (target - position).Normalized();
		position.x += view.x * dt * MOVE_SPEED;
		position.z += view.z * dt * MOVE_SPEED;

		target.x += view.x * dt * MOVE_SPEED;
		target.z += view.z * dt * MOVE_SPEED;
	}
	if (Application::IsKeyPressed('S'))
	{
		Vector3 view = (target - position).Normalized();
		position.x -= view.x * dt * MOVE_SPEED;
		position.z -= view.z * dt * MOVE_SPEED;

		target.x -= view.x * dt * MOVE_SPEED;
		target.z -= view.z * dt * MOVE_SPEED;
	}

	if (Application::IsKeyPressed(VK_UP))
	{
		float pitch = (float)(-CAMERA_SPEED * dt);
		Vector3 view = (target - position).Normalized();
		Vector3 right = view.Cross(up);
		Mtx44 rotation;
		rotation.SetToRotation(-pitch, right.x, right.y, right.z);
		up = rotation * up;
		view = rotation * view;
		target = position + view;
	}

	if (Application::IsKeyPressed(VK_DOWN))
	{
		float pitch = (float)(-CAMERA_SPEED * dt);
		Vector3 view = (target - position).Normalized();
		Vector3 right = view.Cross(up);
		Mtx44 rotation;
		rotation.SetToRotation(pitch, right.x, right.y, right.z);
		up = rotation * up;
		view = rotation * view;
		target = position + view;
	}

	if (Application::IsKeyPressed(VK_LEFT))
	{
		float yaw = (float)(-CAMERA_SPEED * dt);
		Mtx44 rotation;
		rotation.SetToRotation(-yaw, 0, 1, 0);
		Vector3 view = position - target;
		//view.Normalize();
		view = rotation * view;
		position = target + view;
		up = rotation * up;
	}

	if (Application::IsKeyPressed(VK_RIGHT))
	{
		float yaw = (float)(-CAMERA_SPEED * dt);
		Mtx44 rotation;
		rotation.SetToRotation(yaw, 0, 1, 0);
		Vector3 view = position - target;
		//view.Normalize();
		view = rotation * view;
		position = target + view;
		up = rotation * up;
	}

	if (Application::IsKeyPressed('N'))
	{
		Vector3 direction = target - position;
		if (direction.Length() > 5)
		{
			Vector3 view = (target - position).Normalized();
			position += view * (float)(100.f * dt);
		}
	}

	if (Application::IsKeyPressed('M'))
	{
		Vector3 view = (target - position).Normalized();
		position -= view * (float)(100.f * dt);
	}
	if (Application::IsKeyPressed('R'))
	{
		Reset();
	}

	if (((position.x > 490) || (position.z > 490)) || ((position.x < -240) || (position.z < -490))){
		Reset();
	}
	
}

void FPCamera::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}