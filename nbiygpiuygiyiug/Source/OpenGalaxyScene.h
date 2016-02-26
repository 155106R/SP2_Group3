#ifndef OPEN_GALAXY_SCENE_H
#define OPEN_GALAXY_SCENE_H

#include <vector>
#include <string>

using std::string;
using std::vector;
using std::ifstream;

#include "Scene.h"
#include "FPCamera.h"
#include "Camera_Mouse.h"
#include "OpenGalaxyCamera.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "AABB.h"
#include <map>

class Asteroid{
public:
	int material;	//The kind of material this asteroid has (currently set to int)
	int count;		//How much of the material it has

	Vector3 position;//Where the asteroid is
	Vector3 velocity;//How much the asteroid moves in each update
	
	//Visual appearence of the asteroid
	float length;	
	float height;
	float width; 

	AABB hitbox;	//Generate a hitbox for the Asteroid for collision checks

	Asteroid(){
		material = 1;//Math::RandIntMinMax(1, 12);
		count = Math::RandIntMinMax(1, 10);

		position = Vector3(Math::RandFloatMinMax(-200, 200), Math::RandFloatMinMax(-200, 200), Math::RandFloatMinMax(-200, 200));
		velocity = Vector3(Math::RandFloatMinMax(-0.05f, 0.05f), Math::RandFloatMinMax(-0.05f, 0.05f), Math::RandFloatMinMax(-0.05f, 0.05f));

		length = Math::RandFloatMinMax(1, 5);
		height = Math::RandFloatMinMax(1, 5);
		width = Math::RandFloatMinMax(1, 5);

		hitbox = AABB::generateAABB(
			position,		//Pass in where the asteroid is to set as the origin of hitbox
			length * 2,		//Pass in length
			height * 2,		//Pass in height
			width * 2,		//Pass in width
			velocity		//Pass in Velocity
			);
	};

	~Asteroid(){

	};
};

class OpenGalaxyScene : public Scene
{
	struct DrillAssets{
		Vector3 position;
		Vector3 facing;
		float rotate;
		float moveSpeed = 5;
		std::map<unsigned, unsigned> storage;	//Drill's storage of materials in a map <unsigned materialID, unsigned count/stack>

		AABB drillHead;

		OpenGalaxyCamera camera;
	}Drill;

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

		SPACESHIP,
		SPACESHIP_INTERIOR,
		SPACESHIP_DRILL_BODY,
		SPACESHIP_DRILL_HEAD,

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

	enum GAMESTATE{
		PILOTING,
		IN_SHIP,
		DRILLING,

		MAX_GAMESTATE
	};

public:
	OpenGalaxyScene();
	~OpenGalaxyScene();

	MS modelStack, viewStack, projectionStack;

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	//Ship movement stuff
	Vector3 *noseOfShip;
	Vector3 *middleOfShip;

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
	
	//For Planet Interactions
	bool e_state;
	float timer = 0;
	float delay;
	void resetKey();
	bool land;
	string nameOfPlanet;

	//Hitboxes
	AABB spaceshipHitbox;
	AABB planetA_Hitbox;
	AABB planetB_Hitbox;
	AABB planetC_Hitbox;

	AABB shipInterior;

	unsigned CURRENT_STATE;	//Play state

	float shipAxisX, shipAxisY, shipAxisZ; 
	float rotateShipZ;
	bool isTransltingY;
	float rotateShip;
	float translateShip;
	float accelerateShip;

	void updateDrillMovement(double dt);
	void updateShipMovement(double dt);

	Vector3 tempPosition;

	//For Light
	bool enableLight;

	//Generates HUD
	void drawHUD();

	//Cameras
	Camera_Mouse inShipCamera;
	OpenGalaxyCamera shipCamera;
	Light light[2];

	Mesh *meshList[NUM_GEOMETRY];
};

#endif

//Author: Randall (155106R)
//Updated 22/2/2016 - Randall
//Updated 24/2/2016 - Randall
