#ifndef OPEN_GALAXY_SCENE_H
#define OPEN_GALAXY_SCENE_H

#include <vector>
#include <string>

using std::string;
using std::vector;
using std::ifstream;

#include "Scene.h"
#include "FPCamera.h"
//#include "Camera_Mouse.h"
#include "OpenGalaxyCamera.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include <map>

class ASTEROID{
public:
	string materialName;
	int count;
};

class OpenGalaxyScene : public Scene
{


	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_LIGHTCUBE,
		GEO_TEXTBOX,

		U_TEXT_ENABLED,
		U_TEXT_COLOR,

		SKYBOX_Xposv,
		SKYBOX_Xnega,
		SKYBOX_Yposv,
		SKYBOX_Ynega,
		SKYBOX_Zposv,
		SKYBOX_Znega,

		PLANET_A,
		PLANET_B,
		PLANET_C,

		PLANET_SUN,

		ASTEROIDS,

		PROXY_SPACESHIP,

		SPACESHIP,

		GEO_TEXT,
		GEO_HUD,

		NUM_GEOMETRY,
	};
	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,

		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,

		U_LIGHTENABLED,

		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHT0_TYPE,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,

		U_LIGHT1_POSITION,
		U_LIGHT1_COLOR,
		U_LIGHT1_POWER,
		U_LIGHT1_KC,
		U_LIGHT1_KL,
		U_LIGHT1_KQ,
		U_LIGHT1_TYPE,
		U_LIGHT1_SPOTDIRECTION,
		U_LIGHT1_COSCUTOFF,
		U_LIGHT1_COSINNER,
		U_LIGHT1_EXPONENT,

		U_NUMLIGHTS,
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,

		U_TOTAL,
	};

public:
	OpenGalaxyScene();
	~OpenGalaxyScene();

	float LSPEED;

	MS modelStack, viewStack, projectionStack;

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

private:
	unsigned m_vertexArrayID;
	unsigned m_vertexBuffer[NUM_GEOMETRY];
	unsigned m_colorBuffer[NUM_GEOMETRY];
	unsigned m_indexBuffer[NUM_GEOMETRY];
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];
	virtual void RenderMesh(Mesh *mesh, bool enableLight);

	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);

	void generateSkybox();
	void updateShipMovement(float dt);


	
	//For Planet Interactions
	bool land;
	string nameOfPlanet;

	//Ship movement stuff
	Vector3 *noseOfShip;
	Vector3 *middleOfShip;
	float shipAxisX, shipAxisY, shipAxisZ; 
	float rotateShipZ;
	bool isTransltingY;
	float rotateShip;
	float translateShip;
	float accelerateShip;

	//Asteroid random stuff
	float randScaleX[1000];
	float randScaleY[1000];
	float randScaleZ[1000];
	float randTranslateX[1000];
	float randTranslateY[1000];
	float randTranslateZ[1000];
	float asteroidTranslateX[1000];
	float asteroidTranslateY[1000];
	float asteroidTranslateZ[1000];

	//For Light
	bool enableLight;

	//Generates HUD
	void drawHUD();

	//Camera
	//Camera_Mouse camera;
	//FPCamera camera;
	OpenGalaxyCamera camera;
	Light light[2];

	Mesh *meshList[NUM_GEOMETRY];
};

#endif

//Author: Randall (155106R)
//Updated 22/2/2016 - Randall