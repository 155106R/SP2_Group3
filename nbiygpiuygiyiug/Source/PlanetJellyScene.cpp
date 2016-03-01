#include "PlanetJellyScene.h"
#include "GL\glew.h"
#include "MatrixStack.h"
#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"
#include "Utility.h"
#include "LoadTGA.h"
#include "Mtx44.h"

vector<AABB> hitbox;

PlanetJellyScene::PlanetJellyScene()
{
}

PlanetJellyScene::~PlanetJellyScene()
{
}

void PlanetJellyScene::Init()
{
	//interactions
	e_state = 0;
	i_state = 0;

	currentstate = FREEMOVE;

	rendertext = 0;

	hitbox.push_back(AABB::generateAABB(Vector3(100, 0, -100), 30, 30, 30, 0));// Mineral shop [0]

	hitbox.push_back(AABB::generateAABB(Vector3(100, 0, 100), 50, 30, 50, 0));// Ugrade shop [1]

	hitbox.push_back(AABB::generateAABB(Vector3(-135, 0, 0), 45, 60, 70, 0));// drone shop [2]

	hitbox.push_back(AABB::generateAABB(Vector3(180, -7, 0), 90, 15, 75, 0));// cave [3]

	hitbox.push_back(AABB::generateAABB(Vector3(-55, 0, -270), 90, 30, 75, 0));// ship [4]

	hitbox.push_back(AABB::generateAABB(Vector3(140, 8, 22), 5, 15, 5, 0));// mdrone [5]


	//Definations
	LSPEED = 10.0f;

	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);

	//Init VBO here
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);


	//Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Enable depth test
	glEnable(GL_DEPTH_TEST);

	//Camera 
	camera.Init(Vector3(0, 0, 1), Vector3(0, 0, 0), Vector3(0, 1, 0)); //if using Camera_Mouse
	camera.Reset();

	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	//Load vertex and fragment shaders
	m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Texture.fragmentshader");
	m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");


	// Get a handle for our "colorTexture" uniform
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");

	// Get a handle for our "textColor" uniform
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");

	//Light
	light[0].type = Light::LIGHT_SPOT;
	light[0].position.Set(0, 20, 0);
	light[0].color.Set(1, 1, 1);
	light[0].power = 1;
	light[0].kC = 1.0f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;
	light[0].cosCutoff = cos(Math::DegreeToRadian(45));
	light[0].cosInner = cos(Math::DegreeToRadian(30));
	light[0].exponenet = 3.0f;
	light[0].spotDirection.Set(0.f, 1.f, 0.f);

	//Get a handle for our "MVP" uniform
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutOff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponenet");
	glUseProgram(m_programID);

	//Uniform parameter
	glUniform1i(m_parameters[U_NUMLIGHTS], 1);

	glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], light[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], light[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponenet);

	//light toggle shit
	enableLight = true;

	//light toggle mode
	if (light[0].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[0].position.x, light[0].position.y, light[0].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[0].type == Light::LIGHT_SPOT)
	{

		Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[0].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
	}

	//Render wishlist
	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("Axes", 1000, 1000, 1000);

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//pixelFont.tga");

	meshList[GEO_LIGHTBALL] = MeshBuilder::GenerateSphere("LightSource", Color(1, 1, 1), 18, 36);

	//Skybox - Planet JELLY
	meshList[SKYBOX_Xposv] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1));
	meshList[SKYBOX_Xposv]->textureID = LoadTGA("Image//Skybox//jelly_planet//jelly-Z.tga");
	meshList[SKYBOX_Xnega] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1));
	meshList[SKYBOX_Xnega]->textureID = LoadTGA("Image//Skybox//jelly_planet//jelly+Z.tga");
	meshList[SKYBOX_Yposv] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1));
	meshList[SKYBOX_Yposv]->textureID = LoadTGA("Image//Skybox//jelly_planet//jelly+Y.tga");
	meshList[SKYBOX_Ynega] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1));
	meshList[SKYBOX_Ynega]->textureID = LoadTGA("Image//Skybox//jelly_planet//jelly-Y.tga");
	meshList[SKYBOX_Zposv] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1));
	meshList[SKYBOX_Zposv]->textureID = LoadTGA("Image//Skybox//jelly_planet//jelly+X.tga");
	meshList[SKYBOX_Znega] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1));
	meshList[SKYBOX_Znega]->textureID = LoadTGA("Image//Skybox//jelly_planet//jelly-X.tga");

	//Render ground mesh
	meshList[GROUND_MESH] = MeshBuilder::GenerateOBJ("ground", "OBJ//jelly_house.obj");
	meshList[GROUND_MESH]->textureID = LoadTGA("Image//jelly_house_texture.tga");

	//render Cave(mine mineral)
	meshList[CAVE] = MeshBuilder::GenerateOBJ("cave", "OBJ//jelly_cave.obj");
	meshList[CAVE]->textureID = LoadTGA("Image//jelly_cave_texture.tga");

	//Render shops
	meshList[SHOP_MINERAL] = MeshBuilder::GenerateOBJ("mineral shop", "OBJ//jelly_shop.obj");
	meshList[SHOP_MINERAL]->textureID = LoadTGA("Image//jelly_shop_texture.tga");
	meshList[SHOP_DRONE] = MeshBuilder::GenerateOBJ("drone shop", "OBJ//drone_shopA.obj");
	meshList[SHOP_DRONE]->textureID = LoadTGA("Image//droneshop_textureA.tga");
	meshList[SHOP_UPGRADE] = MeshBuilder::GenerateOBJ("upgrade shop", "OBJ//upgradeshopA.obj");
	meshList[SHOP_UPGRADE]->textureID = LoadTGA("Image//upgradeshop_textureA.tga");
	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("hitbox", Color(0,0,0));
	meshList[GEO_UI] = MeshBuilder::GenerateQuad("UI", Color(0, 0, 0));
	meshList[GEO_UI]->textureID = LoadTGA("Image//planet_UI.tga");


	meshList[DRONE_BODY] = MeshBuilder::GenerateOBJ("player ship", "OBJ//mining_drone_body.obj");
	meshList[DRONE_BODY]->textureID = LoadTGA("Image//miningdrone_texture.tga");

	meshList[DRONE_PROPELLER] = MeshBuilder::GenerateOBJ("player ship", "OBJ//mining_drone_propeller.obj");
	meshList[DRONE_PROPELLER]->textureID = LoadTGA("Image//miningdrone_texture.tga");

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//pixelFont.tga");

	meshList[GEO_TEXT_BOX] = MeshBuilder::GenerateQuad("textbox", Color(0, 0, 0));
	meshList[GEO_TEXT_BOX]->textureID = LoadTGA("Image//textbox.tga");

	meshList[GEO_SHOP] = MeshBuilder::GenerateQuad("shop screen", Color(0, 0, 0));
	meshList[GEO_SHOP]->textureID = LoadTGA("Image//shop_screen.tga");

	meshList[GEO_AMOUNTBOX] = MeshBuilder::GenerateQuad("amt box", Color(0, 0, 0));
	meshList[GEO_AMOUNTBOX]->textureID = LoadTGA("Image//amount_screen.tga");

	meshList[SHIP] = MeshBuilder::GenerateOBJ("player ship", "OBJ//ship.obj");
	meshList[SHIP]->textureID = LoadTGA("Image//ship_texture1.tga");

	meshList[GEO_INVENTORY] = MeshBuilder::GenerateQuad("inventory", Color(0, 0, 0));
	meshList[GEO_INVENTORY]->textureID = LoadTGA("Image//Inventory_screen.tga");
	
	//Render NCP
	Init_animation_NPC();
	//render name of npc
	Init_Name_NPC();

	Math::InitRNG();
	Init_Checker();
	Init_minigame();
}

