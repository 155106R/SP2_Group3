#ifndef PLANET_JELLY_SCENE_H
#define PLANET_JELLY_SCENE_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using std::string;
using std::vector;
using std::ifstream;

#include "Scene.h"
#include "Camera_Mouse.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "AABB.h"


class PlanetJellyScene : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_LIGHTBALL,
		GEO_TEXTBOX,
		GEO_CUBE,

		U_TEXT_ENABLED,
		U_TEXT_COLOR,

		SKYBOX_Xposv,
		SKYBOX_Xnega,
		SKYBOX_Yposv,
		SKYBOX_Ynega,
		SKYBOX_Zposv,
		SKYBOX_Znega,

		GROUND_MESH,

		NPC_1,

		CAVE,	//Cave(mine mineral)

		SHOP_MINERAL,
		SHOP_DRONE,
		SHOP_UPGRADE,
		SHIP,
		DRONE_PROPELLER,
		DRONE_BODY,

		GEO_AMOUNTBOX,
		GEO_INVENTORY,
		GEO_TEXT,
		GEO_UI,
		GEO_TEXT_BOX,
		GEO_SHOP,

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

		U_NUMLIGHTS,
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,

		U_TOTAL,
	};

public:
	PlanetJellyScene();
	~PlanetJellyScene();

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

	//animation
	void Init_animation_NPC();
	void Update_animation_NPC(double dt);
	// naming
	void Init_Name_NPC();
	void Update_Name_NPC(double dt);

	//mini game
	void Init_minigame();
	void Updata_minigame(double dt);
	void Render_minigame();

	int InGame;
	int Pnum; //pointer

	//mining drone
	void generate_mdrone();
	void mdrone_animation(double dt);
	float mdrone_spin = 0;
	float mdrone_y = 0;
	bool mdrone_ymax = false;

	float mdrone_mineraltype = 1;
	int mdrone_mineralcount = 0;
	float mdrone_starttime;
	bool mdrone_added = true;


	// bag/shop
	void Init_Checker();
	void Updata_Checker(double dt);
	void Render_Checker();
	int num;
	int tempnum;
	bool shop;
	bool confirm;
	int SellState;
	int SBamount;
	int SID; // shop ID
	
	//For Light
	bool enableLight;

	//animation
	struct animation {
		int state;
		float S_Y;
		float R_X;
		float tempR;
		Vector3 position;
		Vector3 tempposition;
	};
	vector<animation> jelly_NPC_Loop;

	animation jelly;
	animation jelly_jumping;
	
	//name
	struct name {
		int state;	
		float R_X;
		float tempR;
		Vector3 position;
		string NPCname;
	};
	vector<name> nameS;

	Camera_Mouse camera;
	Light light[1];

	Mesh *meshList[NUM_GEOMETRY];



	//key states
	bool button_prompt;
	bool e_state;
	bool i_state;



	//ui states
	enum state{

		FREEMOVE,
		CONVERSE,
		TRADE,
		INVENTORY,
		CAVEGAME,
		NUM_state
	};


	enum shopstates
	{
		BROWSING,
		SELLING,
		BUYING,
		DROPPING,
		UPGRADING,
		DRONES,

		NUM_shostates

	};
	//interactions
	unsigned currentstate=0;
	void inventory();
	void interact_state();
	void renderinteract();
	void text();
	void resetKey();
	int rendertext;
	float timer = 0;
	float delay = 0;
	void interactionUpdate(double dt);

	//collision checking
	void checkCollision();
	AABB player;
};

#endif