#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "TradeCalculation.h"
#include "Trade_Shop.h"
#include "Trade_Bag.h"

using std::map;
using std::vector;
using std::string;
using std::ifstream;

enum onPlanet{
	OPEN_GALAXY,
	PLANET_TOGA,
	PLANET_JELLY,

	NUM_PLANETS
};

class SharedData{

public:

	static SharedData* GetInstance()
	{
		static SharedData data;
		return &data;
	}

	//PlayerAssests
	int SD_hullIntegrity = 100;
	int SD_bitcoins = 0;
	//PlayerLocation
	unsigned SD_location;
	bool SD_enableinteract = true;

	//stuff to get offset for text character spacing
	std::vector<std::string> fontData = fillWithFontData("PixelFontData.csv");
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

	Bag* PlayerInventory = new Bag;
	
private:
	//SharedData(){};

};


#endif