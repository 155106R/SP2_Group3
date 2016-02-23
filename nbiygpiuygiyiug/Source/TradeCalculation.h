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

class TradeCalculation
{

public:
	TradeCalculation();
	~TradeCalculation();

	void setItemsList();

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

	//struct Bag {
	//	int bitcoin;
	//	bool MaxSlot; //one bag have x3 slot
	//	vector<Item> Slot;
	//};
	//Bag Backpack;

};

class Shop : public TradeCalculation
{

public:
	Shop();
	~Shop();
	void setShopList();
	void AddStack(int ID, int amount, char PID);
	void GetNewItem(int ID, int amount, char PID); // get new item into shop (push)
	void RemoveStack(int ID, int amount, char PID); // remove stack from shop (pop)

private:
	Shop* JellyShop;
	Shop* TogaShop;
	string name;
	char PID;  //planet ID(A, B) <shop in that planet>
	vector<Item> GoodS;


};

class Bag :public Shop
{
public:
	Bag();
	~Bag();
	void setBag();
	void GetItem(int ID, int amount); // get item into bag (push)
	void RemoveItem(int ID, int amount); // remove item from bag (pop)
	void buyItem(int ID, int amount, char PID); // buy item from shop (push), PID of the shop
	void sellItem(int ID, int amount, char PID); // sell item in bag (pop), PID of the shop
	bool isEmpty(int slotposition); // check slot whatever slot empty
	void clearSlot_reLocation(); // clear Detail in the slot && so no empting slot in between

private:
	int bitcoin;
	int Slots; // init the slot you want
	bool MaxSlot; 
	vector<Item> Slot;
};

#endif