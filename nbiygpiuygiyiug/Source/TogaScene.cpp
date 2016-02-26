#include "TogaScene.h"
#include "GL\glew.h"
#include "MatrixStack.h"
#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"
#include "Utility.h"
#include "LoadTGA.h"
#include "MyMath.h"
#include "Mtx44.h"
#include "Camera_Mouse.h"


#include <iostream>
using std::cout;
using std::endl;

vector<AABB> Shophitbox;


TogaScene::TogaScene()
{
}

TogaScene::~TogaScene()
{
}

void TogaScene::Init()

{
	e_state = 0;
	i_state = 0;
	currentstate = FREEMOVE;

	rendertext = 0;
	
	Shophitbox.push_back(AABB::generateAABB(Vector3(-70, 0, -60), 30, 30, 30, 0));// Mineral shop [0]

	Shophitbox.push_back(AABB::generateAABB(Vector3(-43, 0, 410), 50, 30, 50, 0));// Upgrade shop [1]

	Shophitbox.push_back(AABB::generateAABB(Vector3(90, 0, -80), 60, 24, 60, 0));// drone shop [2]

	Shophitbox.push_back(AABB::generateAABB(Vector3(0, 0, -500), 200, 200, 200, 0));// Cave [3]
	
	Shophitbox.push_back(AABB::generateAABB(Vector3(-56, 0, 664), 450 ,30, 340, 0));// ship [4]
	
	player = AABB::generateAABB(camera.nextPosition, 10, 30, 10, 0);//player




	//random
	Math::InitRNG();
	targetxz = 0;

	//Definations
	boxRange = 200;
	LSPEED = 10.0f;
	//mm (mineral merchant)
	mm_y_max = false;
	mm_head_y = 0;

	//dm drone merchant
	dm_y_max = false;
	dm_y = 0;

	dm_eye_rotate_max = false;
	dm_eye_rotate = 0;
	
	
	//um upgrade merchant 
	um_head_rotate_max = false;
	um_head_rotate  = 0;

	//Togans
	rotate_armR=0;
	rotate_armL=35;
	rotate_legL=0;
	rotate_legR=40;

	armR_max = 0;
	armL_max = 0;
	legR_max = 0;
	legL_max = 0;


	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);

	//Init VBO here
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);

	//Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Enable depth test
	glEnable(GL_DEPTH_TEST);

	//Camera
	camera.Init(Vector3(30, 0, 1), Vector3(30, 0, 0), Vector3(0, 1, 0));
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
	light[0].type = Light::LIGHT_DIRECTIONAL;
	light[0].position.Set(0, 500, 0);
	light[0].color.Set(1, 1, 1);
	light[0].power = 1;
	light[0].kC = 1.0f;
	light[0].kL = 1.0f;
	light[0].kQ = 1.0f;
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

	//Render wishlist
	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("Axes", 1000, 1000, 1000);

	meshList[GEO_LIGHTBALL] = MeshBuilder::GenerateSphere("LightSource", Color(1, 1, 1), 18, 36);

	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("hitbox", Color(0,1,0));

	//Skybox - TOGA
	meshList[SKYBOX_Xposv] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1));
	meshList[SKYBOX_Xposv]->textureID = LoadTGA("Image//Skybox//SkyboxToga//lakes_rt.tga");
	meshList[SKYBOX_Xnega] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1));
	meshList[SKYBOX_Xnega]->textureID = LoadTGA("Image//Skybox//SkyboxToga//lakes_lf.tga");
	meshList[SKYBOX_Yposv] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1));
	meshList[SKYBOX_Yposv]->textureID = LoadTGA("Image//Skybox//SkyboxToga//lakes_up.tga");
	meshList[SKYBOX_Ynega] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1));
	meshList[SKYBOX_Ynega]->textureID = LoadTGA("Image//Skybox//SkyboxToga//lakes_dn.tga");
	meshList[SKYBOX_Zposv] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1));
	meshList[SKYBOX_Zposv]->textureID = LoadTGA("Image/Skybox//SkyboxToga//lakes_ft.tga");
	meshList[SKYBOX_Znega] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1));
	meshList[SKYBOX_Znega]->textureID = LoadTGA("Image//Skybox//SkyboxToga//lakes_bk.tga");
	meshList[GROUNDMESH] = MeshBuilder::GenerateQuad("floor", Color(1, 1, 1));
	meshList[GROUNDMESH]->textureID = LoadTGA("Image//Skybox//SkyboxToga//groundmeshtoga.tga");
	
	//mineral stuff
	meshList[NPC_MINERAL_BODY] = MeshBuilder::GenerateOBJ("mineral merchant head","OBJ//merchantminerals_bodyA.obj");
	meshList[NPC_MINERAL_BODY]->textureID = LoadTGA("Image//merchantmineral_textureA.tga");

	meshList[NPC_MINERAL_HEAD] = MeshBuilder::GenerateOBJ("mineral merchant head", "OBJ//merchantminerals_headA.obj");
	meshList[NPC_MINERAL_HEAD]->textureID = LoadTGA("Image//merchantmineral_textureA.tga");

	meshList[MINERAL_SHOP] = MeshBuilder::GenerateOBJ("mineral merchant head", "OBJ//mineralshopA.obj");
	meshList[MINERAL_SHOP]->textureID = LoadTGA("Image//mineralshop_textureA.tga");


	//Drone stuff
	meshList[NPC_DRONE_BODY] = MeshBuilder::GenerateOBJ("drone merchant body", "OBJ//merchantdrone_bodyA.obj");
	meshList[NPC_DRONE_BODY]->textureID = LoadTGA("Image//merchantdrone_textureA.tga");

	meshList[NPC_DRONE_EYE] = MeshBuilder::GenerateOBJ("drone merchant body", "OBJ//merchantdrone_eyeA.obj");
	meshList[NPC_DRONE_EYE]->textureID = LoadTGA("Image//merchantdrone_textureA.tga");

	meshList[NPC_DRONE_ARMS] = MeshBuilder::GenerateOBJ("drone merchant body", "OBJ//merchantdrone_armsA.obj");
	meshList[NPC_DRONE_ARMS]->textureID = LoadTGA("Image//merchantdrone_textureA.tga");

	meshList[DRONE_SHOP] = MeshBuilder::GenerateOBJ("drone merchant body", "OBJ//drone_shopA.obj");
	meshList[DRONE_SHOP]->textureID = LoadTGA("Image//droneshop_textureA.tga");



	//Upgrade stuff
	meshList[NPC_UPGRADE_BODY] = MeshBuilder::GenerateOBJ("drone merchant body", "OBJ//merchantupgrade_bodyA.obj");
	meshList[NPC_UPGRADE_BODY]->textureID = LoadTGA("Image//merchantupgrade_textureA.tga");

	meshList[NPC_UPGRADE_HEAD] = MeshBuilder::GenerateOBJ("drone merchant body", "OBJ//merchantupgrade_headA.obj");
	meshList[NPC_UPGRADE_HEAD]->textureID = LoadTGA("Image//merchantupgrade_textureA.tga");

	meshList[NPC_UPGRADE_ARML ] = MeshBuilder::GenerateOBJ("drone merchant body", "OBJ//merchantupgrade_armleftA.obj");
	meshList[NPC_UPGRADE_ARML]->textureID = LoadTGA("Image//merchantupgrade_textureA.tga");

	meshList[NPC_UPGRADE_ARMR] = MeshBuilder::GenerateOBJ("drone merchant body", "OBJ//merchantupgrade_armrightA.obj");
	meshList[NPC_UPGRADE_ARMR]->textureID = LoadTGA("Image//merchantupgrade_textureA.tga");

	meshList[UPGRADE_SHOP] = MeshBuilder::GenerateOBJ("drone merchant body", "OBJ//upgradeshopA.obj");
	meshList[UPGRADE_SHOP]->textureID = LoadTGA("Image//upgradeshop_textureA.tga");

	//cave
	meshList[CAVE] = MeshBuilder::GenerateOBJ("drone merchant body", "OBJ//Cave_A.obj");
	meshList[CAVE]->textureID = LoadTGA("Image//cave_textureA.tga");


	//togan wandering npc
	meshList[NPC_TOGAN_BODY] = MeshBuilder::GenerateOBJ("drone merchant body", "OBJ//togaman_body.obj");
	meshList[NPC_TOGAN_BODY]->textureID = LoadTGA("Image//toga_texture.tga");

	meshList[NPC_TOGAN_HEAD] = MeshBuilder::GenerateOBJ("drone merchant body", "OBJ//togaman_head.obj");
	meshList[NPC_TOGAN_HEAD]->textureID = LoadTGA("Image//toga_texture.tga");

	meshList[NPC_TOGAN_ARM] = MeshBuilder::GenerateOBJ("drone merchant body", "OBJ//togaman_arm.obj");
	meshList[NPC_TOGAN_ARM]->textureID = LoadTGA("Image//toga_texture.tga");


	meshList[NPC_TOGAN_LEG] = MeshBuilder::GenerateOBJ("drone merchant body", "OBJ//togaman_leg.obj");
	meshList[NPC_TOGAN_LEG]->textureID = LoadTGA("Image//toga_texture.tga");

	meshList[GEO_UI] = MeshBuilder::GenerateQuad("UI", Color(0, 0, 0));
	meshList[GEO_UI]->textureID = LoadTGA("Image//planet_UI.tga");

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//pixelFont.tga");

	meshList[GEO_TEXT_BOX] = MeshBuilder::GenerateQuad("textbox", Color(0,0,0));
	meshList[GEO_TEXT_BOX]->textureID = LoadTGA("Image//textbox.tga");

	meshList[GEO_SHOP] = MeshBuilder::GenerateQuad("shop screen", Color(0, 0, 0));
	meshList[GEO_SHOP]->textureID = LoadTGA("Image//shop_screen.tga");

	meshList[SHIP] = MeshBuilder::GenerateOBJ("player ship", "OBJ//ship.obj");
	meshList[SHIP]->textureID = LoadTGA("Image//ship_texture1.tga");

	meshList[GEO_INVENTORY] = MeshBuilder::GenerateQuad("inventory", Color(0,0,0));
	meshList[GEO_INVENTORY]->textureID = LoadTGA("Image//Inventory_screen.tga");

	meshList[GEO_AMOUNTBOX] = MeshBuilder::GenerateQuad("amt box", Color(0, 0, 0));
	meshList[GEO_AMOUNTBOX]->textureID = LoadTGA("Image//amount_screen.tga");

	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("Light Cube", Color(1, 0.2509, 0));

	Init_Checker();
	Init_Name_NPC();
	Init_animation_NPC();
}

