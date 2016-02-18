#include "OpenGalaxyCamera.h"
#include "Application.h"
#include "Mtx44.h"

OpenGalaxyCamera::OpenGalaxyCamera()
{
}

OpenGalaxyCamera::~OpenGalaxyCamera()
{
}

void OpenGalaxyCamera::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();
}
double oldMouseX = 0;
double oldMouseY = 0;
void OpenGalaxyCamera::Update(double dt)
{
	static const float CAMERA_SPEED = 100.f;
	Vector3 view = (position - target).Normalized();
	//std::cout << view << std::endl;
	
	if (Application::IsKeyPressed(VK_LEFT) || Application::IsKeyPressed('A'))
	{
		float yaw = (float)(-CAMERA_SPEED * dt);
		Mtx44 rotation;
		rotation.SetToRotation(yaw, 0, 1, 0);
		view = rotation * view;
		up = rotation * up;
	}
	if (Application::IsKeyPressed(VK_RIGHT) || Application::IsKeyPressed('D'))
	{
		float yaw = (float)(CAMERA_SPEED * dt);
		Mtx44 rotation;
		rotation.SetToRotation(yaw, 0, 1, 0);
		view = rotation * view;
		up = rotation * up;
	}
	if (Application::IsKeyPressed(VK_UP) || Application::IsKeyPressed('W'))
	{
		float pitch = (float)(-CAMERA_SPEED * dt);
		Vector3 view = (target - position).Normalized();
		Vector3 right = view.Cross(up);
		right.y = 0;
		right.Normalize();
		up = right.Cross(view).Normalized();
		Mtx44 rotation;
		rotation.SetToRotation(pitch, right.x, right.y, right.z);
		position = rotation * position;
	}
	if (Application::IsKeyPressed(VK_DOWN) || Application::IsKeyPressed('S'))
	{
		float pitch = (float)(CAMERA_SPEED * dt);
		Vector3 view = (target - position).Normalized();
		Vector3 right = view.Cross(up);
		right.y = 0;
		right.Normalize();
		up = right.Cross(view).Normalized();
		Mtx44 rotation;
		rotation.SetToRotation(pitch, right.x, right.y, right.z);
		position = rotation * position;
	}

	position = target + view * 15;

	if (Application::IsKeyPressed('R'))
	{
		Reset();
	}
}

void OpenGalaxyCamera::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}