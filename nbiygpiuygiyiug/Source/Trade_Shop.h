#ifndef TRADE_SHOP_H
#define TRADE_SHOP_H

#include "TradeCalculation.h"

class Shop : public TradeCalculation
{

public:
	Shop();
	~Shop();
	void AddStack(int ID, int amount, char PID);
	void GetNewItem(int ID, int amount, char PID); // get new item into shop (push)
	void RemoveStack(int ID, int amount, char PID); // remove stack from shop (pop)


	struct Shops
	{
		string name;
		char PID;  //planet ID(A, B) <shop in that planet>
		vector<Item> GoodS;
	};
	
	vector<Shops> store;

};


#endif