float inc = 0;

void TogaScene::Update(double dt)
{
	//cout << SharedData::GetInstance()->SD_enableinteract << endl;

	button_prompt = 0;

	player.m_origin = camera.nextPosition;
	AABB::updateAABB(player);


	if (Application::IsKeyPressed('Z'))
	{
		inc += 50 * dt;
		std::cout << inc << endl;
	}
	if (Application::IsKeyPressed('X'))
	{
		inc -= 50 * dt;
		std::cout << inc << endl;
	}
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
	
	
	if (currentstate == 0)
	{
		camera.Update(dt);
		checkCollision();
		camera.movement();
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
	//tempPosition = camera.position;

	resetKey();
	droneAnimation(dt);
	mineralAnimation(dt);
	upgradeAnimation(dt);
	toganwalk(dt);
	inventory();
	interactionUpdate(dt);
	Updata_Checker(dt);

	Update_Name_NPC(dt);
	Update_animation_NPC(dt);
}


void TogaScene::RenderText(Mesh* mesh, std::string text, Color color)
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
		float lmao = getFontOffset(text[i]);
		if (lmao == 28){
			lmao = lmao + (14);
		}
		if (lmao == 21){
			lmao = lmao + (21);
		}
		if (lmao == 35){
			lmao = lmao + (7);
		}
		if (lmao == 14){
			lmao = lmao + (26);
		}
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

void TogaScene::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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
		float lmao = getFontOffset(text[i]);
		if (lmao == 28){
			lmao = lmao + (14);
		}
		if (lmao == 21){
			lmao = lmao + (21);
		}
		if (lmao == 35){
			lmao = lmao + (7);
		}
		if (lmao == 14){
			lmao = lmao + (26);
		}
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

void TogaScene::RenderMesh(Mesh *mesh, bool enableLight)
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

void TogaScene::Render()
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
	Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
	glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);

	RenderMesh(meshList[GEO_AXES], false);

	modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL], false);
	modelStack.PopMatrix();

	//ground
	modelStack.PushMatrix();
	modelStack.Translate(0, -15, 0);
	modelStack.Scale(2000, 2000, 2000);
	RenderMesh(meshList[GROUNDMESH], false);
	modelStack.PopMatrix();


	//skybox
	modelStack.PushMatrix();
	modelStack.Translate(camera.position.x, 0, camera.position.z);
	modelStack.Translate(0, 185, 0);
	generateSkybox();

	modelStack.PopMatrix();
	//Cave
	modelStack.PushMatrix();
	modelStack.Translate(0, -30, -500);
	modelStack.Scale(150, 150, 150);
	RenderMesh(meshList[CAVE], true);
	modelStack.PopMatrix();

	//render ship
	modelStack.PushMatrix();
	modelStack.Translate(-56, 0, 664);
	//modelStack.Rotate(55, 0, 1, 0);
	modelStack.Scale(20, 20, 20);
	RenderMesh(meshList[SHIP], true);
	modelStack.PopMatrix();

	//Mineral merchant
	modelStack.PushMatrix();
	modelStack.Translate(-70, -15, -60);
	modelStack.Rotate(35,0,1,0);
	generateMineralmerchant();
	modelStack.PopMatrix();

	//Drone merchant
	modelStack.PushMatrix();
	modelStack.Translate(60, dm_y-20, -80);
	modelStack.Rotate(-90, 0, 1, 0);
	generateDronemerchant();
	modelStack.PopMatrix();

	//Upgrade merchant
	modelStack.PushMatrix();
	modelStack.Translate(-40, -23, 410);
	modelStack.Rotate(155, 0, 1, 0);
	generateUpgrademerchant();
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(
		(Shophitbox[2].m_origin.x),
		(Shophitbox[2].m_origin.y),
		(Shophitbox[2].m_origin.z)
		);
	modelStack.Scale(
		(Shophitbox[2].m_length),
		(Shophitbox[2].m_height),
		(Shophitbox[2].m_width)
		);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//set to line for line axis
	RenderMesh(meshList[GEO_CUBE], false);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	//set back to fill
	modelStack.PopMatrix();




	// render NPC
	//NPC-NPC
	for (int i = 0; i <= 1; i++)
	{
		modelStack.PushMatrix();
		modelStack.Translate(togan_NPC_Loop[i].position.x, togan_NPC_Loop[i].position.y, togan_NPC_Loop[i].position.z);
		modelStack.Rotate(togan_NPC_Loop[i].rotate_togan-90, 0, 1, 0);
		generateTogan();
		modelStack.PopMatrix();
	}

	//render names
	for (int i = 0; i < nameS.size(); i++)
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
	
		viewStack.LoadIdentity();
		modelStack.PushMatrix();
		switch (currentstate){
		case FREEMOVE:	
		modelStack.Translate(0, 0, -1);
		modelStack.Rotate(-92.5, 90, 1, 0);
		modelStack.Scale(1.1, 0.8, 0.8);
		RenderMesh(meshList[GEO_UI], false);
		RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(SharedData::GetInstance()->SD_bitcoins), Color(1, 0, 0), 4, 1.9, 1.7);
		RenderTextOnScreen(meshList[GEO_TEXT], "TOGA", Color(1, 0, 0), 3.5, 20.4, 16.2);
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
			RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(SharedData::GetInstance()->SD_bitcoins), Color(1, 0, 0), 2.5, 2.6, 2.);
		}
		else
		{
			RenderMesh(meshList[GEO_AMOUNTBOX], false);
		}
		case INVENTORY:
		modelStack.Translate(-0.035, 0, -0.85);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(1, 1, 0.7);
		RenderMesh(meshList[GEO_INVENTORY], false);
		//RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(SharedData::GetInstance()->SD_bitcoins), Color(1, 0, 0), 2.5, 2.6, 2.);
		break;

		};
		modelStack.PopMatrix();


	
	
	renderinteract();
	text();
	Render_Checker();
}

