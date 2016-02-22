#include "OpenGalaxyScene.h"
#include "GL\glew.h"
#include "MatrixStack.h"
#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"
#include "Utility.h"
#include "LoadTGA.h"
#include "Mtx44.h"
#include "AABB.h"

vector<AABB> AABBboxForAsteroid;

OpenGalaxyScene::OpenGalaxyScene()
{

}

OpenGalaxyScene::~OpenGalaxyScene()
{

}

void OpenGalaxyScene::Init()
{
	//Collision 

	//Test box from origin

	//Definations
	LSPEED = 10.0f;

	land = false;

	rotateShip = 0;
	shipAxisX = 0;
	shipAxisY = 0;
	shipAxisZ = 0;
	rotateShipZ = 0;
	noseOfShip = new Vector3(1, 0, 0);
	middleOfShip = new Vector3(shipAxisX, shipAxisY, shipAxisZ);
	accelerateShip = 0;

	spaceshipHitbox = AABB::generateAABB(*middleOfShip, 20, 5, 17.5, 0);

	//Init random location to spawn for each astroid
	for (unsigned i = 0; i < 500; i++){
		randTranslateX[i] = Math::RandFloatMinMax(-500, 500);
		randTranslateY[i] = Math::RandFloatMinMax(-500, 500);
		randTranslateZ[i] = Math::RandFloatMinMax(-500, 500);

		asteroidTranslateX[i] = Math::RandFloatMinMax(-0.05f, 0.05f);
		asteroidTranslateY[i] = Math::RandFloatMinMax(-0.05f, 0.05f);
		asteroidTranslateZ[i] = Math::RandFloatMinMax(-0.05f, 0.05f);

		randScaleX[i] = Math::RandFloatMinMax(1, 5);
		randScaleY[i] = Math::RandFloatMinMax(1, 5);
		randScaleZ[i] = Math::RandFloatMinMax(1, 5);

		AABBboxForAsteroid.push_back(

			AABB::generateAABB(

			Vector3(randTranslateX[i], randTranslateY[i], randTranslateZ[i]),

			randScaleX[i] * 2,
			randScaleY[i] * 2,
			randScaleZ[i] * 2,

			Vector3(asteroidTranslateX[i], asteroidTranslateY[i], asteroidTranslateZ[i])

			)
		);

	}

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
	camera.Init(Vector3(30, 30, 30), *middleOfShip, Vector3(0, 10, 0));
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

	//Light settings
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

	light[1].type = Light::LIGHT_SPOT;
	light[1].position.Set(0, 10, 0);
	light[1].color.Set(1, 1, 1);
	light[1].power = 10;
	light[1].kC = 1.0f;
	light[1].kL = 1.0f;
	light[1].kQ = 1.0f;
	light[1].cosCutoff = cos(Math::DegreeToRadian(45));
	light[1].cosInner = cos(Math::DegreeToRadian(30));
	light[1].exponenet = 1.0f;
	light[1].spotDirection.Set(0.f, 0.f, 1.f);

	//Get a handle for our "MVP" uniform
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");

	//Light Stuff
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutOff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponenet");

	m_parameters[U_LIGHT1_POSITION] = glGetUniformLocation(m_programID, "lights[1].position_cameraspace");
	m_parameters[U_LIGHT1_COLOR] = glGetUniformLocation(m_programID, "lights[1].color");
	m_parameters[U_LIGHT1_POWER] = glGetUniformLocation(m_programID, "lights[1].power");
	m_parameters[U_LIGHT1_KC] = glGetUniformLocation(m_programID, "lights[1].kC");
	m_parameters[U_LIGHT1_KL] = glGetUniformLocation(m_programID, "lights[1].kL");
	m_parameters[U_LIGHT1_KQ] = glGetUniformLocation(m_programID, "lights[1].kQ");
	m_parameters[U_LIGHT1_TYPE] = glGetUniformLocation(m_programID, "lights[1].type");
	m_parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[1].spotDirection");
	m_parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[1].cosCutOff");
	m_parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(m_programID, "lights[1].cosInner");
	m_parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(m_programID, "lights[1].exponenet");

	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");

	glUseProgram(m_programID);

	//Uniform parameter
	glUniform1i(m_parameters[U_NUMLIGHTS], 2);

	glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], light[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], light[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponenet);

	glUniform1i(m_parameters[U_LIGHT1_TYPE], light[1].type);
	glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &light[1].color.r);
	glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);
	glUniform1f(m_parameters[U_LIGHT1_KC], light[1].kC);
	glUniform1f(m_parameters[U_LIGHT1_KL], light[1].kL);
	glUniform1f(m_parameters[U_LIGHT1_KQ], light[1].kQ);
	glUniform1f(m_parameters[U_LIGHT1_COSCUTOFF], light[1].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT1_COSINNER], light[1].cosInner);
	glUniform1f(m_parameters[U_LIGHT1_EXPONENT], light[1].exponenet);

	//light toggle shit
	enableLight = true;

	//Render wishlist
	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("Axes", 1000, 1000, 1000);

	meshList[GEO_LIGHTCUBE] = MeshBuilder::GenerateCube("Light Cube", Color(0, 1, 0));

	meshList[PLANET_SUN] = MeshBuilder::GenerateOBJ("LightSource", "OBJ//planet.obj");
	meshList[PLANET_SUN]->textureID = LoadTGA("Image//planetSunTexture.tga");

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//pixelFont.tga");

	//Skybox - Galaxy
	meshList[SKYBOX_Xposv] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1));
	meshList[SKYBOX_Xposv]->textureID = LoadTGA("Image//Skybox//Galaxy//galaxy+X.tga");
	meshList[SKYBOX_Xnega] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1));
	meshList[SKYBOX_Xnega]->textureID = LoadTGA("Image//Skybox//Galaxy//galaxy-X.tga");
	meshList[SKYBOX_Yposv] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1));
	meshList[SKYBOX_Yposv]->textureID = LoadTGA("Image//Skybox//Galaxy//galaxy+Y.tga");
	meshList[SKYBOX_Ynega] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1));
	meshList[SKYBOX_Ynega]->textureID = LoadTGA("Image//Skybox//Galaxy//galaxy-Y.tga");
	meshList[SKYBOX_Zposv] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1));
	meshList[SKYBOX_Zposv]->textureID = LoadTGA("Image//Skybox//Galaxy//galaxy+Z.tga");
	meshList[SKYBOX_Znega] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1));
	meshList[SKYBOX_Znega]->textureID = LoadTGA("Image//Skybox//Galaxy//galaxy-Z.tga");

	//Proxy SpaceShip
	meshList[PROXY_SPACESHIP] = MeshBuilder::GeneratePyramid("Proxy spaceship", Color(0.5, 0.5, 0.5), 18, 36);
	meshList[PROXY_SPACESHIP]->material.kAmbient.Set(0.5f, 0.5f, 0.5f);
	meshList[PROXY_SPACESHIP]->material.kDiffuse.Set(0.5f, 0.5f, 0.5f);
	meshList[PROXY_SPACESHIP]->material.kSpecular.Set(0.5f, 0.5f, 0.5f);
	meshList[PROXY_SPACESHIP]->material.kShininess = 0.1f;

	//Spaceship
	meshList[SPACESHIP] = MeshBuilder::GenerateOBJ("Spaceship", "OBJ//ship.obj");
	meshList[SPACESHIP]->textureID = LoadTGA("Image//ship_texture1.tga");
		
	//Asteroids
	meshList[ASTEROIDS] = MeshBuilder::GenerateSphere("Asteroids", Color(0.2941, 0.2941, 0.2941), 5, 10); //75, 75, 75 in RGB (Grey)
	meshList[ASTEROIDS]->material.kAmbient.Set(0.5f, 0.5f, 0.5f);
	meshList[ASTEROIDS]->material.kDiffuse.Set(0.5f, 0.5f, 0.5f);
	meshList[ASTEROIDS]->material.kSpecular.Set(0.5f, 0.5f, 0.5f);
	meshList[ASTEROIDS]->material.kShininess = 0.1f;

	//Planets
	meshList[PLANET_A] = MeshBuilder::GenerateOBJ("PlanetA", "OBJ//planet.obj");
	meshList[PLANET_A]->textureID = LoadTGA("Image//planetATexture.tga");

	meshList[PLANET_B] = MeshBuilder::GenerateOBJ("PlanetB", "OBJ//planet.obj");
	meshList[PLANET_B]->textureID = LoadTGA("Image//planetBTexture.tga");

	meshList[PLANET_C] = MeshBuilder::GenerateOBJ("PlanetC", "OBJ//planet.obj");
	meshList[PLANET_C]->textureID = LoadTGA("Image//planetCTexture.tga");

	//RenderHUD
	meshList[GEO_HUD] = MeshBuilder::GenerateQuad("HUD", Color(0, 0, 0));
	meshList[GEO_HUD]->textureID = LoadTGA("Image//HUD.tga");

}