void PlanetJellyScene::Init_Name_NPC()
{

	for (int i = 0; i < 2; i++)
	{
		name newname;
		newname.position = Vector3(0, 0, 0);
		newname.R_X = 0;
		newname.state = 0;
		newname.tempR = 0;
		newname.NPCname = "Jelly NPC";
		nameS.push_back(newname);
	}

	name newname0;
	newname0.position = Vector3(-115, 0, -15);
	newname0.R_X = 0;
	newname0.state = 0;
	newname0.tempR = 0;
	newname0.NPCname = "DRONE SHOP";
	nameS.push_back(newname0);

	name newname2;
	newname2.position = Vector3(100, 0, 100);
	newname2.R_X = 0;
	newname2.state = 0;
	newname2.tempR = 0;
	newname2.NPCname = "UPGRADE SHOP";
	nameS.push_back(newname2);

	name newname1;
	newname1.position = Vector3(110, 0, -90);
	newname1.R_X = 0;
	newname1.state = 0;
	newname1.tempR = 0;
	newname1.NPCname = "MINERAL SHOP";
	nameS.push_back(newname1);
}
void PlanetJellyScene::Update_Name_NPC(double dt)
{
	for (int i = 0; i < nameS.size(); i++)
	{
		if (i < (nameS.size() - 3)) // only for the moving npc
		{
			nameS[i].position.x = jelly_NPC_Loop[i].position.x;
			nameS[i].position.z = jelly_NPC_Loop[i].position.z;
		}
		nameS[i].position.y = jelly_jumping.position.y + jelly.position.y;
		
		nameS[i].tempR = Math::RadianToDegree(atan((camera.position.x - nameS[i].position.x) / (camera.position.z - nameS[i].position.z)));
		if (((camera.position.x - nameS[i].position.x) < 0) && ((camera.position.z - nameS[i].position.z) > 0)) nameS[i].tempR -= nameS[i].R_X; /*(z,-x) C*/
		else if (((camera.position.x - nameS[i].position.x) < 0) && ((camera.position.z - nameS[i].position.z) < 0)) nameS[i].tempR -= (nameS[i].R_X + 180); /*(-z,-x) T*/
		else if (((camera.position.x - nameS[i].position.x) > 0) && ((camera.position.z - nameS[i].position.z) > 0)) nameS[i].tempR -= nameS[i].R_X; /*(z,x) A*/
		else if (((camera.position.x - nameS[i].position.x) > 0) && ((camera.position.z - nameS[i].position.z) < 0)) nameS[i].tempR -= (nameS[i].R_X - 180); /*(-z,x) S*/

		if (nameS[i].tempR < -180)
		{
			nameS[i].tempR = 360 + nameS[i].tempR;
		}
		if (nameS[i].tempR > 180)
		{
			nameS[i].tempR = 0-360 + nameS[i].tempR;
		}
		if (nameS[i].tempR > 0) // trun left
		{
			nameS[i].tempR -= 360 * dt;
			nameS[i].R_X += 360 * dt;
		}
		if (nameS[i].tempR < 0) // trun right
		{
			nameS[i].tempR += 360 * dt;
			nameS[i].R_X -= 360 * dt;
		}
	}
}

void PlanetJellyScene::Init_animation_NPC()
{
	//Render NCP
	meshList[NPC_1] = MeshBuilder::GenerateOBJ("npc", "OBJ//jelly.obj");
	meshList[NPC_1]->textureID = LoadTGA("Image//jelly_texture.tga");

	// jelly animation
	jelly.position = Vector3(Math::RandFloatMinMax(-50, 50), 0, Math::RandFloatMinMax(-50, 50));
	jelly.S_Y = 0;
	jelly.state = 0;

	// jelly_jumping animation
	jelly_jumping.position = Vector3(0, 0, 0);
	jelly_jumping.state = 0;


	//jelly npc animation
	for (int i = 0; i <= 1; i++)
	{
		animation newjelly;
		newjelly.position = Vector3(0, 0, 0);
		newjelly.tempposition = Vector3(0, 0, 0);
		newjelly.R_X = 0;
		newjelly.state = 0;
		newjelly.tempR = 0;
		jelly_NPC_Loop.push_back(newjelly);
	}
}
void PlanetJellyScene::Update_animation_NPC(double dt)
{
	// jelly animation && jelly_jumping animation
	if (jelly.state == 0 && jelly_jumping.state == 0)
	{
		if (jelly.S_Y >= -1)
		{
			jelly.S_Y -= 4 * dt;
			jelly.position.y -= 4 * dt;
		}
		else
		{
			jelly.state = 1;
			jelly_jumping.state = 1;
		}
	}
	else if (jelly.state == 1 && jelly_jumping.state == 1) //jumping up
	{
		if (jelly.S_Y <= 0)
		{
			jelly.S_Y += 4 * dt;
			jelly.position.y += 4 * dt;
			jelly_jumping.position.y += 10 * dt;
		}
		else
		{
			jelly.S_Y = 0;
			jelly.position.y = 0;
			jelly.state = 0;
			jelly_jumping.state = 2;
		}
	}
	else if (jelly_jumping.state == 2) //jumping down
	{
		if (jelly_jumping.position.y >= 0) jelly_jumping.position.y -= 10 * dt;
		else jelly_jumping.state = 0;
	}

	// jelly npc animation
	for (int i = 0; i <= 1; i++)
	{
		jelly_NPC_Loop[i].position.y = jelly.position.y + jelly_jumping.position.y - 10;
		if (jelly_NPC_Loop[i].state == 0 && jelly_jumping.state != 0)
		{
			if (jelly_NPC_Loop[i].tempposition.x == 0 && jelly_NPC_Loop[i].tempposition.z == 0)
			{
				jelly_NPC_Loop[i].tempposition.x = Math::RandFloatMinMax(-50, 50);
				jelly_NPC_Loop[i].tempposition.z = Math::RandFloatMinMax(-50, 50);
			}
			else if (jelly_NPC_Loop[i].tempR == 0)
			{
				if (sqrt(pow((jelly_NPC_Loop[i].tempposition.x - jelly_NPC_Loop[i].position.x), 2) + pow((jelly_NPC_Loop[i].tempposition.z - jelly_NPC_Loop[i].position.z), 2)) > 5)
				{
					jelly_NPC_Loop[i].tempR = Math::RadianToDegree(atan((jelly_NPC_Loop[i].tempposition.x - jelly_NPC_Loop[i].position.x) / (jelly_NPC_Loop[i].tempposition.z - jelly_NPC_Loop[i].position.z)));
				}
				else
				{
					jelly_NPC_Loop[i].tempposition.x = 0;
					jelly_NPC_Loop[i].tempposition.z = 0;
				}
			}
			else if (jelly_NPC_Loop[i].tempR != 0)
			{
				if (((jelly_NPC_Loop[i].tempposition.x - jelly_NPC_Loop[i].position.x) < 0) && ((jelly_NPC_Loop[i].tempposition.z - jelly_NPC_Loop[i].position.z) > 0)) jelly_NPC_Loop[i].tempR -= jelly_NPC_Loop[i].R_X; /*(z,-x) C*/
				else if (((jelly_NPC_Loop[i].tempposition.x - jelly_NPC_Loop[i].position.x) < 0) && ((jelly_NPC_Loop[i].tempposition.z - jelly_NPC_Loop[i].position.z) < 0)) jelly_NPC_Loop[i].tempR -= (jelly_NPC_Loop[i].R_X + 180); /*(-z,-x) T*/
				else if (((jelly_NPC_Loop[i].tempposition.x - jelly_NPC_Loop[i].position.x) > 0) && ((jelly_NPC_Loop[i].tempposition.z - jelly_NPC_Loop[i].position.z) > 0)) jelly_NPC_Loop[i].tempR -= jelly_NPC_Loop[i].R_X; /*(z,x) A*/
				else if (((jelly_NPC_Loop[i].tempposition.x - jelly_NPC_Loop[i].position.x) > 0) && ((jelly_NPC_Loop[i].tempposition.z - jelly_NPC_Loop[i].position.z) < 0)) jelly_NPC_Loop[i].tempR -= (jelly_NPC_Loop[i].R_X - 180); /*(-z,x) S*/
				if (jelly_NPC_Loop[i].tempR < 0) // trun right
				{
					jelly_NPC_Loop[i].state = 2;
				}
				else if (jelly_NPC_Loop[i].tempR > 0) // trun left
				{
					jelly_NPC_Loop[i].state = 3;
				}

			}
			else jelly_NPC_Loop[i].state = 1;
		}

		if (jelly_NPC_Loop[i].state == 1 && jelly_jumping.state != 0) // move forward
		{
			if (sqrt(pow((jelly_NPC_Loop[i].tempposition.x - jelly_NPC_Loop[i].position.x), 2) + pow((jelly_NPC_Loop[i].tempposition.z - jelly_NPC_Loop[i].position.z), 2)) > 5)
			{
				jelly_NPC_Loop[i].position.x += sin(DegreeToRadian(jelly_NPC_Loop[i].R_X)) * 5.f* dt;
				jelly_NPC_Loop[i].position.z += cos(DegreeToRadian(jelly_NPC_Loop[i].R_X)) * 5.f* dt;
			}
			else
			{
				jelly_NPC_Loop[i].tempposition.x = 0;
				jelly_NPC_Loop[i].tempposition.z = 0;
				jelly_NPC_Loop[i].state = 0;
			}
		}
		if (jelly_NPC_Loop[i].state == 2 && jelly_jumping.state != 0)
		{
			if (jelly_NPC_Loop[i].tempR < -180)
			{
				jelly_NPC_Loop[i].tempR = 360 + jelly_NPC_Loop[i].tempR;
				jelly_NPC_Loop[i].state = 3;
			}
			else if (jelly_NPC_Loop[i].tempR < 0) //turn right
			{
				jelly_NPC_Loop[i].tempR += 18 * dt;
				jelly_NPC_Loop[i].R_X -= 18 * dt;
			}
			else if (jelly_NPC_Loop[i].tempR != 0)
			{

				jelly_NPC_Loop[i].R_X += jelly_NPC_Loop[i].tempR;
				jelly_NPC_Loop[i].tempR = 0;

			}
			else jelly_NPC_Loop[i].state = 1;

		}
		if (jelly_NPC_Loop[i].state == 3 && jelly_jumping.state != 0)
		{

			if (jelly_NPC_Loop[i].tempR>180)
			{
				jelly_NPC_Loop[i].tempR = 0 - 360 + jelly_NPC_Loop[i].tempR;
				jelly_NPC_Loop[i].state = 2;
			}
			else if (jelly_NPC_Loop[i].tempR > 0) //turn left
			{
				jelly_NPC_Loop[i].tempR -= 18 * dt;
				jelly_NPC_Loop[i].R_X += 18 * dt;
			}
			else if (jelly_NPC_Loop[i].tempR != 0)
			{

				jelly_NPC_Loop[i].R_X += jelly_NPC_Loop[i].tempR;
				jelly_NPC_Loop[i].tempR = 0;

			}
			else jelly_NPC_Loop[i].state = 1;
		}
	}
}

