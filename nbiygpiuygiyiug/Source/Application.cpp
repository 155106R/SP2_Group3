#include "Application.h"

//Include GLEW
#include <GL/glew.h>

//Include GLFW
#include <GLFW/glfw3.h>

//Include the standard C++ headers
#include <stdio.h>
#include <stdlib.h>

//Include planet scenes
#include "PlanetJellyScene.h"
#include "OpenGalaxyScene.h"
#include "TogaScene.h"

GLFWwindow* m_window;
const unsigned char FPS = 60; // FPS of this game
const unsigned int frameTime = 1000 / FPS; // time for each frame
float delay = 0;


//Define an error callback
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

//Define the key input callback
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

bool Application::IsKeyPressed(unsigned short key)
{
	return ((GetAsyncKeyState(key) & 0x8001) != 0);
}

Application::Application()
{
}

Application::~Application()
{
}

void resize_callback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h); //update opengl the new indow size
}

void Application::Init()
{
	//Set the error callback
	glfwSetErrorCallback(error_callback);

	//Initialize GLFW
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	//Set the GLFW window creation hints - these are optional
	glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 


	//Create a window and create its OpenGL context
	m_window = glfwCreateWindow(1920, 1080, "Computer Graphics", NULL, NULL);	//For fixed screen size
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	glfwSetWindowSizeCallback(m_window, resize_callback);

	//If the window couldn't be created
	if (!m_window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(m_window);

	//Sets the key callback
	//glfwSetKeyCallback(m_window, key_callback);

	glewExperimental = true; // Needed for core profile
	//Initialize GLEW
	GLenum err = glewInit();

	//If GLEW hasn't initialized
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		//return -1;
	}
	SharedData::GetInstance()->SD_lastLocationVisited = PLANET_TOGA; //Set by default

}

void Application::Run()
{
	//Gets new seed
	Math::InitRNG();

	Scene *currentScene;

	Scene *OpenGalaxy = new OpenGalaxyScene();
	Scene *TogaPlanet = new TogaScene();
	Scene *JellyPlanet = new PlanetJellyScene();
	TogaPlanet->Init();
	JellyPlanet->Init();
	OpenGalaxy->Init();

	currentScene = TogaPlanet;

	//Main Loop
	m_timer.startTimer();    // Start timer to calculate how long it takes to render this frame
	while (!glfwWindowShouldClose(m_window) && !IsKeyPressed(VK_END))
	{
		//references from OpenGalaxyScene and SharedData
		switch (SharedData::GetInstance()->SD_location){
		case(OPEN_GALAXY) :
				currentScene = OpenGalaxy;
				break;
			case(PLANET_TOGA):
				currentScene = TogaPlanet;
				break;
			case(PLANET_JELLY):
				currentScene = JellyPlanet;
				break;
		}

		//Help Button
		if (IsKeyPressed(VK_F1) && SharedData::GetInstance()-> SD_timecounter > delay)
		{
			delay = SharedData::GetInstance()->SD_timecounter + 0.5;
			if (SharedData::GetInstance()->helpMenu == false){
				SharedData::GetInstance()->helpMenu = true;
			}
			else{
				SharedData::GetInstance()->helpMenu = false;
			}
			
		}
		
		if ((SharedData::GetInstance()->helpMenu == true && Application::IsKeyPressed(VK_TAB)) && SharedData::GetInstance()->SD_timecounter > delay)
		{
			delay = SharedData::GetInstance()->SD_timecounter + 0.5;
			SharedData::GetInstance()->currentHelpPage++;
		}
		if (SharedData::GetInstance()->currentHelpPage == HELP_PAGE_MAX){
			SharedData::GetInstance()->currentHelpPage = SHIP;
		}

		cout << SharedData::GetInstance()->currentHelpPage << endl;

		//Update and render current scene
			currentScene->Update(m_timer.getElapsedTime());
			currentScene->Render();

		/*if (SharedData::GetInstance()->SD_bitcoins > 500000){

		}

		if (IsKeyPressed(VK_F12)){
			currentScene->Exit();
			currentScene->Init();
		}*/

		//Swap buffers
		glfwSwapBuffers(m_window);
		
		//Get and organize events, like keyboard and mouse input, window resizing, etc...
		glfwPollEvents();
		m_timer.waitUntil(frameTime);       // Frame rate limiter. Limits each frame to a specified time in ms.   

	} //Check if the END key had been pressed or if the window had been closed
	currentScene->Exit();
	delete currentScene;
}

void Application::Exit()
{
	//Close OpenGL window and terminate GLFW
	glfwDestroyWindow(m_window);
	//Finalize and clean up GLFW
	glfwTerminate();
}

//sopijaijoifjh