void OpenGalaxyScene::Update(double dt)
{

	AABB::updateAABB(spaceshipHitbox);
	spaceshipHitbox.m_origin = *middleOfShip;

	for (int i = 0; i < AABBboxForAsteroid.size(); i++){
		AABB::updateAABB(AABBboxForAsteroid[i]);

		if (collision(AABBboxForAsteroid[i], spaceshipHitbox)){
			std::cout << "YOU RAN INTO AN ASTEROID DIPSHIT " << i << std::endl;
			SharedData::GetInstance()->SD_hullIntegrity--;
			SharedData::GetInstance()->SD_bitcoins++;

			AABBboxForAsteroid[i].m_velocity = *noseOfShip * (accelerateShip * 0.015);
			AABBboxForAsteroid[i].m_origin += *noseOfShip;
		}

		for (int a = 0; a < AABBboxForAsteroid.size(); a++){
			if (a == i){
				break;
			}
			else if (collision(AABBboxForAsteroid[a], AABBboxForAsteroid[i])){

				AABBboxForAsteroid[a].m_velocity = -(AABBboxForAsteroid[a].m_velocity);
				AABBboxForAsteroid[i].m_velocity = -(AABBboxForAsteroid[i].m_velocity);

			}
		}

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

	//Camera Movement
	camera.target = *middleOfShip;
	camera.target.y = (middleOfShip->y) + 15;
	camera.Update(dt);

	////Light
	//if (Application::IsKeyPressed('8')){
	//	light[1].type = Light::LIGHT_POINT;
	//	glUniform1i(m_parameters[U_LIGHT1_TYPE], light[1].type);
	//}
	//if (Application::IsKeyPressed('9')){
	//	light[1].type = Light::LIGHT_DIRECTIONAL;
	//	glUniform1i(m_parameters[U_LIGHT1_TYPE], light[1].type);
	//}
	//if (Application::IsKeyPressed('0')){
	//	light[1].type = Light::LIGHT_SPOT;
	//	glUniform1i(m_parameters[U_LIGHT1_TYPE], light[1].type);
	//}

	updateShipMovement(dt);

	//Planet interaction/docking
	if (((*middleOfShip - (Vector3(250, 250, 0))).Length()) < 100){	//for planet A
		land = true;
		nameOfPlanet = "sean's planet";

		if (Application::IsKeyPressed('E')){
			//SharedData::GetInstance()->location = 69; //Does not fucking exist 
		}
	}
	else if (((*middleOfShip - (Vector3(250, 0, 250))).Length()) < 100){	//for planet B
		land = true;
		nameOfPlanet = "Toga";

		if (Application::IsKeyPressed('E')){
			SharedData::GetInstance()->SD_location = PLANET_TOGA;
		}
	}
	else if (((*middleOfShip - (Vector3(-250, 0, -250))).Length()) < 100){	//for planet C
		land = true;
		nameOfPlanet = "Jelly Planet";

		if (Application::IsKeyPressed('E')){
			SharedData::GetInstance()->SD_location = PLANET_JELLY;
		}
	}
	else{
		land = false;
	}
}

void OpenGalaxyScene::RenderText(Mesh* mesh, std::string text, Color color)
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

void OpenGalaxyScene::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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

void OpenGalaxyScene::RenderMesh(Mesh *mesh, bool enableLight)
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

void OpenGalaxyScene::Render()
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

	//Disabled Sun Directional light first to test ship spotliht
	modelStack.LoadIdentity();
	Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
	glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);

	//Ship spotlight
	modelStack.LoadIdentity();
	Position lightPosition1_cameraspace = viewStack.Top() * light[1].position;
	glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, &lightPosition1_cameraspace.x);

	//Axes
	RenderMesh(meshList[GEO_AXES], false);

	//Sun
	modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	modelStack.Scale(50, 50, 50);
	RenderMesh(meshList[PLANET_SUN], false);
	modelStack.PopMatrix();

	//Test Lightcube - ER it's called a lightcube but i kinda just use it for testing in general. For this, it's the direction vector for the ship
	modelStack.PushMatrix();
	modelStack.Translate(noseOfShip->x, noseOfShip->y, noseOfShip->z);
	RenderMesh(meshList[GEO_LIGHTCUBE], false);
	modelStack.PopMatrix();

	//Skybox
	modelStack.PushMatrix();
	modelStack.Translate(camera.position.x, camera.position.y, camera.position.z);	//toggle for if skybox moves with skybox
	generateSkybox();
	modelStack.PopMatrix();

	//Render Asteroids
	//This is for the object itself
	for (unsigned i = 0; i < AABBboxForAsteroid.size(); i++){
		modelStack.PushMatrix();
		modelStack.Translate(AABBboxForAsteroid[i].m_origin.x, AABBboxForAsteroid[i].m_origin.y, AABBboxForAsteroid[i].m_origin.z);
		modelStack.Scale(randScaleX[i], randScaleY[i], randScaleZ[i]);
		RenderMesh(meshList[ASTEROIDS], true);
		modelStack.PopMatrix();

	//Render hitbox for Astroid
		modelStack.PushMatrix();
		modelStack.Translate(
			(AABBboxForAsteroid[i].m_origin.x),
			(AABBboxForAsteroid[i].m_origin.y),
			(AABBboxForAsteroid[i].m_origin.z)
			);
		modelStack.Scale(
			(AABBboxForAsteroid[i].m_length),
			(AABBboxForAsteroid[i].m_height),
			(AABBboxForAsteroid[i].m_width)
			);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//set to line for line axis
		RenderMesh(meshList[GEO_LIGHTCUBE], false);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	//set back to fill
		modelStack.PopMatrix();
	}

	//Spaceship
	modelStack.PushMatrix();
	modelStack.Translate(middleOfShip->x, middleOfShip->y, middleOfShip->z);
	modelStack.Rotate(rotateShip, 0, 1, 0);
	modelStack.Rotate(rotateShipZ, 0, 0, 1);
	RenderMesh(meshList[SPACESHIP], true);
	modelStack.PopMatrix();

	//test generate AABB from origin for spaceship
	modelStack.PushMatrix();
	modelStack.Translate(
		(spaceshipHitbox.m_origin.x),
		(spaceshipHitbox.m_origin.y),
		(spaceshipHitbox.m_origin.z)
		);
	modelStack.Rotate(rotateShip, 0, 1, 0);
	modelStack.Scale(
		(spaceshipHitbox.m_length),
		(spaceshipHitbox.m_height),
		(spaceshipHitbox.m_width)
		);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//set to line for line axis
	RenderMesh(meshList[GEO_LIGHTCUBE], false);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	//set back to fill
	modelStack.PopMatrix();

	//Planets
	//Planet A
	modelStack.PushMatrix();
	modelStack.Translate(250, 250, 0);
	modelStack.Scale(50, 50, 50);
	RenderMesh(meshList[PLANET_A], false);
	modelStack.Translate(-1.5, 1, 0);
	RenderText(meshList[GEO_TEXT], "Planet A", Color(1, 0, 0));
	modelStack.PopMatrix();

	//Planet B
	modelStack.PushMatrix();
	modelStack.Translate(250, 0, 250);
	modelStack.Scale(50, 50, 50);
	RenderMesh(meshList[PLANET_B], false);
	modelStack.Translate(-1.5, 1, 0);
	RenderText(meshList[GEO_TEXT], "Planet B", Color(1, 0, 0));
	modelStack.PopMatrix();

	//Planet C
	modelStack.PushMatrix();
	modelStack.Translate(-250, 0, -250);
	modelStack.Scale(50, 50, 50);
	RenderMesh(meshList[PLANET_C], false);
	modelStack.Translate(-1.5, 1, 0);
	RenderText(meshList[GEO_TEXT], "Planet C", Color(1, 0, 0));
	modelStack.PopMatrix();

	//Text if ship is within range of landing on planets
	while (land){
		RenderTextOnScreen(meshList[GEO_TEXT], "Press \"E\" to land on " + nameOfPlanet, Color(1, 0, 0), 2, 0.5, 5);
		break;
	}

	drawHUD();

}

