#include "PlanetJellyScene.h"
#include "GL\glew.h"
#include "MatrixStack.h"
#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"
#include "Utility.h"
#include "LoadTGA.h"
#include "Mtx44.h"

PlanetJellyScene::PlanetJellyScene()
{
}

PlanetJellyScene::~PlanetJellyScene()
{
}

void PlanetJellyScene::Init()
{
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

	//Render NCP
	meshList[NPC_1] = MeshBuilder::GenerateOBJ("npc", "OBJ//jelly.obj");
	meshList[NPC_1]->textureID = LoadTGA("Image//jelly_texture.tga");

	// jelly animation
	jelly.position = Vector3(0, 0, 0);
	jelly.S_Y = 0;
	jelly.state = 0;

	// jelly_jumping animation
	jelly_jumping.position = Vector3(0, 0, 0);
	jelly_jumping.state = 0;

	// jelly npc animation
	jelly_NPC_Loop.position = Vector3(0, 0, 0);
	jelly_NPC_Loop.R_X = 0;
	jelly_NPC_Loop.state = 3;

}

void PlanetJellyScene::Update(double dt)
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
			jelly_jumping.position.y += 10* dt;
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
	if (jelly_NPC_Loop.state == 1 && jelly_jumping.state != 0) // move forward
	{
		jelly_NPC_Loop.position.x += sin(DegreeToRadian(jelly_NPC_Loop.R_X)) * 5.f* dt;
		jelly_NPC_Loop.position.z += cos(DegreeToRadian(jelly_NPC_Loop.R_X)) * 5.f* dt;
	}
	if (jelly_NPC_Loop.state == 2 && jelly_jumping.state != 0) // trun right
	{
		jelly_NPC_Loop.R_X -= 18 * dt;
	}
	if (jelly_NPC_Loop.state == 3 && jelly_jumping.state != 0) // turn left
	{
		jelly_NPC_Loop.R_X += 18 * dt;
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

	if (Application::IsKeyPressed('Z'))
	{
		enableLight = false;
	}
	if (Application::IsKeyPressed('X'))
	{
		enableLight = true;
	}
	if (Application::IsKeyPressed('I'))
	{
		light[0].position.z -= (float)(LSPEED*dt);
	}
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
	RenderMesh(meshList[GROUND_MESH], enableLight);
	modelStack.PopMatrix();

	//render cave
	modelStack.PushMatrix();
	modelStack.Translate(200, 43, 0);
	modelStack.Rotate(-90, 0, 1, 0);
	modelStack.Scale(40, 40, 40);
	RenderMesh(meshList[CAVE], enableLight);
	modelStack.PopMatrix();

	// render shop
	modelStack.PushMatrix();
	modelStack.Translate(100, -12, -100);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(20, 20, 20);
	RenderMesh(meshList[SHOP_MINERAL], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-100, -20, 0);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[SHOP_DRONE], enableLight);
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(100, -21, 100);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[SHOP_UPGRADE], enableLight);
	modelStack.PopMatrix();

	// render NPC
	// NPC-NPC
	modelStack.PushMatrix();
	glBlendFunc(1,1);
	modelStack.Translate(10 + jelly_NPC_Loop.position.x, -10 + jelly.position.y + jelly_jumping.position.y, 10 + jelly_NPC_Loop.position.z);
	modelStack.Rotate(jelly_NPC_Loop.R_X , 0, 1, 0);
	modelStack.Scale(5, 5 + jelly.S_Y, 5);
	RenderMesh(meshList[NPC_1], enableLight);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	modelStack.PopMatrix();
	// NPC-DRONE
	modelStack.PushMatrix();
	modelStack.Translate(-115, -10 + jelly.position.y + jelly_jumping.position.y, -15);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(5, 5 + jelly.S_Y, 5);
	RenderMesh(meshList[NPC_1], enableLight);
	modelStack.PopMatrix();
	// NPC-UPGRADE
	modelStack.PushMatrix();
	modelStack.Translate(100, -10 + jelly.position.y + jelly_jumping.position.y, 100);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(5, 5 + jelly.S_Y , 5);
	RenderMesh(meshList[NPC_1], enableLight);
	modelStack.PopMatrix();
	// NPC-MINERAL
	modelStack.PushMatrix();
	modelStack.Translate(110, -5 + jelly.position.y + jelly_jumping.position.y, -90);
	modelStack.Scale(5, 5 + jelly.S_Y, 5);
	RenderMesh(meshList[NPC_1], enableLight);
	modelStack.PopMatrix();

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