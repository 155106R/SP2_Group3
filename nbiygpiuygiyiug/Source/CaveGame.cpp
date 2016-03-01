#include "CaveGame.h"
#include "MyMath.h"

CaveGame::CaveGame()
{
	Board_P = 20;
	for (int i = 0; i < 36; i++)
	{
		falling_rocks newRock;
		newRock.Speed = 0;
		newRock.P_X = i + 2;
		newRock.P_Y = 28;
		newRock.score_ID = 0;
		newRock.score_amount = 0;
		newRock.state = 0;
		newRock.scored = 0;
		Rocks.push_back(newRock);
	}

	for (int i = 0; i < 4; i++)
	{
		Item newScore;
		newScore.name = "NO ITEM";
		newScore.PID = 0;
		newScore.ID = 0;
		newScore.bitcoin = 0;
		newScore.stack = 0;
		newScore.MaxStack = false;
		ScoreS.push_back(newScore);
	}
}

CaveGame::~CaveGame()
{
}

void CaveGame::randSetRocks(char PID)
{
	for (int i = 0; i <36; i++)
	{
		Rocks[i].Speed = Math::RandFloatMinMax(1, 10);
		if (PID == 'A')
		{
			Rocks[i].score_ID = Math::RandIntMinMax(1, 4);
		}
		else if (PID == 'B')
		{
			Rocks[i].score_ID = Math::RandIntMinMax(5, 8);
		}
		Rocks[i].score_amount = Math::RandIntMinMax(1, 3);
		cout << Rocks[i].score_ID << endl;
	}
}
void CaveGame::startFalling(double dt, char PID)
{
	for (int i = 0; i < 36; i++)
	{
		if (Math::RandIntMinMax(0, 10) > 7) Rocks[i].state = 1;
		if (Rocks[i].state = 1) Rocks[i].P_Y -= Rocks[i].Speed * dt;
		if (Rocks[i].P_Y <= 2.5 && Rocks[i].P_Y >= 1.5 && Rocks[i].P_X <= Board_P + 1.4 && Rocks[i].P_X >= Board_P - 1.4)
		{
			addScore(Rocks[i].score_ID, Rocks[i].score_amount);
			reRandRocks(i, PID);
		}
		else if (Rocks[i].P_Y <= 2 && Rocks[i].P_Y >= 1) reRandRocks(i, PID);

	}
}
void CaveGame::reRandRocks(int i, char PID)
{
	Rocks[i].P_Y = 28;
	Rocks[i].state = 0;
	Rocks[i].Speed = Math::RandFloatMinMax(1, 10);
	if (PID == 'A')
	{
		Rocks[i].score_ID = Math::RandIntMinMax(1, 4);  
	}
	if (PID == 'B')
	{
		Rocks[i].score_ID = Math::RandIntMinMax(5, 8);
	}
	Rocks[i].score_amount = Math::RandIntMinMax(1, 3);
}
void CaveGame::addScore(int ID, int amount)
{
	int tempAmount = amount;

	for (int i = 0; i < ScoreS.size(); i++)
	{
		if (ScoreS[i].ID == ID && ScoreS[i].MaxStack == false)
		{
			tempAmount += ScoreS[i].stack;
			if (tempAmount <= 99)
			{
				ScoreS[i].stack = tempAmount;
				break;
			}
			else
			{
				tempAmount -= 99;
				ScoreS[i].stack = 99;
				ScoreS[i].MaxStack = true;
			}
		}
		else if (ScoreS[i].stack <=0 )
		{
			ScoreS[i].name = ItemS[ID - 1].name;
			ScoreS[i].PID = ItemS[ID - 1].PID;
			ScoreS[i].bitcoin = ItemS[ID - 1].bitcoin;
			ScoreS[i].ID = ID;
			if (tempAmount <= 99)
			{
				ScoreS[i].stack = tempAmount;
				break;
			}
			else
			{
				tempAmount -= 99;
				ScoreS[i].stack = 99;
				ScoreS[i].MaxStack = true;
			}
		}
	}
}

void CaveGame::reSetScore()
{
	for (int i = 0; i < 4; i++)
	{
		ScoreS[i].name = "NO ITEM";
		ScoreS[i].PID = 0;
		ScoreS[i].ID = 0;
		ScoreS[i].bitcoin = 0;
		ScoreS[i].stack = 0;
		ScoreS[i].MaxStack = false;
	}
}