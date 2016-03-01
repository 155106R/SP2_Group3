#include "Camera_drill.h"
#include "Application.h"
#include "Mtx44.h"
#include "GLFW\glfw3.h"


extern GLFWwindow* m_window;

Camera_drill::Camera_drill()
{
	cameraRotationX = 0;
	cameraRotationY = 0;
	Counter = 0;
}

Camera_drill::~Camera_drill()
{
}

void Camera_drill::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	colliding = false;
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();
	walkSpeed = 0.5f;
	maxjump = 18;

}

void Camera_drill::Update(double dt)
{

	view *= 20;
	static const float CAMERA_SPEED = 5.f;
	int screenSizeX = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
	int screenSizeY = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
	double screenMidX, screenMidY;
	float maxCameraRotationX = 89.99f;
	float rotationSpeed = 1.0f / (CAMERA_SPEED * dt);
	//float walkSpeed = 0.1f;
	walkSpeed = 0.8f;
	glfwGetWindowSize(m_window, &screenSizeX, &screenSizeY);

	colliding = false;

	screenMidX = screenSizeX / 2;
	screenMidY = screenSizeY / 2;

	//Windows Method
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

	cameraRotationY -= (mousePosition.x - screenMidX) / rotationSpeed;
	cameraRotationX -= (mousePosition.y - screenMidY) / rotationSpeed;

	if (cameraRotationX > maxCameraRotationX) {

		cameraRotationX = maxCameraRotationX;

	}
	else if (cameraRotationX < -maxCameraRotationX) {

		cameraRotationX = -maxCameraRotationX;

	}

	if (cameraRotationY > 360 || cameraRotationY < -360) {

		cameraRotationY = 0;

	}


	if (Application::IsKeyPressed('W'))
	{
		if (cameraRotationX < 89.87 && cameraRotationX > -89.87)
		{
			position.x += sin(DegreeToRadian(cameraRotationY + 180)) * walkSpeed;
			position.z += cos(DegreeToRadian(cameraRotationY + 180)) * walkSpeed;
		}

		position.y += sin(DegreeToRadian(cameraRotationX)) * walkSpeed;

	}

	if (Application::IsKeyPressed('S'))
	{
		if (cameraRotationX > -89.87 && cameraRotationX < 89.87)
		{
			position.x += sin(DegreeToRadian(cameraRotationY)) * walkSpeed;
			position.z += cos(DegreeToRadian(cameraRotationY)) * walkSpeed;
		}
		position.y -= sin(DegreeToRadian(cameraRotationX)) * walkSpeed;
	}


	if (Application::IsKeyPressed('A'))
	{
		position.x += sin(DegreeToRadian(cameraRotationY + 270)) * walkSpeed;
		position.z += cos(DegreeToRadian(cameraRotationY + 270)) * walkSpeed;
	}
	if (Application::IsKeyPressed('D'))
	{
		position.x += sin(DegreeToRadian(cameraRotationY + 90)) * walkSpeed;
		position.z += cos(DegreeToRadian(cameraRotationY + 90)) * walkSpeed;
	}

	target = Vector3(-sin(DegreeToRadian(cameraRotationY)) * cos(DegreeToRadian(cameraRotationX)) + this->position.x,
		sin(DegreeToRadian(cameraRotationX)) + this->position.y,
		-cos(DegreeToRadian(cameraRotationY)) * cos(DegreeToRadian(cameraRotationX)) + this->position.z);
	view = (target - position).Normalized();
	right = view.Cross(defaultUp);
	up = right.Cross(view);

	frontTarget = position + view*(float)(2.5);
}

void Camera_drill::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}

bool Camera_drill::checkCamera(int minX, int minZ, int maxX, int maxZ)
{
	if (position.x > minX && position.x < maxX && position.z >minZ && position.z < maxZ)
	{
		return true;
	}
	return false;
}




