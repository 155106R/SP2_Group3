#include "OpenGalaxyScene.h"
#include "GL\glew.h"
#include "MatrixStack.h"
#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"
#include "Utility.h"
#include "LoadTGA.h"
#include "Mtx44.h"

OpenGalaxyScene::OpenGalaxyScene()
{
}

OpenGalaxyScene::~OpenGalaxyScene()
{
}

void OpenGalaxyScene::Init()
{
	//Definations
	LSPEED = 10.0f;

	land = false;

	rotateTextX = 0;
	rotateTextY = 0;
	rotateTextZ = 0;

	rotateShip = 0;
	shipAxisX = 0;
	shipAxisY = 0;
	shipAxisZ = 0;
	noseOfShip = new Vector3(0, 0, 1);
	middleOfShip = new Vector3(shipAxisX, shipAxisY, shipAxisZ);

	for (unsigned i = 0; i < 1000; i++){
		randTranslateX[i] = Math::RandFloatMinMax(-1000, 1000);
		randTranslateY[i] = Math::RandFloatMinMax(-1000, 1000);
		randTranslateZ[i] = Math::RandFloatMinMax(-1000, 1000);

		randScaleX[i] = Math::RandFloatMinMax(5, 10);
		randScaleY[i] = Math::RandFloatMinMax(5, 10);
		randScaleZ[i] = Math::RandFloatMinMax(5, 10);
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
	meshList[PROXY_SPACESHIP] = MeshBuilder::GeneratePyramid("Proxy spaceship", Color(1, 1, 1), 18, 36);
	meshList[PROXY_SPACESHIP]->material.kAmbient.Set(0.5f, 0.5f, 0.5f);
	meshList[PROXY_SPACESHIP]->material.kDiffuse.Set(0.5f, 0.5f, 0.5f);
	meshList[PROXY_SPACESHIP]->material.kSpecular.Set(0.5f, 0.5f, 0.5f);
	meshList[PROXY_SPACESHIP]->material.kShininess = 0.1f;

	//Asteroids
	meshList[ASTEROIDS] = MeshBuilder::GenerateSphere("Asteroids", Color(0.2941, 0.2941, 0.2941), 5, 10); //75, 75, 75 in RGB (Grey)
	meshList[ASTEROIDS]->material.kAmbient.Set(0.5f, 0.5f, 0.5f);
	meshList[ASTEROIDS]->material.kDiffuse.Set(0.5f, 0.5f, 0.5f);
	meshList[ASTEROIDS]->material.kSpecular.Set(0.5f, 0.5f, 0.5f);
	meshList[ASTEROIDS]->material.kShininess = 0.1f;

	meshList[PLANET_A] = MeshBuilder::GenerateOBJ("PlanetA", "OBJ//planet.obj");
	meshList[PLANET_A]->textureID = LoadTGA("Image//planetATexture.tga");

	meshList[PLANET_B] = MeshBuilder::GenerateOBJ("PlanetB", "OBJ//planet.obj");
	meshList[PLANET_B]->textureID = LoadTGA("Image//planetBTexture.tga");

	meshList[PLANET_C] = MeshBuilder::GenerateOBJ("PlanetC", "OBJ//planet.obj");
	meshList[PLANET_C]->textureID = LoadTGA("Image//planetCTexture.tga");

}

void OpenGalaxyScene::Update(double dt)
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
	//update Ship axes
	

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

	/*if (camera.position.x > 250){
		rotateTextX += 1.0f;
	}
	if(camera.position.x < 250){
		rotateTextX -= 1.0f;
	}
	else if (camera.position.y > 250){
		rotateTextY += 1.0f;
	}
	else if (camera.position.y < 250){
		rotateTextY -= 1.0f;
	}
	if (camera.position.z > 0){
		if (camera.position.z > rotateTextY){
			rotateTextY += 1.0f;
		}
	}
	else if (camera.position.z < 0){
		if (camera.position.z < rotateTextY){
			rotateTextY -= 1.0f;
		}
	}*/


	//Camera frozen, for ship movement
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
		*middleOfShip -= *noseOfShip * 10 * dt;//SPEED VALUE
	}
	if (Application::IsKeyPressed('I')){
		*middleOfShip += *noseOfShip * 50 * dt;//SPEED VALUE
	}
	if (Application::IsKeyPressed('U')){
		middleOfShip->y += 0.5f;
	}
	if (Application::IsKeyPressed('O')){
		middleOfShip->y -= 0.5f;
	}



	//Planet interaction/docking
	if (((camera.position - (Vector3(250, 0, 250))).Length()) < 100){	//for planet A
		land = true;

		if (Application::IsKeyPressed('E')){
			//code to land onto planet A here
		}
	}
	else if (((camera.position - (Vector3(250, 250, 0))).Length()) < 100){	//for planet B
		land = true;

		if (Application::IsKeyPressed('E')){
			//code to land onto planet B here
		}
	}
	else if (((camera.position - (Vector3(-250, 0, -250))).Length()) < 100){	//for planet C
		land = true;

		if (Application::IsKeyPressed('E')){
			//code to land onto planet C here
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

	modelStack.LoadIdentity();
	Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
	glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);

	RenderMesh(meshList[GEO_AXES], false);

	modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	modelStack.Scale(50, 50, 50);
	RenderMesh(meshList[PLANET_SUN], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(camera.position.x, camera.position.y, camera.position.z);	//toggle for if skybox moves with skybox
	generateSkybox();
	modelStack.PopMatrix();

	for (unsigned i = 0; i < 1000; i++){
		modelStack.PushMatrix();
		modelStack.Translate(randTranslateX[i], randTranslateY[i], randTranslateZ[i]);
		modelStack.Scale(randScaleX[i], randScaleY[i], randScaleZ[i]);
		RenderMesh(meshList[ASTEROIDS], true);
		modelStack.PopMatrix();
	}

	modelStack.PushMatrix();
	//modelStack.Translate(camera.position.x, camera.position.y - 10, camera.position.z - 20);
	modelStack.Translate(middleOfShip->x, middleOfShip->y, middleOfShip->z);
	modelStack.Rotate(rotateShip, 0, 1, 0);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(5, 5, 5);
	RenderMesh(meshList[PROXY_SPACESHIP], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(250, 250, 0);
	modelStack.Scale(50, 50, 50);
	RenderMesh(meshList[PLANET_A], false);
	modelStack.Translate(-1.5, 1, 0);
	modelStack.Rotate(rotateTextX, 1, 0, 0);
	modelStack.Rotate(rotateTextY, 0, 1, 0);
	modelStack.Rotate(rotateTextZ, 0, 0, 1);
	RenderText(meshList[GEO_TEXT], "Planet A", Color(1, 0, 0));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(250, 0, 250);
	modelStack.Scale(50, 50, 50);
	RenderMesh(meshList[PLANET_B], false);
	modelStack.Translate(-1.5, 1, 0);
	RenderText(meshList[GEO_TEXT], "Planet B", Color(1, 0, 0));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-250, 0, -250);
	modelStack.Scale(50, 50, 50);
	RenderMesh(meshList[PLANET_C], false);
	modelStack.Translate(-1.5, 1, 0);
	RenderText(meshList[GEO_TEXT], "Planet C", Color(1, 0, 0));
	modelStack.PopMatrix();

	if (land){
		RenderTextOnScreen(meshList[GEO_TEXT], "Press \"E\" to land", Color(1, 0, 0), 2, 0.5, 5);
	}

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