void PlanetJellyScene::Update(double dt)
{
	//cout << SharedData::GetInstance()->SD_enableinteract << endl;
	//cout << camera.position << endl;
	button_prompt = 0;

	player.m_origin = camera.nextPosition;
	AABB::updateAABB(player);

	// NPC animation
	Update_animation_NPC(dt);
	// NPC name
	Update_Name_NPC(dt);

	//test codes
	Updata_Checker(dt);


	//Enable culling
	if (Application::IsKeyPressed('1'))
	{
		glEnable(GL_CULL_FACE);
	}
	//Disable culling
	if (Application::IsKeyPressed('2'))
	{
		glDisable(GL_CULL_FACE);
	}
	//Fill mode
	if (Application::IsKeyPressed('3'))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	//Wireframe mode
	if (Application::IsKeyPressed('4'))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (Application::IsKeyPressed('Z'))
	{
		enableLight = false;
	}
	if (Application::IsKeyPressed('X'))
	{
		enableLight = true;
	}
	//if (Application::IsKeyPressed('I'))
	//{
	//	light[0].position.z -= (float)(LSPEED*dt);
	//}
	if (Application::IsKeyPressed('K'))
	{
		light[0].position.z += (float)(LSPEED*dt);
	}
	if (Application::IsKeyPressed('J'))
	{
		light[0].position.x -= (float)(LSPEED*dt);
	}
	if (Application::IsKeyPressed('L'))
	{
		light[0].position.x += (float)(LSPEED*dt);
	}
	if (Application::IsKeyPressed('O'))
	{
		light[0].position.y -= (float)(LSPEED*dt);
	}
	if (Application::IsKeyPressed('P'))
	{
		light[0].position.y += (float)(LSPEED*dt);
	}

	if (currentstate == 0)
	{
		camera.Update(dt);
		checkCollision();
		camera.movement();
	}
	// miningdrone code
	if (SharedData::GetInstance()->SD_MiningDrone_J)
	{
		mdrone_animation(dt);

		if (mdrone_mineralcount < 99)
		{
			if (mdrone_added == false)
			{
				mdrone_mineralcount += ((SharedData::GetInstance()->SD_timecounter - mdrone_starttime) / 60);//increase mineral count using time away from planet
				//every 30 secs, get 10 to 30 minerals
				mdrone_added = true;
			}
			mdrone_starttime = SharedData::GetInstance()->SD_timecounter;
		}
	}




	//Light
	if (Application::IsKeyPressed('8')){
		light[0].type = Light::LIGHT_POINT;
		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	}
	if (Application::IsKeyPressed('9')){
		light[0].type = Light::LIGHT_DIRECTIONAL;
		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	}
	if (Application::IsKeyPressed('0')){
		light[0].type = Light::LIGHT_SPOT;
		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	}

	timer += dt;


	resetKey();
	inventory();
	interactionUpdate(dt);
	Updata_Checker(dt);
	Updata_minigame(dt);
	SharedData::GetInstance()->SD_timecounter += dt;//add time to the timer
}

void PlanetJellyScene::RenderText(Mesh* mesh, std::string text, Color color)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;

	glDisable(GL_DEPTH_TEST);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		float lmao = SharedData::GetInstance()->getFontOffset(text[i]);
		lmao *= 0.01;
		lmao = 1.0f - lmao;
		characterSpacing.SetToTranslation(i * lmao, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);

	glEnable(GL_DEPTH_TEST);
}

void PlanetJellyScene::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;

	glDisable(GL_DEPTH_TEST);

	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Scale(size, size, size);
	modelStack.Translate(x, y, 0);

	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		float lmao = SharedData::GetInstance()->getFontOffset(text[i]);
		lmao *= 0.01;
		lmao = 1.0f - lmao;
		characterSpacing.SetToTranslation(i * lmao, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);

	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();

	glEnable(GL_DEPTH_TEST);
}

void PlanetJellyScene::RenderMesh(Mesh *mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
	if (enableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);
		//Load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}

	if (mesh->textureID > 0)
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
	mesh->Render(); //this line should only be called once 
	if (mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}


}

void PlanetJellyScene::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Tranformation of shapes 
	Mtx44 translate, rotate, scale;
	Mtx44 model;
	Mtx44 view;
	Mtx44 projection;
	Mtx44 MVP;

	translate.SetToIdentity();
	rotate.SetToIdentity();
	scale.SetToIdentity();
	model.SetToIdentity();
	view.SetToIdentity();

	viewStack.LoadIdentity();
	viewStack.LookAt(camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z);
	modelStack.LoadIdentity();

	Position lightPosition_cameraspace = viewStack.Top()*light[0].position;
	glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
	Vector3 spotDirection_cameraspace = viewStack.Top()*light[0].spotDirection;
	glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);

	RenderMesh(meshList[GEO_AXES], false);

	//render light
	modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL], false);
	modelStack.PopMatrix();

	// render skybox
	modelStack.PushMatrix();
	generateSkybox();
	modelStack.PopMatrix();

	// render ground
	modelStack.PushMatrix();
	modelStack.Translate(100, -463, 1000);
	modelStack.Scale(500, 400, 500);
	RenderMesh(meshList[GROUND_MESH], true);
	modelStack.PopMatrix();

	if (currentstate == CAVEGAME)
	{
		modelStack.Translate(camera.position.x, camera.position.y, camera.position.z);
		//modelStack.Rotate(90, 1, 0, 0);
		modelStack.Scale(20, 20, 20);
		RenderMesh(meshList[GEO_CUBE], false);
	}

	//render cave
	modelStack.PushMatrix();
	modelStack.Translate(200, 43, 0);
	modelStack.Rotate(-90, 0, 1, 0);
	modelStack.Scale(40, 40, 40);
	RenderMesh(meshList[CAVE], true);
	modelStack.PopMatrix();


	//render ship
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -460);
	modelStack.Rotate(-105, 0, 1, 0);
	modelStack.Scale(20, 20, 20);
	RenderMesh(meshList[SHIP], true);
	modelStack.PopMatrix();

	// render shop
	modelStack.PushMatrix();
	modelStack.Translate(100, -12, -100);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(20, 20, 20);
	RenderMesh(meshList[SHOP_MINERAL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-100, -20, 0);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[SHOP_DRONE], true);
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(100, -21, 100);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[SHOP_UPGRADE], true);
	modelStack.PopMatrix();

	for (int i = 0; i <= 1; i++)
	{
		// render NPC
		//NPC-NPC
		modelStack.PushMatrix();
		glBlendFunc(1, 1);
		modelStack.Translate(jelly_NPC_Loop[i].position.x, jelly_NPC_Loop[i].position.y, jelly_NPC_Loop[i].position.z);
		modelStack.Rotate(jelly_NPC_Loop[i].R_X, 0, 1, 0);
		modelStack.Scale(5, 5 + jelly.S_Y, 5);
		RenderMesh(meshList[NPC_1], enableLight);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		modelStack.PopMatrix();
	}
		
	
	//if statement here
	if (SharedData::GetInstance()->SD_MiningDrone_J)
	{
		generate_mdrone();//renders mining drone
	}