void TogaScene::Exit()
{
	// Cleanup VBO here
	glDeleteBuffers(NUM_GEOMETRY, &m_vertexBuffer[0]);
	glDeleteBuffers(NUM_GEOMETRY, &m_colorBuffer[0]);
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

void TogaScene::generateSkybox(){
	//Front - done
	modelStack.PushMatrix();
	modelStack.Rotate(-90, 1, 0, 0);
	modelStack.Translate(0, 500, 0);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[SKYBOX_Zposv], false);
	modelStack.PopMatrix();

	//Top - done
	modelStack.PushMatrix();
	modelStack.Translate(0, 500, 0);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[SKYBOX_Yposv], false);
	modelStack.PopMatrix();

	//Back
	modelStack.PushMatrix();
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Translate(0, 500, 0);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[SKYBOX_Znega], false);
	modelStack.PopMatrix();

	//Left
	modelStack.PushMatrix();
	modelStack.Rotate(180, 0, 0, 1);
	modelStack.Rotate(-90, 0, 1, 0);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Translate(0, 500, 0);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[SKYBOX_Xnega], false);
	modelStack.PopMatrix();

	//Right - done
	modelStack.PushMatrix();
	modelStack.Rotate(90, 0, 0, 1);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Translate(0, 500, 0);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[SKYBOX_Xposv], false);
	modelStack.PopMatrix();

	//Bottom - done
	modelStack.PushMatrix();
	modelStack.Translate(0, -500, 0);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[SKYBOX_Ynega], false);
	modelStack.PopMatrix();


	
}

