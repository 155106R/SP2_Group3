#ifndef SHARED_DATA_H
#define SHARED_DATA_H
#include <map>

using std::map;


class SharedData{

private:
	SharedData();

public:

	enum Minerals{
		Adamantium,
		rock2,

		num_rocks
	};

	map<Minerals, int> inventory;

	SharedData* Getinstance()
	{
		static SharedData data;
		//inventory[rock1] += 10;

		return &data;
	}

	int SD_hullintegrity;
	
	int SD_bitcoins;




};










#endif