//NPC - DRONE
	modelStack.PushMatrix();
	modelStack.Translate(-115, -10 + jelly.position.y + jelly_jumping.position.y, -15);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(5, 5 + jelly.S_Y, 5);
	RenderMesh(meshList[NPC_1], true);
	modelStack.PopMatrix();
	


	// NPC-UPGRADE
	modelStack.PushMatrix();
	modelStack.Translate(100, -10 + jelly.position.y + jelly_jumping.position.y, 100);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(5, 5 + jelly.S_Y , 5);
	RenderMesh(meshList[NPC_1], true);
	modelStack.PopMatrix();
	


	// NPC-MINERAL
	modelStack.PushMatrix();
	modelStack.Translate(110, -5 + jelly.position.y + jelly_jumping.position.y, -90);
	modelStack.Scale(5, 5 + jelly.S_Y, 5);
	RenderMesh(meshList[NPC_1], true);
	modelStack.PopMatrix();




	//render names
	for (int i = 0; i < nameS.size() - 1; i++)
	{
		//Render Text-NPC NAME x 2
		//Render Text -DRONE SHOP
		//Render Text -UPGRADE SHOP
		modelStack.PushMatrix();
		modelStack.Translate(nameS[i].position.x, nameS[i].position.y, nameS[i].position.z);
		modelStack.Rotate(0 + nameS[i].R_X, 0, 1, 0);
		modelStack.Scale(2, 2, 2);
		RenderText(meshList[GEO_TEXT], nameS[i].NPCname, Color(0, 1, 0));
		modelStack.PopMatrix();
	}

	//Render Text -MINERAL SHOP
	modelStack.PushMatrix();
	modelStack.Translate(nameS[nameS.size() - 1].position.x, nameS[nameS.size() - 1].position.y + 5, nameS[nameS.size() - 1].position.z);
	modelStack.Rotate(0 + nameS[nameS.size() - 1].R_X, 0, 1, 0);
	modelStack.Scale(2, 2, 2);
	RenderText(meshList[GEO_TEXT], nameS[nameS.size() - 1].NPCname, Color(0, 1, 0));
	modelStack.PopMatrix();




	modelStack.PushMatrix();
	modelStack.Translate(
		(hitbox[3].m_origin.x),
		(hitbox[3].m_origin.y),
		(hitbox[3].m_origin.z)
		);
	modelStack.Scale(
		(hitbox[3].m_length),
		(hitbox[3].m_height),
		(hitbox[3].m_width)
		);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//set to line for line axis
	RenderMesh(meshList[GEO_CUBE], false);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	//set back to fill
	modelStack.PopMatrix();


	viewStack.LoadIdentity();
	modelStack.PushMatrix();
	switch (currentstate){
	case FREEMOVE:
		modelStack.Translate(0, 0, -1);
		modelStack.Rotate(-92.5, 90, 1, 0);
		modelStack.Scale(1.1, 0.8, 0.8);
		RenderMesh(meshList[GEO_UI], false);
		RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(SharedData::GetInstance()->SD_bitcoins), Color(1, 0, 0), 4, 2.1, 1.7);
		RenderTextOnScreen(meshList[GEO_TEXT], "JELLO", Color(1, 0, 0), 3.5, 19.4, 16.2);
		break;
	case CONVERSE:
		modelStack.Translate(0, -0.3, -1);
		modelStack.Rotate(90, 1, 0, 0);
		modelStack.Scale(1, 1, 0.3);
		RenderMesh(meshList[GEO_TEXT_BOX], false);
		break;
	case TRADE:
		modelStack.Translate(0, 0, -1);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(1.1, 0.8, 0.8);
		if (SellState == BROWSING)
		{
			RenderMesh(meshList[GEO_SHOP], false);
			//RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(SharedData::GetInstance()->SD_bitcoins), Color(1, 0, 0), 2.5, 2.6, 2.);
		}
		else
		{
			RenderMesh(meshList[GEO_AMOUNTBOX], false);
		}
		break;
	case INVENTORY:
		modelStack.Translate(-0.035, 0, -0.85);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(1, 1, 0.7);
		if (SellState == DROPPING)
		{
			RenderMesh(meshList[GEO_AMOUNTBOX], false);
		}
		else
		{
			RenderMesh(meshList[GEO_INVENTORY], false);
		}

		//RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(SharedData::GetInstance()->SD_bitcoins), Color(1, 0, 0), 2.5, 2.6, 2.);
		break;

	};
	Render_Checker();
	modelStack.PopMatrix();


	renderinteract();
	Render_minigame();

	text();

}

void PlanetJellyScene::Exit()
{
	// Cleanup VBO here
	glDeleteBuffers(NUM_GEOMETRY, &m_vertexBuffer[0]);
	glDeleteBuffers(NUM_GEOMETRY, &m_colorBuffer[0]);
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

void PlanetJellyScene::generateSkybox(){
	//Front 
	modelStack.PushMatrix();
	modelStack.Translate(camera.position.x, camera.position.y, camera.position.z);
	modelStack.Rotate(-90, 1, 0, 0);
	modelStack.Translate(0, 500, 0);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[SKYBOX_Zposv], false);
	modelStack.PopMatrix();

	//Top 
	modelStack.PushMatrix();
	modelStack.Translate(camera.position.x, camera.position.y, camera.position.z);
	modelStack.Translate(0, 500, 0);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Rotate(-90, 0, 1, 0);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[SKYBOX_Yposv], false);
	modelStack.PopMatrix();

	//Back
	modelStack.PushMatrix();
	modelStack.Translate(camera.position.x, camera.position.y, camera.position.z);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Rotate(-180, 0, 1, 0);
	modelStack.Translate(0, 500, 0);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[SKYBOX_Znega], false);
	modelStack.PopMatrix();

	//Left
	modelStack.PushMatrix();
	modelStack.Translate(camera.position.x, camera.position.y, camera.position.z);
	modelStack.Rotate(90, 0, 0, 1);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Translate(0, 500, 0);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[SKYBOX_Xnega], false);
	modelStack.PopMatrix();

	//Right 
	modelStack.PushMatrix();
	modelStack.Translate(camera.position.x, camera.position.y, camera.position.z);
	modelStack.Rotate(-90, 0, 0, 1);
	modelStack.Rotate(-90, 0, 1, 0);
	modelStack.Translate(0, 500, 0);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[SKYBOX_Xposv], false);
	modelStack.PopMatrix();

	//Bottom 
	modelStack.PushMatrix();
	modelStack.Translate(camera.position.x, camera.position.y, camera.position.z);
	modelStack.Translate(0, -500, 0);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[SKYBOX_Ynega], false);
	modelStack.PopMatrix();
}

void PlanetJellyScene::generate_mdrone()
{
	modelStack.PushMatrix();
	//modelStack.Translate(camera.position.x, camera.position.y, camera.position.z);
	//modelStack.Rotate(-90, 1, 0, 0);
	modelStack.Translate(140, mdrone_y, 22);
	modelStack.Scale(2, 2, 2);
	modelStack.Rotate(-90, 0, 1, 0);
	RenderMesh(meshList[DRONE_BODY], true);	
	modelStack.Rotate(mdrone_spin, 0, 1, 0);
	RenderMesh(meshList[DRONE_PROPELLER], true);
	modelStack.PopMatrix();


}

void PlanetJellyScene::mdrone_animation(double dt)
{
	mdrone_spin += 800*dt;

	if (mdrone_ymax == false)
	{
		mdrone_y += 4 * dt;
		if (mdrone_y > 5)
		{
			mdrone_ymax = true;
		}

	}
	else if (mdrone_ymax == true)
	{
		mdrone_y -= 4 * dt;
		if (mdrone_y < -3)
		{
			mdrone_ymax = false;
		}

	}
}

