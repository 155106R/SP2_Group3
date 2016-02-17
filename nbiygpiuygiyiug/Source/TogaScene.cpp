#include "TogaScene.h"
#include "GL\glew.h"
#include "MatrixStack.h"
#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"
#include "Utility.h"
#include "LoadTGA.h"

TogaScene::TogaScene()
{
}

TogaScene::~TogaScene()
{
}

void TogaScene::Init()
{
	//Definations
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
	camera.Init(Vector3(30, 30, 30), Vector3(0, 0, 0), Vector3(0, 10, 0));
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


}

void TogaScene::Update(double dt)
{
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

	camera.Update(dt);


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

	//looping animations
	//mm mineral merchant
	if (mm_y_max == false)
	{
		if (mm_head_y < 1.5)
		{
			mm_head_y += 1 * dt;
			if (mm_head_y >= 1.5)
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

	//um drone merchant
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
	modelStack.Translate(0, 0, 0);
	modelStack.Scale(2000, 2000, 2000);
	RenderMesh(meshList[GROUNDMESH], false);
	modelStack.PopMatrix();


	//skybox
	modelStack.PushMatrix();
	modelStack.Translate(camera.position.x, 0, camera.position.z);
	modelStack.Translate(0, 200, 0);
	generateSkybox();

	modelStack.PopMatrix();
	//Cave
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -500);
	modelStack.Scale(150, 150, 150);
	RenderMesh(meshList[CAVE], true);
	modelStack.PopMatrix();

	//Mineral merchant
	modelStack.PushMatrix();
	modelStack.Translate(-70, 0, -60);
	modelStack.Rotate(35,0,1,0);
	generateMineralmerchant();
	modelStack.PopMatrix();

	//Drone merchant
	modelStack.PushMatrix();
	modelStack.Translate(60, dm_y-5, -80);
	modelStack.Rotate(-75, 0, 1, 0);
	generateDronemerchant();
	modelStack.PopMatrix();

	//Upgrade merchant
	modelStack.PushMatrix();
	modelStack.Translate(-40, -8, 410);
	modelStack.Rotate(155, 0, 1, 0);
	generateUpgrademerchant();
	modelStack.PopMatrix();

	//Upgrade merchant
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0);
	generateTogan();
	modelStack.PopMatrix();
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
	modelStack.Translate(0, 0, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[NPC_TOGAN_HEAD], true);
	modelStack.PopMatrix();

	//legs
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[NPC_TOGAN_LEG], true);
	modelStack.PopMatrix();

	//arm
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[NPC_TOGAN_ARM], true);
	modelStack.PopMatrix();


}