#include "OpenGalaxyScene.h"
#include "GL\glew.h"
#include "MatrixStack.h"
#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"
#include "Utility.h"
#include "LoadTGA.h"
#include "Mtx44.h"

vector<Asteroid*> allAsteroids;

OpenGalaxyScene::OpenGalaxyScene()
{

}

OpenGalaxyScene::~OpenGalaxyScene()
{

}

void OpenGalaxyScene::Init()
{
	Math::InitRNG();

	//Definations
	e_state = 0;//e key state

	land = false;						//If player is within bounds of landing on a planet
	rotateShip = 0;						//Rotates the ship accordingly to input
	rotateShipZ = 0;					//Rotates when ship travels along y-axis
	noseOfShip = new Vector3(1, 0, 0);	//Directional vector of which direction player is moving in
	middleOfShip = new Vector3(0, 0, 0);//Ship position
	accelerateShip = 0;

	CURRENT_STATE = PILOTING;
	inShip = false;						//bool check if player is in ship interior or piloting

	//Hardcoded hitbox
	spaceshipHitbox = AABB::generateAABB(*middleOfShip, 20, 5, 17.5, NULL);
	planetA_Hitbox = AABB::generateAABB(Vector3(250, 250, 0), 100, 100, 100, NULL);		//---]
	planetB_Hitbox = AABB::generateAABB(Vector3(250, 0, 250), 100, 100, 100, NULL);		//   |}> This might be kind of retarded. just do a length check??
	planetC_Hitbox = AABB::generateAABB(Vector3(-250, 0, -250), 100, 100, 100, NULL);	//---]

	shipInterior = AABB::generateAABB(Vector3(middleOfShip->x, middleOfShip->y, middleOfShip->z), 41, 12, 20.5, NULL);

	//Generate asteroids
	for (unsigned i = 0; i < 200; i++){	//change loop number for how many asteroids you want to random
		allAsteroids.push_back(new Asteroid());
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
	shipCamera.Init(Vector3(30, 30, 30), *middleOfShip, Vector3(0, 10, 0));
	shipCamera.Reset();
	inShipCamera.Init(Vector3(0, 0, 1), Vector3(0, 0, 0), Vector3(0, 1, 0)); //if using Camera_Mouse
	inShipCamera.Reset();

	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	//Load vertex and fragment shaders
	//m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Texture.fragmentshader");
	m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");

	// Get a handle for our "colorTexture" uniform
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");

	// Get a handle for our "textColor" uniform
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");

	//Light settings
	//Sun light
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

	//Spaceship interior
	light[1].type = Light::LIGHT_DIRECTIONAL;
	light[1].position.Set(middleOfShip->x, middleOfShip->y + 5, middleOfShip->z);
	light[1].color.Set(1, 1, 1);
	light[1].power = 100;
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

	meshList[GEO_LIGHTCUBE] = MeshBuilder::GenerateCube("Light Cube", Color(1, 0.2509, 0));

	//Light source
	meshList[PLANET_SUN] = MeshBuilder::GenerateOBJ("LightSource", "OBJ//planet.obj");
	meshList[PLANET_SUN]->textureID = LoadTGA("Image//planetSunTexture.tga");

	//Text select
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

	//Spaceship
	meshList[SPACESHIP] = MeshBuilder::GenerateOBJ("Spaceship", "OBJ//ship.obj");
	meshList[SPACESHIP]->textureID = LoadTGA("Image//ship_texture1.tga");

	//Spaceship Interior
	meshList[SPACESHIP_INTERIOR] = MeshBuilder::GenerateOBJ("Spaceship", "OBJ//ship_interior.obj");
	meshList[SPACESHIP_INTERIOR]->textureID = LoadTGA("Image//ship_interior.tga");
		
	//Asteroids
	meshList[ASTEROIDS] = MeshBuilder::GenerateSphere("Asteroids", Color(0.2941, 0.2941, 0.2941), 3, 7); //75, 75, 75 in RGB (Grey)
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
	timer += dt;


	spaceshipHitbox.m_origin = *middleOfShip;
	AABB::updateAABB(spaceshipHitbox);

	//std::cout << allAsteroids[0]->material << ": " << allAsteroids[0]->count << std::endl;


	for (int i = 0; i < allAsteroids.size(); i++){
		AABB::updateAABB(allAsteroids[i]->hitbox);//Currently updating the translate for asteroids in render according to the asteroid's hitbox position

		//================================================================================================================================================================================//
		//																					Collision																					  //
		//================================================================================================================================================================================//

		if (collision(allAsteroids[i]->hitbox, spaceshipHitbox)){
			SharedData::GetInstance()->SD_hullIntegrity--;
			SharedData::GetInstance()->SD_bitcoins++;

			if (accelerateShip == 0){
				allAsteroids[i]->hitbox.m_velocity = -(allAsteroids[i]->hitbox.m_velocity);			//Asteroids bounce off ship if it's not moving
			}else{
				allAsteroids[i]->hitbox.m_velocity = (*noseOfShip) * (accelerateShip * 0.5) * dt;	//Asteroids are dragged along the path of the ship because the ship passes ONLY HALF it's velocity onto the asteroid upon collison
				allAsteroids[i]->hitbox.m_origin += *noseOfShip;									//Get the astroid to move in the same direction as the ship upon collision (after passing the ship's velocity)

				if (allAsteroids[i]->count){//bug: need to delete the asteroid completely			//If the asteroid still has any minerals in it
					allAsteroids[i]->count--;
				}
				else{
					//allAsteroids[i]->hitbox.m_origin = Vector3(100000000, 100000000, 1000000);		//move asteroid out of worldspace. supposed to delete this
					//allAsteroids[i]->hitbox.m_velocity = Vector3(0, 0, 0);
					//allAsteroids.erase(i);
					delete allAsteroids[i];	//need to erase and fix the vector before deleting
				}
			}
		}

		for (int a = 0; a < allAsteroids.size(); a++){
			if (a == i){	//Don't check for asteroid collision with itself (obviously)
				break;
			}
			else if (collision(allAsteroids[a]->hitbox, allAsteroids[i]->hitbox)){

				allAsteroids[a]->hitbox.m_velocity = allAsteroids[i]->hitbox.m_velocity;
				allAsteroids[i]->hitbox.m_velocity = allAsteroids[a]->hitbox.m_velocity;

			}
		}

	}

	//================================================================================================================================================================================//
	//																				Standard Keypress Stuff																			  //
	//================================================================================================================================================================================//

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

	//================================================================================================================================================================================//
	//																			Ship interior/exterior																				  //
	//================================================================================================================================================================================//
	std::cout << SharedData::GetInstance()->SD_enableinteract << std::endl;
	*middleOfShip += (*noseOfShip) * accelerateShip * dt;	//Move ship in a direction(Nose is a directional vector)

	if (Application::IsKeyPressed(VK_SPACE)){
		switch (CURRENT_STATE){
			case PILOTING:
				inShip = true;
				inShipCamera.position = *middleOfShip;
				CURRENT_STATE = IN_SHIP;
				break;
			case IN_SHIP:
				inShip = false;
				CURRENT_STATE = PILOTING;
				break;
		}
	}


	switch (CURRENT_STATE){
		case PILOTING:
			shipCamera.target = *middleOfShip;
			shipCamera.Update(dt);
			updateShipMovement(dt);
			break;

		case IN_SHIP:
			inShipCamera.position.y = 0;									//Player height
			inShipCamera.position += (*noseOfShip) * accelerateShip * dt;	//Movement
			tempPosition = inShipCamera.position;
			inShipCamera.Update(dt);
			break;
	}

	//================================================================================================================================================================================//
	//																			Planet Landing																						  //
	//================================================================================================================================================================================//


	//Planet interaction/docking
	if (SharedData::GetInstance()->SD_enableinteract)
	{
		if (((*middleOfShip - (Vector3(250, 250, 0))).Length()) < 100){			//for planet A - sean's planet - ????????
			land = true;
			nameOfPlanet = "sean's planet";

			if (Application::IsKeyPressed('E')){
				//SharedData::GetInstance()->location = 69; //Does not fucking exist 
				//SharedData::GetInstance()->SD_enableinteract = false;
			//	return;
			}
		}
		else if (((*middleOfShip - (Vector3(250, 0, 250))).Length()) < 100){	//for planet B - YueXian's planet - Toga
			land = true;
			nameOfPlanet = "Toga";
		
			if (Application::IsKeyPressed('E'))
			{
				SharedData::GetInstance()->SD_enableinteract = false;
				SharedData::GetInstance()->SD_location = PLANET_TOGA;
				
			}
		}
		else if (((*middleOfShip - (Vector3(-250, 0, -250))).Length()) < 100){	//for planet C - ShuYu's planet - Jelly Planet
			land = true;
			nameOfPlanet = "Jelly Planet";

			if (Application::IsKeyPressed('E'))
			{
				SharedData::GetInstance()->SD_enableinteract = false;
				SharedData::GetInstance()->SD_location = PLANET_JELLY;
				return;
			}
		}
		else{
			land = false;
			
		}
	}
	resetKey();
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

	//Change viewstack for each camera
	
	switch (CURRENT_STATE){
	case PILOTING:
		viewStack.LookAt(shipCamera.position.x, shipCamera.position.y, shipCamera.position.z,
			shipCamera.target.x, shipCamera.target.y, shipCamera.target.z,
			shipCamera.up.x, shipCamera.up.y, shipCamera.up.z);
		break;

	case IN_SHIP:
		viewStack.LookAt(inShipCamera.position.x, inShipCamera.position.y, inShipCamera.position.z,
			inShipCamera.target.x, inShipCamera.target.y, inShipCamera.target.z,
			inShipCamera.up.x, inShipCamera.up.y, inShipCamera.up.z);
		break;
	}

	//Sun Directional light
	modelStack.LoadIdentity();
	Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
	glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);

	//Ship light
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

	//For light in ship
	modelStack.PushMatrix();
	modelStack.Translate(light[1].position.x, light[1].position.y, light[1].position.z);
	RenderMesh(meshList[PLANET_SUN], false);
	modelStack.PopMatrix();

	//Skybox
	modelStack.PushMatrix();
	modelStack.Translate(middleOfShip->x, middleOfShip->y, middleOfShip->z);	//toggle for if skybox moves with skybox
	generateSkybox();
	modelStack.PopMatrix();

	//================================================================================================================================================================================//
	//																				Asteroids																						  //
	//================================================================================================================================================================================//

	//Render Asteroids
	for (unsigned i = 0; i < allAsteroids.size(); i++){
			modelStack.PushMatrix();
			modelStack.Translate(allAsteroids[i]->hitbox.m_origin.x, allAsteroids[i]->hitbox.m_origin.y, allAsteroids[i]->hitbox.m_origin.z);	//Move asteroids to their positions
			modelStack.Scale(allAsteroids[i]->length, allAsteroids[i]->height, allAsteroids[i]->width);											//Give them their appearance
			RenderMesh(meshList[ASTEROIDS], true);
			modelStack.PopMatrix();

			//Render hitbox for Astroid
			modelStack.PushMatrix();
			modelStack.Translate(
				(allAsteroids[i]->hitbox.m_origin.x),
				(allAsteroids[i]->hitbox.m_origin.y),
				(allAsteroids[i]->hitbox.m_origin.z)
				);
			modelStack.Scale(
				(allAsteroids[i]->hitbox.m_length),
				(allAsteroids[i]->hitbox.m_height),
				(allAsteroids[i]->hitbox.m_width)
				);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//set to line for line axis
			RenderMesh(meshList[GEO_LIGHTCUBE], false);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	//set back to fill
			modelStack.PopMatrix();
			//Render hitbox for Astroid end
	}

	//================================================================================================================================================================================//
	//																				Ship Stuff																						  //
	//================================================================================================================================================================================//
	//Spaceship
	modelStack.PushMatrix();
	modelStack.PushMatrix();
	//modelStack.Translate(middleOfShip->x, middleOfShip->y, middleOfShip->z);
	modelStack.Rotate(rotateShip, 0, 1, 0);
	modelStack.Rotate(rotateShipZ, 0, 0, 1);

	switch (CURRENT_STATE){
		case PILOTING:
			RenderMesh(meshList[SPACESHIP], true);
			//Particle effects

			for (int i = 0; i < accelerateShip * 3; i++){
				modelStack.PushMatrix();
				modelStack.Translate(Math::RandFloatMinMax(-10, -15), Math::RandFloatMinMax(-2, 2), Math::RandFloatMinMax(-8, 8));
				RenderMesh(meshList[GEO_LIGHTCUBE], false);
				modelStack.PopMatrix();
			}
			break;

		case IN_SHIP:
			modelStack.Rotate(90, 0, 1, 0);
			modelStack.Scale(10, 10, 10);
			RenderMesh(meshList[SPACESHIP_INTERIOR], true);
			break;
	}

	modelStack.PopMatrix();
	modelStack.PopMatrix();

	//================================================================================================================================================================================//
	//																				Hitbox																							  //
	//================================================================================================================================================================================//

	//Generate AABB from origin for spaceship
	modelStack.PushMatrix();
	modelStack.Translate(
		(spaceshipHitbox.m_origin.x),
		(spaceshipHitbox.m_origin.y),
		(spaceshipHitbox.m_origin.z)
		);
	modelStack.Scale(
		(spaceshipHitbox.m_length),
		(spaceshipHitbox.m_height),
		(spaceshipHitbox.m_width)
		);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//set to line for line axis
	RenderMesh(meshList[GEO_LIGHTCUBE], false);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	//set back to fill
	modelStack.PopMatrix();

	//visual test shit
	modelStack.PushMatrix();
	modelStack.Translate(
		(shipInterior.m_origin.x),
		(shipInterior.m_origin.y),
		(shipInterior.m_origin.z)
		);
	modelStack.Scale(
		shipInterior.m_length,
		shipInterior.m_height, 
		shipInterior.m_width
		);
	//modelStack.Rotate(rotateShip, 0, 1, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//set to line for line axis
	RenderMesh(meshList[GEO_LIGHTCUBE], false);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	//set back to fill
	modelStack.PopMatrix();
	//================================================================================================================================================================================//
	//																				Planets																							  //
	//================================================================================================================================================================================//
	//Planet A
	modelStack.PushMatrix();
	modelStack.Translate(250, 250, 0);
	modelStack.Scale(50, 50, 50);
	RenderMesh(meshList[PLANET_A], false);
	modelStack.Translate(-1.5, 1, 0);
	RenderText(meshList[GEO_TEXT], "Planet A", Color(1, 0, 0));
	modelStack.PushMatrix();
	modelStack.PopMatrix();

	modelStack.Translate(
		(planetA_Hitbox.m_origin.x),
		(planetA_Hitbox.m_origin.y),
		(planetA_Hitbox.m_origin.z)
		);
	modelStack.Scale(
		(planetA_Hitbox.m_length),
		(planetA_Hitbox.m_height),
		(planetA_Hitbox.m_width)
		);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//set to line for line axis
	RenderMesh(meshList[GEO_LIGHTCUBE], false);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	//set back to fill
	modelStack.PopMatrix();

	//Planet B
	modelStack.PushMatrix();
	modelStack.Translate(250, 0, 250);
	modelStack.Scale(50, 50, 50);
	RenderMesh(meshList[PLANET_B], false);
	modelStack.Translate(-1.5, 1, 0);
	RenderText(meshList[GEO_TEXT], "Planet B", Color(1, 0, 0));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(
		(planetB_Hitbox.m_origin.x),
		(planetB_Hitbox.m_origin.y),
		(planetB_Hitbox.m_origin.z)
		);
	modelStack.Scale(
		(planetB_Hitbox.m_length),
		(planetB_Hitbox.m_height),
		(planetB_Hitbox.m_width)
		);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//set to line for line axis
	RenderMesh(meshList[GEO_LIGHTCUBE], false);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	//set back to fill
	modelStack.PopMatrix();

	//Planet C
	modelStack.PushMatrix();
	modelStack.Translate(-250, 0, -250);
	modelStack.Scale(50, 50, 50);
	RenderMesh(meshList[PLANET_C], false);
	modelStack.Translate(-1.5, 1, 0);
	RenderText(meshList[GEO_TEXT], "Planet C", Color(1, 0, 0));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(
		(planetC_Hitbox.m_origin.x),
		(planetC_Hitbox.m_origin.y),
		(planetC_Hitbox.m_origin.z)
		);
	modelStack.Scale(
		(planetC_Hitbox.m_length),
		(planetC_Hitbox.m_height),
		(planetC_Hitbox.m_width)
		);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//set to line for line axis
	RenderMesh(meshList[GEO_LIGHTCUBE], false);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	//set back to fill
	modelStack.PopMatrix();

	//Text if ship is within range of landing on planets
	while (land){
		RenderTextOnScreen(meshList[GEO_TEXT], "Press \"E\" to land on " + nameOfPlanet, Color(1, 0, 0), 2, 0.5, 5);
		break;
	}

	//================================================================================================================================================================================//
	//																					HUD																							  //
	//================================================================================================================================================================================//

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

	//Back - done
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 500);
	modelStack.Rotate(-90, 1, 0, 0);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[SKYBOX_Znega], false);
	modelStack.PopMatrix();

	//Left - done
	modelStack.PushMatrix();
	modelStack.Translate(-500, 0, 0);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Rotate(90, 0, 0, 1);
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
		else{
			accelerateShip = -10;
		}
	}
	if (Application::IsKeyPressed('I')){
		if (accelerateShip < 100.0f){	//acceleration capped at 100.0f
			accelerateShip += 1.0f;
		}
		else{
			accelerateShip = 100;
		}
	}
	if (Application::IsKeyPressed('U')){
		middleOfShip->y += 1.f;
		if (rotateShipZ < -5.0f){
			rotateShipZ += 0.5f;
		}
	}
	if (Application::IsKeyPressed('O')){
		middleOfShip->y -= 1.f;
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

void OpenGalaxyScene::resetKey()
{
	if (!Application::IsKeyPressed('E'))
	{
		e_state = 0;
	}

	if (SharedData::GetInstance()->SD_enableinteract == false)
	{
		
		if (timer > delay)
		{
			delay = timer + 1;
			SharedData::GetInstance()->SD_enableinteract = true;
		}

	}
}

//Author: Randall (155106R)
//Updated 22/2/2016 - Randall

//Note to self: We are gonna make the ship interior smaller and of a more regular shape so that the hit box stuff is much better
//Currently we can't do much cause of the irregular shape - so wait for the new interior and fix it

//Okay, so now, I'll just do the drill.

//Do clean up the shipInterior stuff

//Randall, 25/2/2016, 5:09pm