void PlanetJellyScene::Init_Checker()
{
	num = 0;
	tempnum = 0;
	shop = false;
	confirm = false;
	SellState = 0;
	SBamount = 1;
	SID = 0;
}
void PlanetJellyScene::Updata_Checker(double dt)
{
	// hack mode
	if (Application::IsKeyPressed('B'))
	{
		cout << "check  " << SharedData::GetInstance()->PlayerInventory->Slot[0].name << endl;
		for (int i = 0; i < 3; i++)
		{
			cout << "Before " << i << " + " << SharedData::GetInstance()->PlayerInventory->Slot[i].name << endl;
		}
		SharedData::GetInstance()->PlayerInventory->GetItem(4, 10);
		SharedData::GetInstance()->PlayerInventory->GetItem(10, 10);
		SharedData::GetInstance()->PlayerInventory->GetItem(7, 10);
		SharedData::GetInstance()->SD_bitcoins += 3000;
		SharedData::GetInstance()->PlayerInventory->IncreaseSlots(5);
		for (int i = 0; i < 3; i++)
		{
			cout << "After " << i << " + " << SharedData::GetInstance()->PlayerInventory->Slot[i].name << endl;
		}
	}

	// Buying Upgrades
	if ((Application::IsKeyPressed('E') && currentstate == TRADE && (SID==2 || SID==3) && SellState != UPGRADING && shop==true) && timer > delay)
	{
		delay = timer + 0.2;
		SellState = UPGRADING;
	}

	if (SellState == UPGRADING)
	{
		if ((Application::IsKeyPressed('E') && confirm == true) && timer > delay) // confirm Upgrade
		{
			delay = timer + 0.2;
			SharedData::GetInstance()->PlayerInventory->BuyUpgrade(SharedData::GetInstance()->PlayerInventory->store[SID].PowerS[num].ID, SharedData::GetInstance()->PlayerInventory->store[SID].SID);
			SellState = 0;
			SBamount = 1;
			confirm = false;
		}

		if ((Application::IsKeyPressed('E') && confirm == false) && timer > delay) // cancel
		{
			delay = timer + 0.2;
			SellState = 0;
			SBamount = 1;
			confirm = false;
		}

		if (Application::IsKeyPressed('A')) // confirm/cancel
		{
			confirm = true;
		}

		if (Application::IsKeyPressed('D')) // confirm/cancel
		{
			confirm = false;
		}
	}
	// Buying Drones
	if ((Application::IsKeyPressed('E') && currentstate == TRADE && (SID == 4 || SID == 5) && SellState != DRONES && shop == true && SharedData::GetInstance()->PlayerInventory->store[SID].RebotS[num].Sold == false) && timer > delay)
	{
		delay = timer + 0.2;
		SellState = DRONES;
	}

	if (SellState == DRONES)
	{
		if ((Application::IsKeyPressed('E') && confirm == true) && timer > delay) // confirm buy drone
		{
			delay = timer + 0.2;
			SharedData::GetInstance()->PlayerInventory->BuyDrone(num, SID);
			SellState = 0;
			SBamount = 1;
			confirm = false;
		}

		if ((Application::IsKeyPressed('E') && confirm == false) && timer > delay) // cancel
		{
			delay = timer + 0.2;
			SellState = 0;
			SBamount = 1;
			confirm = false;
		}

		if (Application::IsKeyPressed('A')) // confirm/cancel
		{
			confirm = true;
		}

		if (Application::IsKeyPressed('D')) // confirm/cancel
		{
			confirm = false;
		}
	}

	// droppong items in inventory
	if ((Application::IsKeyPressed('E') && currentstate == INVENTORY && SellState != DROPPING) && timer > delay)
	{
		delay = timer + 0.2;
		if (SharedData::GetInstance()->PlayerInventory->Slot[num].stack > 0)
		{
			SellState = DROPPING;
		}
	}

	if (SellState == DROPPING) // seleting amount to drop
	{
		if (Application::IsKeyPressed('W') && timer > delay) // increase amount to drop
		{
			delay = timer + 0.1;
			if (SBamount < SharedData::GetInstance()->PlayerInventory->Slot[num].stack)
			{
				SBamount++;
			}
		}

		if ((Application::IsKeyPressed('S')) && timer > delay) // decrease amount to drop
		{
			delay = timer + 0.1;
			if (SBamount > 1)
			{
				SBamount--;
			}
		}

		if ((Application::IsKeyPressed('E') && confirm == true) && timer > delay) // confirm dropping
		{
			delay = timer + 0.2;
			SellState = 0;
			SharedData::GetInstance()->PlayerInventory->RemoveItem(SharedData::GetInstance()->PlayerInventory->Slot[num].ID, SBamount);
			SBamount = 1;
			confirm = false;
		}

		if ((Application::IsKeyPressed('E') && confirm == false) && timer > delay) // cancel
		{
			delay = timer + 0.2;
			SellState = 0;
			SBamount = 1;
			confirm = false;
		}

		if (Application::IsKeyPressed('A')) // confirm/cancel
		{
			confirm = true;
		}

		if (Application::IsKeyPressed('D')) // confirm/cancel
		{
			confirm = false;
		}
	}

	// Selling 
	if ((Application::IsKeyPressed('E') && currentstate == TRADE && shop==false && SellState != SELLING) && timer > delay)
	{
		delay = timer + 0.2;
		if (SharedData::GetInstance()->PlayerInventory->Slot[num].stack > 0)
		{
			SellState = SELLING;
		}
	}
	if (SellState == SELLING)
	{
		if ((Application::IsKeyPressed('E') && confirm == true) && timer > delay) // confirm selling
		{
			delay = timer + 0.2;
			SharedData::GetInstance()->PlayerInventory->sellItem(SharedData::GetInstance()->PlayerInventory->Slot[num].ID, SBamount, SharedData::GetInstance()->PlayerInventory->store[SID].SID);
			SellState = 0;
			SBamount = 1;
			confirm = false;
		}

		if (Application::IsKeyPressed('W')  && timer > delay) // increase amount to sell
		{
			delay = timer + 0.1;
			if (SBamount < SharedData::GetInstance()->PlayerInventory->Slot[num].stack)
			{
				SBamount++;
			}
		}
		if ((Application::IsKeyPressed('S')) && timer > delay) // decrease amount to sell
		{
			delay = timer + 0.1;
			if (SBamount > 1)
			{
				SBamount--;
			}
		}
		if ((Application::IsKeyPressed('E') && confirm == false) && timer > delay) // cancel
		{
			delay = timer + 0.2;
			SellState = 0;
			SBamount = 1;
			confirm = false;
		}

		if (Application::IsKeyPressed('A')) // confirm/cancel
		{
			confirm = true;
		}

		if (Application::IsKeyPressed('D')) // confirm/cancel
		{
			confirm = false;
		}
	}

	// Buying
	if ((Application::IsKeyPressed('E') && currentstate == TRADE && shop == true && (SID == 0 || SID == 1) && SellState != BUYING) && timer > delay)
	{
		delay = timer + 0.2;
		if (SharedData::GetInstance()->PlayerInventory->Slot[num].stack > 0)
		{
			SellState = BUYING;
		}
	}
	if (SellState == BUYING)
	{

		if (Application::IsKeyPressed('E')  && confirm == true) // confirm buying
		{
			delay = timer + 0.2;
			SharedData::GetInstance()->PlayerInventory->buyItem(SharedData::GetInstance()->PlayerInventory->store[SID].GoodS[num].ID, SBamount, SharedData::GetInstance()->PlayerInventory->store[SID].SID, num);
			SellState = 0;
			SBamount = 1;
			confirm = false;
		}

		if ((Application::IsKeyPressed('W') ) && timer > delay) // increase amount to buy
		{
			delay = timer + 0.1;
			if (SBamount < SharedData::GetInstance()->PlayerInventory->store[SID].GoodS[num].stack)
			{
				SBamount++;
			}
		}
		if ((Application::IsKeyPressed('S')) && timer > delay) // decrease amount to buy
		{
			delay = timer + 0.1;
			if (SBamount > 1)
			{
				SBamount--;
			}
		}
		if ((Application::IsKeyPressed('E') && confirm == false) && timer > delay) // cancel
		{
			delay = timer + 0.2;
			SellState = 0;
			SBamount = 1;
			confirm = false;
		}

		if (Application::IsKeyPressed('A')) // confirm/cancel
		{
			confirm = true;
		}

		if (Application::IsKeyPressed('D')) // confirm/cancel
		{
			confirm = false;
		}
	}

	// bag - inventory scrolling part
	if ((currentstate == TRADE || currentstate == INVENTORY) && SellState == 0)
	{
		if ((Application::IsKeyPressed('S')) && timer > delay && shop == true) // scroll down your list
		{
			delay = timer + 0.2;
			if ((SID == 0 || SID == 1)) // mineral shop
			{
				if (num < SharedData::GetInstance()->PlayerInventory->store[SID].GoodS.size() - 1)
				{
					num++;
					if (num > 4)
					{
						tempnum++;
					}
				}
			}
			if ((SID == 2 || SID == 3)) // upgrade shop
			{
				if (num < SharedData::GetInstance()->PlayerInventory->store[SID].PowerS.size() - 1)
				{
					num++;
					if (num > 4)
					{
						tempnum++;
					}
				}
			}
			if ((SID == 4 || SID == 5)) // drone shop
			{
				if (num < SharedData::GetInstance()->PlayerInventory->store[SID].RebotS.size() - 1)
				{
					num++;
					if (num > 4)
					{
						tempnum++;
					}
				}
			}
		}

		if ((Application::IsKeyPressed('S')) && timer > delay && shop == false) // scroll down your list
		{
			if (num < SharedData::GetInstance()->PlayerInventory->Slots-1)
			{
				delay = timer + 0.2;
				num++;
				if (num > 4)
				{
					tempnum++;
				}
			}
		}
		if ((Application::IsKeyPressed('W') && SellState == 0) && timer > delay) // scroll up your list
		{
			delay = timer + 0.2;
			if (num > 0)
			{
				num--;
				if (num > 3)
				{
					tempnum--;
				}
			}
		}
	}

	// store - mineral
	if (currentstate == TRADE && SellState == 0)
	{
		if ((Application::IsKeyPressed('A') && shop != true) && timer > delay) // switch to shop
		{
			delay = timer + 0.2;
			shop = true;
			num = 0;
			tempnum = 0;
		}

		if ((Application::IsKeyPressed('D') && shop != false) && timer > delay) // switch to bag
		{
			delay = timer + 0.2;
			shop = false;
			num = 0;
			tempnum = 0;
		}
	}
}
void PlanetJellyScene::Render_Checker()
{
	
	////////////////////////////////////////////////////////// bag

	if (SellState == BROWSING && (currentstate == TRADE || currentstate == INVENTORY))
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Slots :" + std::to_string(SharedData::GetInstance()->PlayerInventory->Slots), Color(1, 0, 0), 2, 34, 26);

		for (int i = 0; i < SharedData::GetInstance()->PlayerInventory->Slots; i++)
		{
			if (i < 5)
			{
				if (shop == false)
				{
					RenderTextOnScreen(meshList[GEO_TEXT], "->", Color(1, 0, 0), 2, 25, (20 - (num - tempnum) * 2));
					RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(i + 1 + tempnum) + "-" + SharedData::GetInstance()->PlayerInventory->Slot[i + tempnum].name, Color(1, 0, 0), 2, 27, (20 - (i * 2)));
					RenderTextOnScreen(meshList[GEO_TEXT], "x" + std::to_string(SharedData::GetInstance()->PlayerInventory->Slot[i + tempnum].stack), Color(1, 0, 0), 2, 38, (20 - (i * 2)));
				}
				else
				{
					RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(i + 1) + "-" + SharedData::GetInstance()->PlayerInventory->Slot[i].name, Color(1, 0, 0), 2, 27, (20 - (i * 2)));
					RenderTextOnScreen(meshList[GEO_TEXT], "x" + std::to_string(SharedData::GetInstance()->PlayerInventory->Slot[i].stack), Color(1, 0, 0), 2, 38, (20 - (i * 2)));
				}
			}
		}
		RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(SharedData::GetInstance()->SD_bitcoins), Color(1, 0, 0), 2, 30, 4);
	}
	////////////////////////////////////////////////////////// shop

	if (SellState == BROWSING && currentstate == TRADE)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], SharedData::GetInstance()->PlayerInventory->store[SID].name, Color(1, 0, 0), 2, 1, 25);

		if (SID == 1 || SID == 0) // mineral shop
		{
			for (int i = 0; i < SharedData::GetInstance()->PlayerInventory->store[SID].GoodS.size(); i++)
			{
				if (i < 5)
				{
					if (shop == true)
					{
						RenderTextOnScreen(meshList[GEO_TEXT], "->", Color(1, 0, 0), 2, 1, (22 - (num - tempnum) * 2));
						RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(SharedData::GetInstance()->PlayerInventory->store[SID].GoodS[i + tempnum].bitcoin) + "-" + SharedData::GetInstance()->PlayerInventory->store[SID].GoodS[i + tempnum].name, Color(1, 0, 0), 2, 3, (22 - (i * 2)));
						RenderTextOnScreen(meshList[GEO_TEXT], "x" + std::to_string(SharedData::GetInstance()->PlayerInventory->store[SID].GoodS[i + tempnum].stack), Color(1, 0, 0), 2, 14, (22 - (i * 2)));
					}
					else
					{
						RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(SharedData::GetInstance()->PlayerInventory->store[SID].GoodS[i].bitcoin) + "-" + SharedData::GetInstance()->PlayerInventory->store[SID].GoodS[i].name, Color(1, 0, 0), 2, 3, (22 - (i * 2)));
						RenderTextOnScreen(meshList[GEO_TEXT], "x" + std::to_string(SharedData::GetInstance()->PlayerInventory->store[SID].GoodS[i].stack), Color(1, 0, 0), 2, 14, (22 - (i * 2)));
					}
				}
			}
		}
		if (SID == 3 || SID == 2) // upgrade shop
		{
			for (int i = 0; i < SharedData::GetInstance()->PlayerInventory->store[SID].PowerS.size(); i++)
			{
				if (i < 5)
				{
					if (shop == true)
					{
						RenderTextOnScreen(meshList[GEO_TEXT], "->", Color(1, 0, 0), 2, 1, (22 - (num) * 2));
						if (SharedData::GetInstance()->PlayerInventory->store[SID].PowerS[i + tempnum].ID != 0)
							RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(SharedData::GetInstance()->PlayerInventory->store[SID].PowerS[i + tempnum].bitcoin) + "-" + SharedData::GetInstance()->PlayerInventory->store[SID].PowerS[i + tempnum].name + std::to_string(SharedData::GetInstance()->PlayerInventory->store[SID].PowerS[i + tempnum].level), Color(1, 0, 0), 2, 3, (22 - (i * 2)));
						else RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(SharedData::GetInstance()->PlayerInventory->store[SID].PowerS[i + tempnum].bitcoin* (SharedData::GetInstance()->SD_hullmax - (int)(SharedData::GetInstance()->SD_hullIntegrity))) + "-" + SharedData::GetInstance()->PlayerInventory->store[SID].PowerS[i + tempnum].name, Color(1, 0, 0), 2, 3, (22 - (i * 2)));
					}
					else
					{
						if (SharedData::GetInstance()->PlayerInventory->store[SID].PowerS[i].ID != 0)
							RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(SharedData::GetInstance()->PlayerInventory->store[SID].PowerS[i].bitcoin) + "-" + SharedData::GetInstance()->PlayerInventory->store[SID].PowerS[i].name + std::to_string(SharedData::GetInstance()->PlayerInventory->store[SID].PowerS[i].level), Color(1, 0, 0), 2, 3, (22 - (i * 2)));
						else RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(SharedData::GetInstance()->PlayerInventory->store[SID].PowerS[i].bitcoin* (SharedData::GetInstance()->SD_hullmax - (int)(SharedData::GetInstance()->SD_hullIntegrity))) + "-" + SharedData::GetInstance()->PlayerInventory->store[SID].PowerS[i].name, Color(1, 0, 0), 2, 3, (22 - (i * 2)));
					}
				}
			}
		}
		if (SID == 5 || SID == 4) // drones shop
		{
			for (int i = 0; i < SharedData::GetInstance()->PlayerInventory->store[SID].RebotS.size(); i++)
			{
				if (i < 5)
				{
					if (shop == true)
					{
						RenderTextOnScreen(meshList[GEO_TEXT], "->", Color(1, 0, 0), 2, 1, (22 - (num - tempnum) * 2));
						if (SharedData::GetInstance()->PlayerInventory->store[SID].RebotS[i + tempnum].Sold == false) 
						{
							RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(SharedData::GetInstance()->PlayerInventory->store[SID].RebotS[i + tempnum].bitcoin) + "-" + SharedData::GetInstance()->PlayerInventory->store[SID].RebotS[i + tempnum].name, Color(1, 0, 0), 2, 3, (22 - (i * 2)));
						}
						else
						{
							RenderTextOnScreen(meshList[GEO_TEXT], "SOLD -" + SharedData::GetInstance()->PlayerInventory->store[SID].RebotS[i + tempnum].name, Color(1, 0, 0), 2, 3, (22 - (i * 2)));
						}
					}
					else
					{
						if (SharedData::GetInstance()->PlayerInventory->store[SID].RebotS[i].Sold == false)
						{
							RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(SharedData::GetInstance()->PlayerInventory->store[SID].RebotS[i].bitcoin) + "-" + SharedData::GetInstance()->PlayerInventory->store[SID].RebotS[i].name, Color(1, 0, 0), 2, 3, (22 - (i * 2)));
						}
						else
						{
							RenderTextOnScreen(meshList[GEO_TEXT], "SOLD -" + SharedData::GetInstance()->PlayerInventory->store[SID].RebotS[i].name, Color(1, 0, 0), 2, 3, (22 - (i * 2)));
						}
					}
				}
			}
		}
	}


	////////////////////////////////////////////////////////// confirm

	if (SellState == SELLING && currentstate == TRADE)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Sell: " + std::to_string(SBamount) + "   (" + SharedData::GetInstance()->PlayerInventory->Slot[num].name + ")", Color(1, 0, 0), 2, 16, 18);
		if (SharedData::GetInstance()->PlayerInventory->Slot[num].PID == SharedData::GetInstance()->PlayerInventory->store[SID].SID)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Bitcoin : " + std::to_string(SharedData::GetInstance()->PlayerInventory->Slot[num].bitcoin * SBamount), Color(1, 0, 0), 2, 14, 15);
		}
		else
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Bitcoin : " + std::to_string(SharedData::GetInstance()->PlayerInventory->Slot[num].bitcoin * SBamount * 2), Color(1, 0, 0), 2, 14, 15);
		}
	}

	if (SellState == BUYING && currentstate == TRADE)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Buy:   " + std::to_string(SBamount) + "   (" + SharedData::GetInstance()->PlayerInventory->store[SID].GoodS[num].name + ")", Color(1, 0, 0), 2, 16, 18);
		RenderTextOnScreen(meshList[GEO_TEXT], "Bitcoin : " + std::to_string(SharedData::GetInstance()->PlayerInventory->store[SID].GoodS[num].bitcoin * SBamount), Color(1, 0, 0), 2, 14, 15);
	}

	if (SellState == UPGRADING && currentstate == TRADE)
	{
		if (SharedData::GetInstance()->PlayerInventory->store[SID].PowerS[num].ID != 0)
		{

			RenderTextOnScreen(meshList[GEO_TEXT], "Buy:  (" + SharedData::GetInstance()->PlayerInventory->store[SID].PowerS[num].name + std::to_string(SharedData::GetInstance()->PlayerInventory->store[SID].PowerS[num].level) + ")", Color(1, 0, 0), 2, 16, 18);
			RenderTextOnScreen(meshList[GEO_TEXT], "Bitcoin : " + std::to_string(SharedData::GetInstance()->PlayerInventory->store[SID].PowerS[num].bitcoin), Color(1, 0, 0), 2, 14, 15);
		}
		else
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Buy:  (" + SharedData::GetInstance()->PlayerInventory->store[SID].PowerS[num].name + ")", Color(1, 0, 0), 2, 16, 18);
			RenderTextOnScreen(meshList[GEO_TEXT], "Bitcoin : " + std::to_string(SharedData::GetInstance()->PlayerInventory->store[SID].PowerS[num].bitcoin* (SharedData::GetInstance()->SD_hullmax - (int)(SharedData::GetInstance()->SD_hullIntegrity))), Color(1, 0, 0), 2, 14, 15);
		}
	}

	if (SellState == DRONES && currentstate == TRADE)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Buy:  (" + SharedData::GetInstance()->PlayerInventory->store[SID].RebotS[num].name + ")", Color(1, 0, 0), 2, 16, 18);
		RenderTextOnScreen(meshList[GEO_TEXT], "Bitcoin : " + std::to_string(SharedData::GetInstance()->PlayerInventory->store[SID].RebotS[num].bitcoin), Color(1, 0, 0), 2, 14, 15);
	}

	if (SellState == DROPPING && currentstate == INVENTORY)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Drop:  " + std::to_string(SBamount) + "   (" + SharedData::GetInstance()->PlayerInventory->Slot[num].name + ")", Color(1, 0, 0), 2, 16, 18);
	}

	if (SellState != BROWSING && (currentstate == TRADE || currentstate == INVENTORY)) //not browsing
	{
		if (confirm == true)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "->", Color(1, 0, 0), 2, 13, 13);
		}
		else
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "->", Color(1, 0, 0), 2, 23, 13);
		}
		RenderTextOnScreen(meshList[GEO_TEXT], "Confirm", Color(1, 0, 0), 2, 15, 13);
		RenderTextOnScreen(meshList[GEO_TEXT], "Cancel", Color(1, 0, 0), 2, 25, 13);
	}

	
}