void TogaScene::generateMineralmerchant()
{

	//head
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[NPC_MINERAL_BODY], true);
	modelStack.PopMatrix();
	modelStack.PushMatrix();
	modelStack.Translate(0, mm_head_y, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[NPC_MINERAL_HEAD], true);
	modelStack.PopMatrix();
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[MINERAL_SHOP], true);
	modelStack.PopMatrix();
}

void TogaScene::generateDronemerchant()
{

	//head
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -15);
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[NPC_DRONE_BODY], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Rotate(dm_eye_rotate,0, 1, 0);
	modelStack.Translate(0, 0, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[NPC_DRONE_EYE], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[NPC_DRONE_ARMS], true);
	modelStack.PopMatrix();
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(0, 0-dm_y, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[DRONE_SHOP], true);
	modelStack.PopMatrix();
}

void TogaScene::generateUpgrademerchant()
{

	//hbody
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[NPC_UPGRADE_BODY], true);
	modelStack.PopMatrix();


	//head
	modelStack.PushMatrix();
	modelStack.Rotate(um_head_rotate,0, 1, 0);
	modelStack.Translate(0, 0, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[NPC_UPGRADE_HEAD], true);
	modelStack.PopMatrix();

	//Larm
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[NPC_UPGRADE_ARML], true);
	modelStack.PopMatrix();

	//Rarm
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[NPC_UPGRADE_ARMR], true);
	modelStack.PopMatrix();

	//ashop
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[UPGRADE_SHOP], true);
	modelStack.PopMatrix();

}

void TogaScene::generateTogan()
{

	//body
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[NPC_TOGAN_BODY], true);
	modelStack.PopMatrix();

	//head
	modelStack.PushMatrix();
	modelStack.Rotate(0, 1, 1, 1);
	modelStack.Translate(0, 0, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[NPC_TOGAN_HEAD], true);
	modelStack.PopMatrix();

	//legs
	//left leg
	modelStack.PushMatrix();
	modelStack.Translate(0, 10, 3.5);
	modelStack.Rotate(rotate_legL, 0, 0, 1);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[NPC_TOGAN_LEG], true);
	modelStack.PopMatrix();
	//right leg
	modelStack.PushMatrix();	
	modelStack.Translate(0, 10, -3.5);
	modelStack.Rotate(rotate_legR, 0, 0, 1);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[NPC_TOGAN_LEG], true);
	modelStack.PopMatrix();
	
	//arms
	//left arm
	modelStack.PushMatrix();
	modelStack.Translate(2, 22, 8.5);	
	modelStack.Rotate(rotate_armL, 0, 0, 1);
	modelStack.Translate(0, 0, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[NPC_TOGAN_ARM], true);
	modelStack.PopMatrix();
	//right arm
	modelStack.PushMatrix();
	modelStack.Translate(2, 22, -8.5);
	modelStack.Rotate(rotate_armR, 0, 0, 1);
	modelStack.Translate(0, 0, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[NPC_TOGAN_ARM], true);
	modelStack.PopMatrix();

}

void TogaScene::droneAnimation(double dt)
{
	//dm drone merchant
	if (dm_eye_rotate_max == false)
	{

		dm_eye_rotate += 100 * dt;
		if (dm_eye_rotate >= 75)
		{
			dm_eye_rotate_max = true;
		}

	}
	else if (dm_eye_rotate_max == true)
	{

		dm_eye_rotate -= 100 * dt;
		if (dm_eye_rotate <= -75)
		{
			dm_eye_rotate_max = false;
		}

	}
	/////
	if (dm_y_max == false)
	{
		dm_y += 4 * dt;
		if (dm_y > 5)
		{
			dm_y_max = true;
		}

	}
	else if (dm_y_max == true)
	{
		dm_y -= 4 * dt;
		if (dm_y < -3)
		{
			dm_y_max = false;
		}

	}
}

void TogaScene::mineralAnimation(double dt)
{
	//looping animations
	//mm mineral merchant
	if (mm_y_max == false)
	{
		if (mm_head_y < 0.5)
		{
			mm_head_y += 1 * dt;
			if (mm_head_y >= 0.5)
			{
				mm_y_max = true;
			}

		}
	}
	else if (mm_y_max == true)
	{

		mm_head_y -= 1 * dt;
		if (mm_head_y <= -0.5)
		{
			mm_y_max = false;
		}


	}

}

void TogaScene::upgradeAnimation(double dt)
{
	//um upgrade merchant
	if (um_head_rotate_max == false)
	{

		um_head_rotate += 10 * dt;
		if (um_head_rotate >= 15)
		{
			um_head_rotate_max = true;
		}

	}
	else if (um_head_rotate_max == true)
	{

		um_head_rotate -= 10 * dt;
		if (um_head_rotate <= -15)
		{
			um_head_rotate_max = false;
		}


	}

}

void TogaScene::toganwalk(double dt)
{
	if (armR_max ==0 )
	{
		rotate_armR += 7 * dt;
			if(rotate_armR > 35)
			{
				armR_max = 1;
			}
	}
	else if(armR_max == 1)
	{
		rotate_armR -= 7 * dt ;
		if (rotate_armR < -3)
		{
			armR_max = 0;
		}
	}

	if (armL_max == 0)
	{
		rotate_armL += 7 * dt;
		if (rotate_armL > 35)
		{
			armL_max = 1;
		}
	}
	else if (armL_max == 1)
	{
		rotate_armL -= 7 * dt;
		if (rotate_armL < -3)
		{
			armL_max = 0;
		}
	}


	if (legR_max == 0)
	{
		rotate_legR += 7 * dt;
		if (rotate_legR > 40)
		{
			legR_max = 1;
		}
	}
	else if (legR_max == 1)
	{
		rotate_legR -= 7 * dt;
		if (rotate_legR < -5)
		{
			legR_max = 0;
		}
	}

	if (legL_max == 0)
	{
		rotate_legL += 7 * dt;
		if (rotate_legL > 40)
		{
			legL_max = 1;
		}
	}
	else if (legL_max == 1)
	{
		rotate_legL -= 7 * dt;
		if (rotate_legL < -5)
		{
			legL_max = 0;
		}
	}


}


