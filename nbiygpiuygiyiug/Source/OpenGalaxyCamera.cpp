#include "OpenGalaxyCamera.h"
#include "Application.h"
#include "Mtx44.h"
#include "MyMath.h"
#include "math.h"
#include "OpenGalaxyScene.h"
#include "GLFW\glfw3.h"

extern GLFWwindow* m_window;

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
	this->up = defaultUp = right.Cross(view).Normalized();
}
double oldMouseX = 0;
double oldMouseY = 0;
void OpenGalaxyCamera::Update(double dt)
{
	//std::cout << position << std::endl;


	int screenSizeX = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
	int screenSizeY = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
	double screenMidX, screenMidY;
	float maxCameraRotationX = 89.99f;
	float rotationSpeed = 4.0f;

	static const float CAMERA_SPEED = 100.f;
	glfwGetWindowSize(m_window, &screenSizeX, &screenSizeY);

	screenMidX = screenSizeX / 2;
	screenMidY = screenSizeY / 2;

	POINT mousePosition;
	GetCursorPos(&mousePosition);
	SetCursorPos(screenMidX, screenMidY);


	if (mousePosition.x > screenSizeX) {

		mousePosition.x = screenSizeX;

	}
	else if (mousePosition.x < 0) {

		mousePosition.x = 0;

	}

	if (mousePosition.y > screenSizeY) {

		mousePosition.y = screenSizeY;

	}
	else if (mousePosition.y < 0) {

		mousePosition.y = 0;

	}


	yaw -= (mousePosition.x - screenMidX) / rotationSpeed;

	float newpitch = pitch+(mousePosition.y - screenMidY) / rotationSpeed;

	if (newpitch < 50 && newpitch > -50)
	{
			pitch += (mousePosition.y - screenMidY) / rotationSpeed;
	}



	if (Application::IsKeyPressed(VK_LEFT))
	{
		yaw += (float)(-CAMERA_SPEED * dt);
	
	}
	if (Application::IsKeyPressed(VK_RIGHT))
	{
		yaw += (float)(CAMERA_SPEED * dt);

	}
	if (Application::IsKeyPressed(VK_UP))
	{
		
		if (pitch < 50)
		{
			pitch += (float)(CAMERA_SPEED * dt);
		}

	}
	if (Application::IsKeyPressed(VK_DOWN))
	{
		if (pitch > -50)
		{
			pitch += (float)(-CAMERA_SPEED * dt);
		}

	}

	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	up = right.Cross(view).Normalized();

	float h = getHordistance();
	float v = getVertdistance();
	getcameraCoords(h,v);
	


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


void OpenGalaxyCamera::boundCheck(float minX, float minZ, float maxX, float maxZ)
{

	if ((position.x > minX - 5) && (position.x < maxX + 5) && (position.z > minZ - 4) && (position.z<minZ))
	{
		position.z = minZ;
	}
	if ((position.x > minX - 5) && (position.x < maxX + 5) && (position.z<maxZ + 4) && (position.z>maxZ))
	{
		position.z = maxZ;
	}
	if ((position.z >minZ - 5) && (position.z < maxZ + 5) && (position.x>minX - 4) && (position.x<minX))
	{
		position.x = minX;
	}
	if ((position.z >minZ - 5) && (position.z < maxZ + 5) && (position.x < maxX + 4) && (position.x>maxX))
	{
		position.x = maxX;
	}
}

void OpenGalaxyCamera::boundCheck2(float minX, float minZ, float maxX, float maxZ)
{

	if ((position.x > minX) && (position.x < maxX) && (position.z < minZ + 4) && (position.z>minZ - 4))
	{
		position.z = minZ - 4;
	}
	if ((position.x > minX) && (position.x < maxX) && (position.z>maxZ - 4) && (position.z<maxZ + 4))
	{
		position.z = maxZ + 4;
	}
	if ((position.z >minZ) && (position.z < maxZ) && (position.x<minX + 4) && (position.x>minX - 4))
	{
		position.x = minX - 4;
	}
	if ((position.z >minZ) && (position.z < maxZ) && (position.x > maxX - 4) && (position.x<maxX + 4))
	{
		position.x = maxX + 4;
	}
}
//Updated 22/2/2016 - Randall