void PlanetJellyScene::interactionUpdate(double dt)
{
	//cout <<"view : " << camera.view << endl;
	//cout << "Target : " << camera.target << endl;
	if (SharedData::GetInstance()->SD_enableinteract)
	{

		if ((collision(hitbox[0], camera.frontTarget) == true))//Mineral merchant
		{
			if (currentstate == 0)
			{
				button_prompt = 1;
			}

			if ((Application::IsKeyPressed('E') && timer > delay) && e_state == 0)
			{
				e_state = 1;
				SID = 1; // jelly mineral shop mode
				button_prompt = 0;
				interact_state();
				if (rendertext == 0 && currentstate == 1)
				{
					rendertext = 1;
				}
				else
				{
					rendertext = 0;
				}
				delay = timer + 0.5;//set delay offset
			}
		}


		if ((collision(hitbox[1], camera.frontTarget) == true))//upgrade merchant
		{
			if (currentstate == 0)
			{
				button_prompt = 1;
			}
			if ((Application::IsKeyPressed('E') && timer > delay) && e_state == 0)
			{
				e_state = 1;
				SID = 3; // jelly upgrade shop mode
				button_prompt = 0;
				interact_state();
				if (rendertext == 0 && currentstate == 1)
				{
					rendertext = 2;
				}
				else
				{
					rendertext = 0;
				}

				delay = timer + 0.5;//set delay offset
			}
		}

		if ((collision(hitbox[2], camera.frontTarget) == true))//Drone merchant
		{
			if (currentstate == 0)
			{
				button_prompt = 1;
			}
			if ((Application::IsKeyPressed('E') && timer > delay) && e_state == 0)
			{
				e_state = 1;
				SID = 5; // jelly upgrade shop mode
				button_prompt = 0;
				interact_state();
				if (rendertext == 0 && currentstate == 1)
				{
					rendertext = 3;
				}
				else
				{
					rendertext = 0;
				}
				delay = timer + 0.5;//set delay offset
			}
		}

		if (((collision(hitbox[3], camera.frontTarget) == true)) && e_state == 0)//cave
		{
			if (currentstate == FREEMOVE)
			{
				button_prompt = 1;
			}

			if ((Application::IsKeyPressed('E') && timer > delay) && e_state == 0)
			{
				delay = timer + 0.5;//set delay offset

				e_state = 1;
				button_prompt = 0;
				InGame = 1;
				currentstate = CAVEGAME;
				//run your game code
			}

				//to quit
				//change currenstate =0;
		}

		if (((collision(hitbox[4], camera.frontTarget) == true)) && e_state == 0)//ship
		{

			button_prompt = 1;

			if (Application::IsKeyPressed('E'))
			{
				mdrone_added = false;
				SharedData::GetInstance()->SD_enableinteract = false;
				e_state = 1;
				SharedData::GetInstance()->SD_location = OPEN_GALAXY;


			}

		}

		if ((collision(hitbox[5], camera.frontTarget) == true))//Mining drone
		{
			if (currentstate == 0)
			{
				button_prompt = 1;
			}
		}

		if ((collision(hitbox[5], camera.frontTarget)) && SharedData::GetInstance()->SD_MiningDrone_J)//Mining drone
		{
			if (currentstate == 0)
			{
				button_prompt = 1;
			}
			if ((Application::IsKeyPressed('E') && timer > delay) && e_state == 0)
			{
				delay = timer + 0.5;//set delay offset
				e_state = 1;
				button_prompt = 0;
				if (currentstate == CONVERSE)
				{
					SharedData::GetInstance()->PlayerInventory->GetItem(mdrone_mineraltype, mdrone_mineralcount);//add to player inventory

					mdrone_mineralcount = 0;//empty mining drone storage

					mdrone_mineraltype = Math::RandFloatMinMax(1, 4);//switch mineral type to be mined next

					currentstate = FREEMOVE;

					rendertext = 0;
					button_prompt = 1;
					return;


				}
				currentstate = CONVERSE;
				if (rendertext == 0 && mdrone_mineralcount <= 0)//empty mining drone
				{
					rendertext = 4;
				}
				else if (mdrone_mineralcount > 0)//give mined minerals to player
				{
					rendertext = 5;
				}





			}



		}
	}
}

