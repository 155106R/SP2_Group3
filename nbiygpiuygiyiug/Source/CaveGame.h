#ifndef CAVEGAME_H
#define CAVEGAME_H

#include "Trade_Bag.h"

class CaveGame :public Bag
{
public:
	CaveGame();
	~CaveGame();

	void randSetRocks(char PID);
	void startFalling(double dt , char PID);
	void reRandRocks(int i, char PID);
	void addScore(int ID, int amount);
	void reSetScore();

	struct falling_rocks
	{
		float Speed;
		float P_Y; // height	
		float P_X; // point to check get score?
		int score_ID;
		int score_amount;
		bool state;
		bool scored;
	};

	vector<falling_rocks> Rocks;

	vector<Item> ScoreS;

	int Board_P; // board position

};

#endif