float TogaScene::checkDistance(Vector3 firstvector, Vector3 secondvector)
{

	return sqrt(pow((firstvector.x - secondvector.x), 2) + pow((firstvector.z - secondvector.z), 2));
}

void TogaScene::interactionUpdate(double dt)
{
	//cout <<"view : " << camera.view << endl;
	//cout << "Target : " << camera.target << endl;

	

	if (SharedData::GetInstance()->SD_enableinteract)
	{
		if ((collision(Shophitbox[0], camera.frontTarget) == true))//Mineral merchant
		{
			if (currentstate == 0)
			{
				button_prompt = 1;
			}

			if ((Application::IsKeyPressed('E') && timer > delay) && e_state == 0)
			{
				PID = 0;
				e_state = 1;
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


		if ((collision(Shophitbox[1], camera.frontTarget) == true))//Upgrade merchant
		{
			if (currentstate == 0)
			{
				button_prompt = 1;
			}
			if ((Application::IsKeyPressed('E') && timer > delay) && e_state == 0)
			{
				PID = 1;
				e_state = 1;
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


		if ((collision(Shophitbox[2], camera.frontTarget) == true))//Drone merchant
		{
			if (currentstate == 0)
			{
				button_prompt = 1;
			}
			if ((Application::IsKeyPressed('E') && timer > delay) && e_state == 0)
			{
				e_state = 1;
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

		if (((collision(Shophitbox[3], camera.frontTarget) == true)) && e_state == 0)//cave
		{
			e_state = 1;

		}

		if (collision(Shophitbox[4], camera.frontTarget) && e_state == 0)//ship
		{
			button_prompt = 1;
			
			if (Application::IsKeyPressed('E'))
			{
				SharedData::GetInstance()->SD_enableinteract = false;
				e_state = 1;
				
				SharedData::GetInstance()->SD_location = OPEN_GALAXY;
			}
		}
	}
	
}

void TogaScene::interact_state()
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
		shop = false;
		currentstate = FREEMOVE;
		return;
	}

}

void TogaScene::renderinteract()
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

void TogaScene::text()
{
	switch(rendertext)
	{
	case(0) : break;//inactive
			
	case(1):
	RenderTextOnScreen(meshList[GEO_TEXT], "H-Hey kid, wanna buy some minerals? ", Color(1, 0, 0), 2, 4.2, 5.8);//mineral shop
		break;

	case(2) :
		RenderTextOnScreen(meshList[GEO_TEXT], "I have the parts. If you have the Bitcoins...", Color(1, 0, 0), 2, 4.2, 5.8);//upgrade shop
		break;

	case(3) :
		RenderTextOnScreen(meshList[GEO_TEXT], "Drone prices are non-negotiable.", Color(1, 0, 0), 2, 4.2, 5.8);//drone shop
		break;
	};

}

void TogaScene::resetKey()
{
	if (!Application::IsKeyPressed('E'))
	{
		e_state = 0;
	}
	if (!Application::IsKeyPressed('I'))
	{
		i_state = 0;
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

void TogaScene::checkCollision()
{
	for (int i = 0; i <= 4; i++)
	{		if ((collision(Shophitbox[i], camera.nextPosition)))
		{
			camera.colliding = true;
		
			
		}
	}
	

}

void TogaScene::inventory()
{

	if (Application::IsKeyPressed('I'))// && timer > delay) && e_state == 0)
	{
		i_state = 1;
		if (timer > delay)
		{
			delay = timer + 0.5;
				if (currentstate == 0)
				{
					currentstate = INVENTORY;
				}
				else
				{
					currentstate = FREEMOVE;
				}
		}

	}
	if ((Application::IsKeyPressed(VK_RETURN) && currentstate == INVENTORY && SellState != DROPPING) && timer > delay)
	{
		delay = timer + 0.2;
		if (SharedData::GetInstance()->PlayerInventory->Slot[num].stack > 0)
		{
			SellState = DROPPING;
			cout << "FUCCKCKCKCKCKCCK" << endl;
		}
	}
	if (SellState == DROPPING)
	{
		if (Application::IsKeyPressed(VK_UP) && timer > delay) // increase amount to sell
		{
			delay = timer + 0.2;
			if (SBamount < SharedData::GetInstance()->PlayerInventory->Slot[num].stack)
			{
				SBamount++;
			}
		}

		if ((Application::IsKeyPressed(VK_DOWN)) && timer > delay) // decrease amount to buy/sell
		{
			delay = timer + 0.2;
			if (SBamount > 1)
			{
				SBamount--;
			}
		}

		if ((Application::IsKeyPressed(VK_RETURN)&& confirm == true) && timer > delay) // confirm dropping
		{
			delay = timer + 0.2;
			SellState = 0;
			SharedData::GetInstance()->PlayerInventory->RemoveItem(SharedData::GetInstance()->PlayerInventory->Slot[num].ID, SBamount);
			SBamount = 1;
			confirm = false;
		}

		if ((Application::IsKeyPressed(VK_RETURN) && confirm == false) && timer > delay) // cancel
		{
			delay = timer + 0.2;
			SellState = 0;
			SBamount = 1;
			confirm = false;
		}

		if (Application::IsKeyPressed(VK_LEFT)) // confirm/cancel
		{
			confirm = true;
		}

		if (Application::IsKeyPressed(VK_RIGHT)) // confirm/cancel
		{
			confirm = false;
		}
	}

}

void TogaScene::Init_Checker()
{
	num = 0;
	tempnum = 0;
	shop = false;
	confirm = false;
	SellState = 0;
	SBamount = 1;
	PID = 0; // toga
}
void TogaScene::Updata_Checker(double dt)
{
	cout << SellState << endl;
	if (Application::IsKeyPressed('B'))
	{
		cout << "check  " << SharedData::GetInstance()->PlayerInventory->Slot[0].name << endl;
		/*SharedData::GetInstance()->PlayerInventory->Bag::GetItem(4, 10);*/
		for (int i = 0; i < 3; i++)
		{
			cout << "Before " << i << " + " << SharedData::GetInstance()->PlayerInventory->Slot[i].name << endl;
		}
		cout << SharedData::GetInstance()->SD_bitcoins << endl;
		SharedData::GetInstance()->PlayerInventory->GetItem(4, 10);
		SharedData::GetInstance()->PlayerInventory->GetItem(10, 10);
		SharedData::GetInstance()->PlayerInventory->GetItem(7, 10);


		for (int i = 0; i < 3; i++)
		{
			cout << "After " << i << " + " << SharedData::GetInstance()->PlayerInventory->Slot[i].name << endl;
		}


		cout << SharedData::GetInstance()->SD_bitcoins << endl;
		/*SharedData::GetInstance()->PlayerInventory->buyItem(6, 10, 'A');*/

		cout << SharedData::GetInstance()->SD_bitcoins << endl;
		/*	for (int i = 0; i < 3; i++)
		{
		cout << "After " << i << " + " << SharedData::GetInstance()->PlayerInventory->Slot[i].name << endl;
		}*/
		/*	cout << camera.target.x << endl;
		cout << camera.target.y << endl;*/
		/*cout << "check  " << SharedData::GetInstance()->PlayerInventory->Slots << endl;*/
	}

	if (currentstate == TRADE || currentstate == INVENTORY)
	{
		if ((Application::IsKeyPressed(VK_DOWN) && SellState == 0) && timer > delay) // scroll down your list
		{
			delay = timer + 0.2;
			if (num < SharedData::GetInstance()->PlayerInventory->Slots - 1 && shop == false)
			{
				num++;
				if (num > 4)
				{
					tempnum++;
				}
			}

			if (num < SharedData::GetInstance()->PlayerInventory->store[PID].GoodS.size() - 1 && shop == true)
			{
				num++;
				if (num > 4)
				{
					tempnum++;
				}
			}


		}
		if ((Application::IsKeyPressed(VK_UP) && SellState == 0) && timer > delay) // scroll up your list
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
	if (currentstate == TRADE)
	{
		if ((Application::IsKeyPressed(VK_LEFT) && shop != true && SellState == 0) && timer > delay) // switch to shop
		{
			delay = timer + 0.2;
			shop = true;
			num = 0;
			tempnum = 0;
		}

		if ((Application::IsKeyPressed(VK_RIGHT) && shop != false && SellState == 0) && timer > delay) // switch to bag
		{
			delay = timer + 0.2;
			shop = false;
			num = 0;
			tempnum = 0;
		}

		if ((Application::IsKeyPressed(VK_RETURN) && shop != true && SellState == 0) && timer > delay) // press to sell from bag
		{
			delay = timer + 0.2;
			if (SharedData::GetInstance()->PlayerInventory->Slot[num].stack > 0)
			{
				SellState = 1;
			}
		}

		if ((Application::IsKeyPressed(VK_RETURN) && shop == true && SellState == 0) && timer > delay) // press to buy from shop
		{
			delay = timer + 0.2;
			if (SharedData::GetInstance()->PlayerInventory->store[PID].GoodS[num].stack > 0)
			{
				SellState = 2;

			}
		}

		if (Application::IsKeyPressed(VK_UP) && SellState == 1 && timer > delay) // increase amount to sell
		{
			delay = timer + 0.2;
			if (SBamount < SharedData::GetInstance()->PlayerInventory->Slot[num].stack)
			{
				SBamount++;
			}
		}

		if ((Application::IsKeyPressed(VK_UP) && SellState == 2) && timer > delay) // increase amount to buy
		{
			delay = timer + 0.2;
			if (SBamount < SharedData::GetInstance()->PlayerInventory->store[PID].GoodS[num].stack)
			{
				SBamount++;
			}
		}

		if ((Application::IsKeyPressed(VK_DOWN) && SellState != 0) && timer > delay) // decrease amount to buy/sell
		{
			delay = timer + 0.2;
			if (SBamount > 1)
			{
				SBamount--;
			}
		}

		if ((Application::IsKeyPressed(VK_RETURN) && shop != true && SellState == 1 && confirm == true) && timer > delay) // confirm selling
		{
			delay = timer + 0.2;
			SharedData::GetInstance()->PlayerInventory->sellItem(SharedData::GetInstance()->PlayerInventory->Slot[num].ID, SBamount, SharedData::GetInstance()->PlayerInventory->store[PID].PID);
			SellState = 0;
			SBamount = 1;
			confirm = false;
		}

		if (Application::IsKeyPressed(VK_RETURN) && shop == true && SellState == 2 && confirm == true) // confirm buying
		{
			delay = timer + 0.2;
			SharedData::GetInstance()->PlayerInventory->buyItem(SharedData::GetInstance()->PlayerInventory->store[PID].GoodS[num].ID, SBamount, SharedData::GetInstance()->PlayerInventory->store[PID].PID, num);
			SellState = 0;
			SBamount = 1;
			confirm = false;
		}

		if ((Application::IsKeyPressed(VK_RETURN) && SellState != 0 && confirm == false) && timer > delay) // cancel
		{
			delay = timer + 0.2;
			SellState = 0;
			SBamount = 1;
			confirm = false;
		}

		if (Application::IsKeyPressed(VK_LEFT) && SellState != 0) // confirm/cancel
		{
			confirm = true;
		}

		if (Application::IsKeyPressed(VK_RIGHT) && SellState != 0) // confirm/cancel
		{
			confirm = false;
		}

	}
}
void TogaScene::Render_Checker()
{

	if (SellState == BROWSING && (currentstate == TRADE || currentstate == INVENTORY))
	{
		////////////////////////////////////////////////////////// bag
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
	if (SellState == BROWSING && currentstate == TRADE)
	{
		////////////////////////////////////////////////////////// shop

		RenderTextOnScreen(meshList[GEO_TEXT], SharedData::GetInstance()->PlayerInventory->store[PID].name, Color(1, 0, 0), 2, 1, 25);

		for (int i = 0; i < SharedData::GetInstance()->PlayerInventory->store[PID].GoodS.size(); i++)
		{

			if (i < 5)
			{
				if (shop == true)
				{
					RenderTextOnScreen(meshList[GEO_TEXT], "->", Color(1, 0, 0), 2, 1, (22 - (num - tempnum) * 2));
					RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(SharedData::GetInstance()->PlayerInventory->store[PID].GoodS[i + tempnum].bitcoin * 2) + "-" + SharedData::GetInstance()->PlayerInventory->store[PID].GoodS[i + tempnum].name, Color(1, 0, 0), 2, 3, (22 - (i * 2)));
					RenderTextOnScreen(meshList[GEO_TEXT], "x" + std::to_string(SharedData::GetInstance()->PlayerInventory->store[PID].GoodS[i + tempnum].stack), Color(1, 0, 0), 2, 14, (22 - (i * 2)));
				}
				else
				{
					RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(SharedData::GetInstance()->PlayerInventory->store[PID].GoodS[i].bitcoin * 2) + "-" + SharedData::GetInstance()->PlayerInventory->store[PID].GoodS[i].name, Color(1, 0, 0), 2, 3, (22 - (i * 2)));
					RenderTextOnScreen(meshList[GEO_TEXT], "x" + std::to_string(SharedData::GetInstance()->PlayerInventory->store[PID].GoodS[i].stack), Color(1, 0, 0), 2, 14, (22 - (i * 2)));
				}
			}

		}
		//RenderTextOnScreen(meshList[GEO_TEXT], "Items in shop :" + std::to_string(SharedData::GetInstance()->PlayerInventory->store[PID].GoodS.size()), Color(1, 0, 0), 2, 7, 12);
	}

	////////////////////////////////////////////////////////// confirm
	if (SellState == SELLING && currentstate == TRADE)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Sell: " + std::to_string(SBamount) + "   (" + SharedData::GetInstance()->PlayerInventory->Slot[num].name + ")", Color(1, 0, 0), 2, 16, 18);
		if (SharedData::GetInstance()->PlayerInventory->Slot[num].PID == SharedData::GetInstance()->PlayerInventory->store[PID].PID)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Bitcoin : " + std::to_string(SharedData::GetInstance()->PlayerInventory->Slot[num].bitcoin * SBamount), Color(1, 0, 0), 2, 14, 15);
		}
		else
			RenderTextOnScreen(meshList[GEO_TEXT], "Bitcoin : " + std::to_string(SharedData::GetInstance()->PlayerInventory->Slot[num].bitcoin * SBamount * 2), Color(1, 0, 0), 2, 14, 15);
	}
	if (SellState == BUYING && currentstate == TRADE)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Buy:   " + std::to_string(SBamount) + "   (" + SharedData::GetInstance()->PlayerInventory->store[PID].GoodS[num].name + ")", Color(1, 0, 0), 2, 16, 18);
		RenderTextOnScreen(meshList[GEO_TEXT], "Bitcoin : " + std::to_string(SharedData::GetInstance()->PlayerInventory->store[PID].GoodS[num].bitcoin * SBamount * 2), Color(1, 0, 0), 2, 14, 15);

	}
	if (SellState == DROPPING && currentstate == INVENTORY)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Drop:  " + std::to_string(SBamount) + "   (" + SharedData::GetInstance()->PlayerInventory->Slot[num].name + ")", Color(1, 0, 0), 2, 16, 18);
	}

	if (SellState != BROWSING && (currentstate == TRADE || currentstate == INVENTORY))//not browsing
	{
		if (confirm == true)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "->", Color(1, 0, 0), 2, 13, 13);
		}
		else RenderTextOnScreen(meshList[GEO_TEXT], "->", Color(1, 0, 0), 2, 23, 13);
		RenderTextOnScreen(meshList[GEO_TEXT], "Confirm", Color(1, 0, 0), 2, 15, 13);
		RenderTextOnScreen(meshList[GEO_TEXT], "Cancel", Color(1, 0, 0), 2, 25, 13);
	}

}