void PlanetJellyScene::interact_state()
{
	if (currentstate == FREEMOVE)
	{
		currentstate = CONVERSE;
		return;

	}
	else if (currentstate == CONVERSE)
	{
		currentstate = TRADE;
		return;
	}
	else if (currentstate == TRADE)
	{
		num = 0;
		tempnum = 0;
		shop = false;
		currentstate = FREEMOVE;
		return;
	}

}

void PlanetJellyScene::renderinteract()
{
	if (button_prompt == 1)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to interact", Color(1, 0, 0), 3.5, 7.4, 5.8);
	}
	else
	{
		return;
	}
}

void PlanetJellyScene::text()
{
	switch (rendertext)
	{
	case(0) : break;//inactive

	case(1) :
		RenderTextOnScreen(meshList[GEO_TEXT], "Bloop???Bloop!", Color(1, 0, 0), 2, 4.2, 5.8);//mineral shop
		break;

	case(2) :
		RenderTextOnScreen(meshList[GEO_TEXT], "BLOOOOOOOOOOOOOO!", Color(1, 0, 0), 2, 4.2, 5.8);//upgrade shop
		RenderTextOnScreen(meshList[GEO_TEXT], "(He places jelly coated components on the counter.)", Color(1, 0, 0), 2, 4.2, 4.6);
		break;

	case(3) :
		RenderTextOnScreen(meshList[GEO_TEXT], "Please do not feed purchased drones to the natives.", Color(1, 0, 0), 2, 4.2, 5.8);//drone shop
		break;
	case(4) :
		RenderTextOnScreen(meshList[GEO_TEXT], "BEGINNING MINING OPERATIONS.", Color(1, 0, 0), 2, 4.2, 5.8);//mining drone (empty)
		RenderTextOnScreen(meshList[GEO_TEXT], "PLEASE RETURN LATER.", Color(1, 0, 0), 2, 4.2, 3.8);
		break;
	case(5) :
		RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(mdrone_mineralcount) + "x" + std::to_string(mdrone_mineraltype) + " READY FOR COLLECTION.", Color(1, 0, 0), 2, 4.2, 5.8);//mining drone (has minerals)
		break;
	};

}

void PlanetJellyScene::resetKey()
{
	if (!Application::IsKeyPressed('E'))
	{
		e_state = 0;
	}
	if (SharedData::GetInstance()->SD_enableinteract == false)
	{

		if (timer > delay)
		{
			delay = timer + 3;
			SharedData::GetInstance()->SD_enableinteract = true;
		}

	}

}

void PlanetJellyScene::checkCollision()
{
	for (int i = 0; i <= 4; i++)
	{
		if ((collision(hitbox[i], camera.nextPosition)))
		{
			camera.colliding = true;
		}
	}


}

void PlanetJellyScene::inventory()
{

	if (Application::IsKeyPressed('I') && (currentstate == INVENTORY || currentstate == FREEMOVE))
	{
		i_state = 1;
		if (timer > delay)
		{
			delay = timer + 0.5;
			if (currentstate == 0)
			{
				num = 0;
				tempnum = 0;
				currentstate = INVENTORY;
			}
			else
			{
				num = 0;
				tempnum = 0;
				currentstate = FREEMOVE;
			}
		}
	}
}

