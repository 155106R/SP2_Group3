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
	OPENGALAXY,
	TOGAPLANET,
	JELLYPLANET,

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

	int SD_hullintegrity;
	int SD_bitcoins;

	unsigned location = 0;

private:
	//SharedData(){};

};


#endif