void TogaScene::Init_Name_NPC()
{

	for (int i = 0; i < 2; i++)
	{
		name newname;
		newname.position = Vector3(0, 25, 0);
		newname.R_X = 0;
		newname.state = 0;
		newname.tempR = 0;
		newname.NPCname = "Togan NPC";
		nameS.push_back(newname);
	}

	name newname0;
	newname0.position = Vector3(60, 10, -80);
	newname0.R_X = 0;
	newname0.state = 0;
	newname0.tempR = 0;
	newname0.NPCname = "DRONE SHOP";
	nameS.push_back(newname0);

	name newname2;
	newname2.position = Vector3(-40, 10, 410);
	newname2.R_X = 0;
	newname2.state = 0;
	newname2.tempR = 0;
	newname2.NPCname = "UPGRADE SHOP";
	nameS.push_back(newname2);

	name newname1;
	newname1.position = Vector3(-70, 10, -60);
	newname1.R_X = 0;
	newname1.state = 0;
	newname1.tempR = 0;
	newname1.NPCname = "MINERAL SHOP";
	nameS.push_back(newname1);
}
void TogaScene::Update_Name_NPC(double dt)
{
	for (int i = 0; i < nameS.size(); i++)
	{
		if (i < (nameS.size() - 3)) // only for the moving npc
		{
			nameS[i].position.x = togan_NPC_Loop[i].position.x;
			nameS[i].position.z = togan_NPC_Loop[i].position.z;
		}
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
			nameS[i].tempR = 0 - 360 + nameS[i].tempR;
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

void TogaScene::Init_animation_NPC()
{
	// jelly animation
	togan.position = Vector3(0, 0, 0);


	//jelly npc animation
	for (int i = 0; i <= 1; i++)
	{
		Togan newtoga;
		newtoga.position = Vector3(0, -15, 0);
		newtoga.tempposition = Vector3(0, 0, 0);
		newtoga.rotate_togan = 0;
		newtoga.state = 0;
		newtoga.tempR = 0;
		togan_NPC_Loop.push_back(newtoga);
	}
}
void TogaScene::Update_animation_NPC(double dt)
{

	// jelly npc animation
	for (int i = 0; i <= 1; i++)
	{
		
		if (togan_NPC_Loop[i].state == 0 )
		{
			if (togan_NPC_Loop[i].tempposition.x == 0 && togan_NPC_Loop[i].tempposition.z == 0)
			{
				togan_NPC_Loop[i].tempposition.x = Math::RandFloatMinMax(-50, 50);
				togan_NPC_Loop[i].tempposition.z = Math::RandFloatMinMax(-50, 50);
			}
			else if (togan_NPC_Loop[i].tempR == 0)
			{
				if (sqrt(pow((togan_NPC_Loop[i].tempposition.x - togan_NPC_Loop[i].position.x), 2) + pow((togan_NPC_Loop[i].tempposition.z - togan_NPC_Loop[i].position.z), 2)) > 5)
				{
					togan_NPC_Loop[i].tempR = Math::RadianToDegree(atan((togan_NPC_Loop[i].tempposition.x - togan_NPC_Loop[i].position.x) / (togan_NPC_Loop[i].tempposition.z - togan_NPC_Loop[i].position.z)));
				}
				else
				{
					togan_NPC_Loop[i].tempposition.x = 0;
					togan_NPC_Loop[i].tempposition.z = 0;
				}
			}
			else if (togan_NPC_Loop[i].tempR != 0)
			{
				if (((togan_NPC_Loop[i].tempposition.x - togan_NPC_Loop[i].position.x) < 0) && ((togan_NPC_Loop[i].tempposition.z - togan_NPC_Loop[i].position.z) > 0)) togan_NPC_Loop[i].tempR -= togan_NPC_Loop[i].rotate_togan; /*(z,-x) C*/
				else if (((togan_NPC_Loop[i].tempposition.x - togan_NPC_Loop[i].position.x) < 0) && ((togan_NPC_Loop[i].tempposition.z - togan_NPC_Loop[i].position.z) < 0)) togan_NPC_Loop[i].tempR -= (togan_NPC_Loop[i].rotate_togan + 180); /*(-z,-x) T*/
				else if (((togan_NPC_Loop[i].tempposition.x - togan_NPC_Loop[i].position.x) > 0) && ((togan_NPC_Loop[i].tempposition.z - togan_NPC_Loop[i].position.z) > 0)) togan_NPC_Loop[i].tempR -= togan_NPC_Loop[i].rotate_togan; /*(z,x) A*/
				else if (((togan_NPC_Loop[i].tempposition.x - togan_NPC_Loop[i].position.x) > 0) && ((togan_NPC_Loop[i].tempposition.z - togan_NPC_Loop[i].position.z) < 0)) togan_NPC_Loop[i].tempR -= (togan_NPC_Loop[i].rotate_togan - 180); /*(-z,x) S*/
				if (togan_NPC_Loop[i].tempR < 0) // trun right
				{
					togan_NPC_Loop[i].state = 2;
				}
				else if (togan_NPC_Loop[i].tempR > 0) // trun left
				{
					togan_NPC_Loop[i].state = 3;
				}

			}
			else togan_NPC_Loop[i].state = 1;
		}

		if (togan_NPC_Loop[i].state == 1 ) // move forward
		{
			if (sqrt(pow((togan_NPC_Loop[i].tempposition.x - togan_NPC_Loop[i].position.x), 2) + pow((togan_NPC_Loop[i].tempposition.z - togan_NPC_Loop[i].position.z), 2)) > 5)
			{
				togan_NPC_Loop[i].position.x += sin(DegreeToRadian(togan_NPC_Loop[i].rotate_togan)) * 5.f* dt;
				togan_NPC_Loop[i].position.z += cos(DegreeToRadian(togan_NPC_Loop[i].rotate_togan)) * 5.f* dt;
			}
			else
			{
				togan_NPC_Loop[i].tempposition.x = 0;
				togan_NPC_Loop[i].tempposition.z = 0;
				togan_NPC_Loop[i].state = 0;
			}
		}
		if (togan_NPC_Loop[i].state == 2 )
		{
			if (togan_NPC_Loop[i].tempR < -180)
			{
				togan_NPC_Loop[i].tempR = 360 + togan_NPC_Loop[i].tempR;
				togan_NPC_Loop[i].state = 3;
			}
			else if (togan_NPC_Loop[i].tempR < 0) //turn right
			{
				togan_NPC_Loop[i].tempR += 18 * dt;
				togan_NPC_Loop[i].rotate_togan -= 18 * dt;
			}
			else if (togan_NPC_Loop[i].tempR != 0)
			{

				togan_NPC_Loop[i].rotate_togan += togan_NPC_Loop[i].tempR;
				togan_NPC_Loop[i].tempR = 0;

			}
			else togan_NPC_Loop[i].state = 1;

		}
		if (togan_NPC_Loop[i].state == 3 )
		{

			if (togan_NPC_Loop[i].tempR>180)
			{
				togan_NPC_Loop[i].tempR = 0 - 360 + togan_NPC_Loop[i].tempR;
				togan_NPC_Loop[i].state = 2;
			}
			else if (togan_NPC_Loop[i].tempR > 0) //turn left
			{
				togan_NPC_Loop[i].tempR -= 18 * dt;
				togan_NPC_Loop[i].rotate_togan += 18 * dt;
			}
			else if (togan_NPC_Loop[i].tempR != 0)
			{

				togan_NPC_Loop[i].rotate_togan += togan_NPC_Loop[i].tempR;
				togan_NPC_Loop[i].tempR = 0;

			}
			else togan_NPC_Loop[i].state = 1;
		}
	}
}
