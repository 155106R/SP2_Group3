#ifndef TOGA__SCENE_H
#define TOGA_SCENE_H

#include <vector>
#include <string>
#include <fstream>

using std::string;
using std::vector;
using std::ifstream;

#include "Scene.h"
#include "FPCamera.h"
#include "Camera_Mouse.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "Camera_Mouse.h"
#include "AABB.h"

class TogaScene : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_LIGHTBALL,
		GEO_TEXTBOX,
		GEO_CUBE,
		GEO_ROCK,
		GEO_PLANE,

		U_TEXT_ENABLED,
		U_TEXT_COLOR,

		SKYBOX_Xposv,
		SKYBOX_Xnega,
		SKYBOX_Yposv,
		SKYBOX_Ynega,
		SKYBOX_Zposv,
		SKYBOX_Znega,
		GROUNDMESH,
		
		NPC_MINERAL_HEAD,
		NPC_MINERAL_BODY,
		MINERAL_SHOP,

		NPC_DRONE_BODY,
		NPC_DRONE_ARMS,
		NPC_DRONE_EYE,
		DRONE_SHOP,

		NPC_UPGRADE_BODY,
		NPC_UPGRADE_ARMR,
		NPC_UPGRADE_ARML,
		NPC_UPGRADE_HEAD,
		UPGRADE_SHOP,

		NPC_TOGAN_HEAD,
		NPC_TOGAN_BODY,
		NPC_TOGAN_ARM,
		NPC_TOGAN_LEG,

		DRONE_PROPELLER,
		DRONE_BODY,


		CAVE,

		GEO_AMOUNTBOX,
		GEO_INVENTORY,
		GEO_TEXT,
		GEO_UI,
		GEO_TEXT_BOX,
		GEO_SHOP,
		SHIP,


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
	TogaScene();
	~TogaScene();

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
	void generateMineralmerchant();
	void generateDronemerchant();
	void generateUpgrademerchant();
	void generateTogan();

	//ANIMATIONS
	void droneAnimation(double dt);
	void mineralAnimation(double dt);
	void upgradeAnimation(double dt);


	void toganwalk(double dt);

	//animation
	void Init_animation_NPC();
	void Update_animation_NPC(double dt);
	// naming
	void Init_Name_NPC();
	void Update_Name_NPC(double dt);

	struct name {
		int state;
		float R_X;
		float tempR;
		Vector3 position;
		string NPCname;
	};
	vector<name> nameS;

	//mini game
	void Init_minigame();
	void Updata_minigame(double dt);
	void Render_minigame();

	int InGame;
	int Pnum; //pointer
	
	//key states and prompts	
	
	
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


	//interactions
	unsigned currentstate;
	void inventory();
	void interact_state();
	void renderinteract();
	void text();
	void resetKey();
	int rendertext;
	float timer = 0;
	float delay = 0;




	//Distance check
	 float checkDistance(Vector3 firstvector,Vector3 secondvector);
	 void interactionUpdate(double dt);





	vector<string> fillWithFontData(string path){

		string line;
		vector<string> data;
		ifstream myfile;
		myfile.open(path);
		if (myfile.is_open())
		{
			std::cout << "File Opened" << std::endl;
			while (!myfile.eof()) {
				while (getline(myfile, line, '\n'))
				{
					data.push_back(line);
				}
			}
			myfile.close();
		}
		else
		{
			std::cout << "Cannot open .csv file" << std::endl;
		}

		return data;

	}
	std::vector<std::string> fontData = fillWithFontData("PixelFontData.csv");
	float getFontOffset(char text){
		std::string whatIWantHolder;
		std::string whatIWantInt;

		float FINAL;

		bool store = false;

		whatIWantHolder = fontData[static_cast<int>(text)];

		for (int i = 0; i < whatIWantHolder.size(); i++)
		{
			if (whatIWantHolder[i] == ','){
				store = true;
			}
			else if (store){
				whatIWantInt += whatIWantHolder[i];
			}
			else{}
		}

		FINAL = std::stoi(whatIWantInt, nullptr, 0);
		//std::cout << FINAL << std::endl;
		if (FINAL == 28){
			FINAL += 14;
		}
		if (FINAL == 21){
			FINAL += 21;
		}
		if (FINAL == 35){
			FINAL += 7;
		}
		if (FINAL == 14){
			FINAL += 26;
		}

		return FINAL;
	}
	//For Light
	bool enableLight;
	AABB infrontOfPlayer;
	Camera_Mouse camera;
	Light light[1];

	Mesh *meshList[NUM_GEOMETRY];



	//animationstuff
	//mm (mineral merchant)
	bool mm_y_max;
	float mm_head_y;

	//dm (droid merchant)
	bool dm_y_max;
	float dm_y;

	bool dm_eye_rotate_max;
	float dm_eye_rotate;


	//um (upgrade merchant)
	bool um_head_rotate_max;
	float um_head_rotate;

	//Togans
	float rotate_armR;
	float rotate_armL;
	float rotate_legL;
	float rotate_legR;

	bool armR_max;
	bool armL_max;
	bool legR_max;
	bool legL_max;
	float targetxz;

	float boxRange;

	//mining drone
	void generate_mdrone();
	void mdrone_animation(double dt);
	float mdrone_spin = 0;
	float mdrone_y = 0;
	bool mdrone_ymax = false;

	float mdrone_mineraltype=1;
	int mdrone_mineralcount=0;
	float mdrone_starttime;
	bool mdrone_added=true;



	//collision checking
	void checkCollision();
	AABB player;
	//Vector3 tempPosition;

	struct Togan
	{
		int state;
		

		Vector3 position;
		Vector3 tempposition;
		
		float rotate_togan;
		float tempR;

	};
	vector<Togan> togan_NPC_Loop;
	Togan togan;
	
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
	int SID;

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
	

};

#endif