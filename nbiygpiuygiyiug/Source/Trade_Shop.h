#ifndef TRADE_SHOP_H
#define TRADE_SHOP_H

#include "TradeCalculation.h"

class Shop : public TradeCalculation
{

public:
	Shop();
	~Shop();
	void AddStack(int ID, int amount, char SID);
	void GetNewItem(int ID, int amount, char SID); // get new item into shop (push)
	void RemoveStack(int ID, int amount, char SID); // remove stack from shop (pop)
	void BuyDrone(int num, int SID);
	void BuyUpgrade(int ID, char SID);


	struct Shops
	{
		string name;
		char SID;  //planet ID(A, B) <shop in that planet>
		vector<Item> GoodS;
		vector<Upgrade> PowerS;
		vector<Drone> RebotS;
	};
	
	vector<Shops> store;

};


#endif