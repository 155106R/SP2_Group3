#ifndef OPEN_GALAXY_SCENE_H
#define OPEN_GALAXY_SCENE_H

#include <vector>
#include <string>
#include <fstream>

using std::string;
using std::vector;
using std::ifstream;

#include "Scene.h"
#include "FPCamera.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"

class OpenGalaxyScene : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_LIGHTBALL,
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

		ASTEROIDS,

		GEO_TEXT,

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
	OpenGalaxyScene();
	~OpenGalaxyScene();

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

	bool land;
	float rotateTextX, rotateTextY, rotateTextZ;

	float randScaleX[1000];
	float randScaleY[1000];
	float randScaleZ[1000];
	float randTranslateX[1000];
	float randTranslateY[1000];
	float randTranslateZ[1000];

	//For Light
	bool enableLight;

	FPCamera camera;
	Light light[1];

	Mesh *meshList[NUM_GEOMETRY];
};

#endif

//Author: Randall (155106R)
