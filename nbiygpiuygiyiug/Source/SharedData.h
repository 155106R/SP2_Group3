#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <map>

using std::map;

enum Minerals{
	ADAMANTIUM,
	ROCK2,

	NUM_ROCKS
};

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

	map<Minerals, int> inventory;

	int SD_hullIntegrity = 100;
	int SD_bitcoins = 100;

	unsigned SD_location;

	void SD_generateHUD(){

	}

private:
	//SharedData(){};

};


#endif