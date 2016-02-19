#include "OpenGalaxyCamera.h"
#include "Application.h"
#include "Mtx44.h"
#include "MyMath.h"
#include "math.h"
#include "OpenGalaxyScene.h"
OpenGalaxyCamera::OpenGalaxyCamera()
{
}

OpenGalaxyCamera::~OpenGalaxyCamera()
{
}

void OpenGalaxyCamera::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	//init scamera angle and such
	pitch = 20;
	distance = 50;
	yaw = 0;

	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	Vector3 view = (position-target).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	//right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();
}
double oldMouseX = 0;
double oldMouseY = 0;
void OpenGalaxyCamera::Update(double dt)
{
	//std::cout << position << std::endl;

	static const float CAMERA_SPEED = 100.f;
	//Vector3 view = (position - target).Normalized();
	//Mtx44 rotation;
	//rotation.SetToRotation(0, 0, 1, 0);
	//view = rotation * view;

	//std::cout << position << std::endl;
	//std::cout << view.Normalized() << std::endl;
	//position = target + view * 50;

	if (Application::IsKeyPressed(VK_LEFT) || Application::IsKeyPressed('A'))
	{
		yaw += (float)(-CAMERA_SPEED * dt);
		/*Mtx44 rotation;
		rotation.SetToRotation(yaw, 0, 1, 0);
		view = rotation * view;
		up = rotation * up;*/
		//position = target + view * 50;
	}
	if (Application::IsKeyPressed(VK_RIGHT) || Application::IsKeyPressed('D'))
	{
		yaw += (float)(CAMERA_SPEED * dt);
		//Mtx44 rotation;
		//rotation.SetToRotation(yaw, 0, 1, 0);
		//view = rotation * view;
		//up = rotation * up;
		//
	}
	if (Application::IsKeyPressed(VK_UP) || Application::IsKeyPressed('W'))
	{
		
		if (pitch < 50)
		{
			pitch += (float)(CAMERA_SPEED * dt);
		}
		std::cout << pitch << std::endl;
	/*	Vector3 view = (target - position).Normalized();
		Vector3 right = view.Cross(up);
		right.Normalize();
		up = right.Cross(view).Normalized();
		Mtx44 rotation;
		rotation.SetToRotation(pitch, right.x, right.y, right.z);*/
		//position = rotation * position;
	}
	if (Application::IsKeyPressed(VK_DOWN) || Application::IsKeyPressed('S'))
	{
		if (pitch > -50)
		{
			pitch += (float)(-CAMERA_SPEED * dt);
		}
	/*	Vector3 view = (target - position).Normalized();
		Vector3 right = view.Cross(up);
		right.y = 0;
		right.Normalize();
		up = right.Cross(view).Normalized();*/
		//Mtx44 rotation;
		//rotation.SetToRotation(pitch, right.x, right.y, right.z);
		////position = rotation * position;
	}

	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	up = right.Cross(view).Normalized();

	float h = getHordistance();
	float v = getVertdistance();
	getcameraCoords(h,v);
	
	//position = target + view * 50;

	//position = (target) - view *50;


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

float OpenGalaxyCamera::getHordistance()
{
	return(float)(distance* cos(DegreeToRadian(pitch)));

}

float OpenGalaxyCamera::getVertdistance()
{
	return(float)(distance* sin(DegreeToRadian(pitch)));

}

void OpenGalaxyCamera::getcameraCoords(float h, float v) 
{
	float theta = yaw;
	float offsetx = (float)(h*sin(DegreeToRadian(theta)));
	float offsetz = (float)(h*cos(DegreeToRadian(theta)));
	position.x = target.x - offsetx;
	position.z = target.z - offsetz;
	
	//vert distance (y)
	position.y = target.y + v;
}