#ifndef TRADE_CALCULATION_H
#define TRADE_CALCULATION_H

#include <vector>
#include <string>
#include <iostream>

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::cin;

class TradeCalculation{

public:
	TradeCalculation();
	~TradeCalculation();

	void setItemsList();

	void setBag();

	void setShopList(); 

	void sellItem(int ID , int amount, char PID); // sell item in bag (pop) , PID of the shop

	void getItem(int ID, int amount); // get item into bag (push)

	void buyItem(int ID, int amount); // buy item from shop (push)

private:
	enum Minerals
	{
		ZERO,
		FFLINTIAU,
		CALCH,
		ASID_FFOSFFORING,
		EITHIN,
		CALSIWM,
		HAEARN,
		COPR,
		FFLWORIN,
		AUR,
		IRIDIUM,
		ARIAN,
		OSMIWM,

		NUM_MINERALS
	};

	struct Item {

		string name;
		int ID;
		/*int rarity; */
		char PID; // planet ID (A,B,O) <get from where>
		int stack; //amount
		int bitcoin;
		bool MaxStack; // one stack x99
	};

	vector<Item> ItemS;

	struct Bag {
		int bitcoin;
		bool MaxSlot; //one bag have x3 slot
		vector<Item> Slot;
	};

	Bag Backpack;

	struct Shop {

		string name;
		char PID;  //planet ID(A, B) <shop in that planet>
		vector<Item> GoodS;
	};

	vector<Shop> ShopS;
};

#endif