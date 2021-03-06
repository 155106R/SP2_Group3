#ifndef TRADE_BAG_H
#define TRADE_BAG_H

#include "Trade_Shop.h"

class Bag :public Shop
{
public:
	Bag();
	~Bag();
	void IncreaseSlots(int slots);
	void GetItem(int ID, int amount); // get item into bag (push)
	void RemoveItem(int ID, int amount); // remove item from bag (pop)
	void buyItem(int ID, int amount, char SID, int num); // buy item from shop (push), SID of the shop
	void sellItem(int ID, int amount, char SID); // sell item in bag (pop), SID of the shop
	bool isEmpty(int slotposition); // check slot whatever slot empty
	void clearSlot_reLocation(); // clear Detail in the slot && so no empting slot in between
	bool Slot_Full(int ID, int amount); // check if slot full? and stack full?


	int Slots; // init the slot you want
	bool MaxSlot;
	vector<Item> Slot;
};



#endif