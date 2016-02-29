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
	_jump = false;

}

void Camera_drill::Update(double dt)
{

	view *= 20;
	static const float CAMERA_SPEED = 5.f;
	int screenSizeX = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
	int screenSizeY = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
	double screenMidX, screenMidY;
	float maxCameraRotationX = 50.0f;
	float rotationSpeed = 1.0f / (CAMERA_SPEED * dt);
	//float walkSpeed = 0.1f;
	walkSpeed = 0.8f;
	glfwGetWindowSize(m_window, &screenSizeX, &screenSizeY);

	colliding = false;

	screenMidX = screenSizeX / 2;
	screenMidY = screenSizeY / 2;

	nextPosition.y = 0;
	tempPosition = position;
	//GLFW Method (Laggier)
	/*MousePosition mousePosition;
	glfwGetCursorPos(m_window, &(mousePosition.x), &(mousePosition.y));
	glfwSetCursorPos(m_window, screenMidX, screenMidY);*/

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
	float newpitch = cameraRotationX - (mousePosition.y - screenMidY) / rotationSpeed;

	if (newpitch < 50 && newpitch > -50)
	{
		cameraRotationX -= (mousePosition.y - screenMidY) / rotationSpeed;
	}

	if (cameraRotationX > maxCameraRotationX) {

		cameraRotationX = maxCameraRotationX;

	}
	else if (cameraRotationX < -maxCameraRotationX) {

		cameraRotationX = -maxCameraRotationX;

	}

	if (cameraRotationY > 360 || cameraRotationY < -360) {

		cameraRotationY = 0;

	}

	if (Application::IsKeyPressed(MK_CONTROL) || Application::IsKeyPressed(VK_CONTROL))
	{
		if (Counter <= 1)
		{
			Counter++;
			position.y -= 2;
			target.y -= 2;
		}
		walkSpeed = walkSpeed / 2;
	}
	else if (Counter > 0)
	{
		position.y += 2;
		target.y += 2;
		Counter--;
	}


	if (Application::IsKeyPressed('W'))
	{
		nextPosition.x = position.x + sin(DegreeToRadian(cameraRotationY + 180))  *walkSpeed;
		nextPosition.z = position.z + cos(DegreeToRadian(cameraRotationY + 180)) * walkSpeed;
		move = FOWARD;

		//check for colllision now


		//check before runing v


	}


	if (Application::IsKeyPressed('S'))
	{
		nextPosition.x = position.x + sin(DegreeToRadian(cameraRotationY)) * walkSpeed;
		nextPosition.z = position.z + cos(DegreeToRadian(cameraRotationY)) * walkSpeed;
		move = BACK;
		//check for colllision now


		//check before runing v
		/*	if (!colliding){
		position.x += sin(DegreeToRadian(cameraRotationY)) * walkSpeed;
		position.z += cos(DegreeToRadian(cameraRotationY)) * walkSpeed;
		}*/
	}

	if (Application::IsKeyPressed('A'))
	{
		nextPosition.x = position.x + sin(DegreeToRadian(cameraRotationY + 270)) * walkSpeed;
		nextPosition.z = position.z + (DegreeToRadian(cameraRotationY + 270)) * walkSpeed;
		move = LEFT;
		/*	if (!colliding){
		position.x += sin(DegreeToRadian(cameraRotationY + 270)) * walkSpeed;
		position.z += cos(DegreeToRadian(cameraRotationY + 270)) * walkSpeed;
		}*/
	}

	if (Application::IsKeyPressed('D'))
	{
		nextPosition.x = position.x + sin(DegreeToRadian(cameraRotationY + 90)) * walkSpeed;
		nextPosition.z = position.z + (DegreeToRadian(cameraRotationY + 90)) * walkSpeed;

		move = RIGHT;
		/*	if (!colliding){
		position.x += sin(DegreeToRadian(cameraRotationY + 90)) * walkSpeed;
		position.z += cos(DegreeToRadian(cameraRotationY + 90)) * walkSpeed;
		}*/
	}



	if (Application::IsKeyPressed(VK_SPACE))
	{
		if (position.y < 15.2)
		{
			_jump = true;
		}
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

void Camera_drill::boundCheck(float minX, float minZ, float maxX, float maxZ)
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

void Camera_drill::boundCheck2(float minX, float minZ, float maxX, float maxZ)
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


void Camera_drill::movement()
{
	position = tempPosition;
	if ((!colliding && move == FOWARD) && (Application::IsKeyPressed('W')))
	{
		
		position.x += sin(DegreeToRadian(cameraRotationY + 180)) * walkSpeed;
		position.z += cos(DegreeToRadian(cameraRotationY + 180)) * walkSpeed;
		position.y += sin(DegreeToRadian(cameraRotationX)) * walkSpeed;
	}

	if ((!colliding && move == BACK) && (Application::IsKeyPressed('S')))
	{
		position.x += sin(DegreeToRadian(cameraRotationY)) * walkSpeed;
		position.z += cos(DegreeToRadian(cameraRotationY)) * walkSpeed;
		position.y -= sin(DegreeToRadian(cameraRotationX)) * walkSpeed;
	}
	if ((!colliding && move == LEFT) && (Application::IsKeyPressed('A')))
	{
		position.x += sin(DegreeToRadian(cameraRotationY + 270)) * walkSpeed;
		position.z += cos(DegreeToRadian(cameraRotationY + 270)) * walkSpeed;
	}
	if ((!colliding && move == RIGHT) && (Application::IsKeyPressed('D')))
	{
		position.x += sin(DegreeToRadian(cameraRotationY + 90)) * walkSpeed;
		position.z += cos(DegreeToRadian(cameraRotationY + 90)) * walkSpeed;
	}

	target = Vector3(-sin(DegreeToRadian(cameraRotationY)) * cos(DegreeToRadian(cameraRotationX)) + this->position.x,
		sin(DegreeToRadian(cameraRotationX)) + this->position.y,
		-cos(DegreeToRadian(cameraRotationY)) * cos(DegreeToRadian(cameraRotationX)) + this->position.z);
	/*
	view = (target - position).Normalized();
	right = view.Cross(defaultUp);
	up = right.Cross(view);*/

	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	up = right.Cross(view).Normalized();

}