void OpenGalaxyScene::Exit()
{
	// Cleanup VBO here
	glDeleteBuffers(NUM_GEOMETRY, &m_vertexBuffer[0]);
	glDeleteBuffers(NUM_GEOMETRY, &m_colorBuffer[0]);
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

void OpenGalaxyScene::generateSkybox(){
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
	modelStack.Rotate(-180, 0, 1, 0);
	modelStack.Translate(0, 500, 0);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[SKYBOX_Znega], false);
	modelStack.PopMatrix();

	//Left
	modelStack.PushMatrix();
	modelStack.Rotate(90, 0, 0, 1);
	//modelStack.Rotate(-90, 0, 1, 0);
	modelStack.Translate(0, 500, 0);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[SKYBOX_Xnega], false);
	modelStack.PopMatrix();

	//Right - done
	modelStack.PushMatrix();
	modelStack.Rotate(-90, 0, 0, 1);
	modelStack.Rotate(180, 0, 1, 0);
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

void OpenGalaxyScene::updateShipMovement(float dt){
	//For stabilisation
	*middleOfShip += (*noseOfShip) * accelerateShip * dt;
	if (rotateShipZ && (!Application::IsKeyPressed('U')) && (!Application::IsKeyPressed('O'))){	//to "stabilize" ship
		if (rotateShipZ > 0){
			rotateShipZ -= 0.1f;
		}
		else if (rotateShipZ < 0){
			rotateShipZ += 0.1f;
		}
	}
	//For ship movement
	if (Application::IsKeyPressed('J')){
		rotateShip += 1.0f;
		Mtx44 rotate;
		rotate.SetToRotation(1, 0, 1, 0);

		(*noseOfShip) = rotate * (*noseOfShip);
	}
	if (Application::IsKeyPressed('L')){
		rotateShip -= 1.0f;
		Mtx44 rotate;
		rotate.SetToRotation(-1, 0, 1, 0);

		(*noseOfShip) = rotate * (*noseOfShip);
	}
	if (Application::IsKeyPressed('K')){
		if (accelerateShip > -10.0f){
			accelerateShip -= 1.0f;		//deceleration capped at -10.0f
		}
	}
	if (Application::IsKeyPressed('I')){
		if (accelerateShip < 100.0f){	//acceleration capped at 100.0f
			accelerateShip += 1.0f;
		}
	}
	if (Application::IsKeyPressed('U')){
		middleOfShip->y += 0.05f;
		if (rotateShipZ < -5.0f){
			rotateShipZ += 0.5f;
		}
	}
	if (Application::IsKeyPressed('O')){
		middleOfShip->y -= 0.05f;
		if (rotateShipZ > 5.0f){
			rotateShipZ -= 0.5f;
		}
	}
	if (Application::IsKeyPressed('P')){	//break acceleration to 0
		if (accelerateShip != 0){
			if (accelerateShip > 0){
				accelerateShip -= 0.5f;
			}
			else{
				accelerateShip += 0.5f;
			}
		}
	}

}

void OpenGalaxyScene::drawHUD()
{
	viewStack.LoadIdentity();
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -1);
	modelStack.Rotate(-92.5, 90, 1, 0);
	modelStack.Scale(1.1, 0.8, 0.8);
	RenderMesh(meshList[GEO_HUD], false);
	modelStack.PopMatrix();

	RenderTextOnScreen(meshList[GEO_TEXT], "Bitcoins:" + std::to_string(SharedData::GetInstance()->SD_bitcoins), Color(1, 0, 0), 3, 0.5, 18);
	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string((int)SharedData::GetInstance()->SD_hullIntegrity), Color(1, 0, 0), 3, 22.5, 3.5);

	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string((int)middleOfShip->x), Color(1, 0, 0), 3, 4.5, 6.5);
	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string((int)middleOfShip->y), Color(1, 0, 0), 3, 4.5, 5.5);
	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string((int)middleOfShip->z), Color(1, 0, 0), 3, 4.5, 4.5);
	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string((int)accelerateShip), Color(1, 0, 0), 4, 18.0, 5.7);
}

//Author: Randall (155106R)
//Updated 22/2/2016 - Randall