void PlanetJellyScene::Init_minigame()
{
	InGame = 0; // menu
	Pnum = 0;
}
void PlanetJellyScene::Updata_minigame(double dt)
{
	
	if (InGame == 1) // menu
	{
		SharedData::GetInstance()->Game->randSetRocks('B');
		if (Application::IsKeyPressed('W') && Pnum > 0 && timer > delay)
		{
			delay = timer + 0.2;
			Pnum--;
		}
		if (Application::IsKeyPressed('S') && Pnum < 3 && timer > delay)
		{
			delay = timer + 0.2;
			Pnum++;
		}
		if (Application::IsKeyPressed('E') && timer > delay) // change mode
		{
			delay = timer + 0.2;
			if (Pnum == 0) InGame = 2; //Instruction && Controls
			if (Pnum == 1) InGame = 3; // start mining game
			if (Pnum == 2) InGame = 4; // check mineral list
			if (Pnum == 3)
			{
				currentstate = 0;
				InGame = 0; // exit game
			}
			Pnum = 0;
		}
	}
	if (InGame == 2) //Instruction && Controls
	{
		if (Application::IsKeyPressed('W') && Pnum > 0 && timer > delay)
		{
			delay = timer + 0.2;
			Pnum--;
		}
		if (Application::IsKeyPressed('S') && Pnum < 3 && timer > delay)
		{
			delay = timer + 0.2;
			Pnum++;
		}
		if (Application::IsKeyPressed('E') && timer > delay && Pnum == 3)  // back to menu
		{
			delay = timer + 0.2;
			InGame = 1;
			Pnum = 0;
		}
	}
	if (InGame == 3) // really In game
	{
		SharedData::GetInstance()->Game->startFalling(dt, 'B');
		if (Application::IsKeyPressed('A') && SharedData::GetInstance()->Game->Board_P > 3 && timer > delay)
		{
			delay = timer + 0.1;
			SharedData::GetInstance()->Game->Board_P--;
		}

		if (Application::IsKeyPressed('D') && SharedData::GetInstance()->Game->Board_P < 37 && timer > delay)
		{
			delay = timer + 0.1;
			SharedData::GetInstance()->Game->Board_P++;
		}
		if (Application::IsKeyPressed('P') && timer > delay) // end game check score
		{
			delay = timer + 0.2;
			SharedData::GetInstance()->Game->Board_P = 20;
			for (int i = 0; i < 36; i++) SharedData::GetInstance()->Game->reRandRocks(i, 'B');
			InGame = 5;
			Pnum = 0;
		}
	}
	if (InGame == 4) //Instruction && Controls
	{
		if (Application::IsKeyPressed('W') && Pnum > 0 && timer > delay)
		{
			delay = timer + 0.2;
			Pnum--;
		}
		if (Application::IsKeyPressed('S') && Pnum < 4 && timer > delay)
		{
			delay = timer + 0.2;
			Pnum++;
		}
		if (Application::IsKeyPressed('E') && timer > delay && Pnum == 4) // back to menu
		{
			delay = timer + 0.2;
			InGame = 1;
			Pnum = 0;
		}
	}
	if (InGame == 5) //Check Score
	{
		if (Application::IsKeyPressed('W') && Pnum > 0 && timer > delay)
		{
			delay = timer + 0.2;
			Pnum--;
		}
		if (Application::IsKeyPressed('S') && Pnum < 4 && timer > delay)
		{
			delay = timer + 0.2;
			Pnum++;
		}
		if (Application::IsKeyPressed('E') && timer > delay && Pnum == 4) // exit game && Add mineral got in cave to bag
		{
			for (int i = 0; i < 4; i++) SharedData::GetInstance()->PlayerInventory->GetItem(SharedData::GetInstance()->Game->ScoreS[i].ID, SharedData::GetInstance()->Game->ScoreS[i].stack);
			SharedData::GetInstance()->Game->reSetScore();
			delay = timer + 0.2;
			InGame = 0;
			Pnum = 0;
			currentstate = 0;
		}
	}
	
}
void PlanetJellyScene::Render_minigame()
{
	// Game Box
	if (InGame != 0)
	{
		for (int x = 1; x < 40; x++)
		{
			for (int y = 1; y < 30; y++)
			{
				if ((x == 1 && y < 30) || (x == 39 && y < 30) || (x < 40 && y == 1) || (x < 40 && y == 29))
					RenderTextOnScreen(meshList[GEO_TEXT], "+", Color(1, 0, 0), 2, x, y);
			}
		}
		// random falling minerals
		for (int i = 0; i < 36; i++)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "*", Color(1, 0, 0), 2, SharedData::GetInstance()->Game->Rocks[i].P_X, SharedData::GetInstance()->Game->Rocks[i].P_Y);
		}
		// moving mining cart
		RenderTextOnScreen(meshList[GEO_TEXT], "/", Color(1, 0, 0), 2, SharedData::GetInstance()->Game->Board_P + 1.4, 2.3);
		RenderTextOnScreen(meshList[GEO_TEXT], "\\", Color(1, 0, 0), 2, SharedData::GetInstance()->Game->Board_P - 1.6, 2.3);
		for (float x = -1; x <= 1; x += 0.5) RenderTextOnScreen(meshList[GEO_TEXT], "-", Color(1, 0, 0), 2, SharedData::GetInstance()->Game->Board_P + x, 2);
		RenderTextOnScreen(meshList[GEO_TEXT], "o", Color(1, 0, 0), 2, SharedData::GetInstance()->Game->Board_P + 0.5, 1.5);
		RenderTextOnScreen(meshList[GEO_TEXT], "o", Color(1, 0, 0), 2, SharedData::GetInstance()->Game->Board_P - 0.5, 1.5);
		// menu conrols
		if (InGame == 1) //menu
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "->", Color(1, 0, 0), 2, 14, (22 - (Pnum)* 2));
			RenderTextOnScreen(meshList[GEO_TEXT], "Instruction && Controls", Color(1, 0, 0), 2, 16, 22);
			RenderTextOnScreen(meshList[GEO_TEXT], "Start Mining", Color(1, 0, 0), 2, 16, 20);
			RenderTextOnScreen(meshList[GEO_TEXT], "Mineral In Cave", Color(1, 0, 0), 2, 16, 18);
			RenderTextOnScreen(meshList[GEO_TEXT], "Exit Cave", Color(1, 0, 0), 2, 16, 16);
		}
		if (InGame == 2) //Instruction && Controls
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Instruction && Controls", Color(1, 0, 0), 2, 8, 24);
			RenderTextOnScreen(meshList[GEO_TEXT], "->", Color(1, 0, 0), 2, 6, (22 - (Pnum)* 2));
			RenderTextOnScreen(meshList[GEO_TEXT], "Moving mining cart to collect falling minerals", Color(1, 0, 0), 2, 8, 22);
			RenderTextOnScreen(meshList[GEO_TEXT], "Control Arrow Left & Right to move mining cart", Color(1, 0, 0), 2, 8, 20);
			RenderTextOnScreen(meshList[GEO_TEXT], "When in Game Press 'P' to End Game", Color(1, 0, 0), 2, 8, 18);
			RenderTextOnScreen(meshList[GEO_TEXT], "Back to menu", Color(1, 0, 0), 2, 8, 16);
		}
		if (InGame == 4) // mineral list in cave
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Mineral In Cave", Color(1, 0, 0), 2, 16, 24);
			RenderTextOnScreen(meshList[GEO_TEXT], "->", Color(1, 0, 0), 2, 14, (22 - (Pnum)* 2));
			for (int i = 4; i < 8; i++) RenderTextOnScreen(meshList[GEO_TEXT], SharedData::GetInstance()->Game->ItemS[i].name, Color(1, 0, 0), 2, 16, 22 - (i - 4) * 2); // jelly planet mineral
			RenderTextOnScreen(meshList[GEO_TEXT], "Back to menu", Color(1, 0, 0), 2, 16, 14);
		}
		if (InGame == 5) // Check Score
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Mineral Got In Cave", Color(1, 0, 0), 2, 16, 24);
			RenderTextOnScreen(meshList[GEO_TEXT], "->", Color(1, 0, 0), 2, 14, (22 - (Pnum)* 2));
			for (int i = 0; i < 4; i++)
			{
				RenderTextOnScreen(meshList[GEO_TEXT], SharedData::GetInstance()->Game->ScoreS[i].name, Color(1, 0, 0), 2, 16, 22 - i * 2);
				RenderTextOnScreen(meshList[GEO_TEXT], "x "+ std::to_string(SharedData::GetInstance()->Game->ScoreS[i].stack), Color(1, 0, 0), 2, 24, 22 - i * 2);
			}
			RenderTextOnScreen(meshList[GEO_TEXT], "Exit Mining Cave", Color(1, 0, 0), 2, 16, 14);
		}
	}
}