#include"Trade_Bag.h"
#include "SharedData.h"

Bag::Bag() {}

Bag::~Bag() {}

void Bag::setBag()
{
	
	Slots = 3;
	MaxSlot = false;
	for (int i = 0; i < Slots; i++)
	{
		Item newSlot;
		newSlot.name = "NO ITEM";
		newSlot.PID = 0;
		newSlot.ID = 0;
		newSlot.bitcoin = 0;
		newSlot.stack = 0;
		newSlot.MaxStack = false;
		Slot.push_back(newSlot);
	}
}

void Bag::GetItem(int ID, int amount) // get item into bag (push)
{
	int tempAmount = amount;

	for (int i = 0; i < Slots; i++)
	{
		if (Slot[i].ID == ID && Slot[i].MaxStack == false)
		{
			tempAmount += Slot[i].stack;
			if (tempAmount <= 99)
			{
				Slot[i].stack = tempAmount;
				break;
			}
			else
			{
				tempAmount -= 99;
				Slot[i].stack = 99;
				Slot[i].MaxStack = true;
			}
		}
		else if (isEmpty(i) == true)
		{
			Slot[i].name = ItemS[ID - 1].name;
			Slot[i].PID = ItemS[ID - 1].PID;
			Slot[i].bitcoin = ItemS[ID - 1].bitcoin;
			Slot[i].ID = ID;
			if (tempAmount <= 99)
			{
				Slot[i].stack = tempAmount;
				break;
			}
			else
			{
				tempAmount -= 99;
				Slot[i].stack = 99;
				Slot[i].MaxStack = true;
			}
		}
	}

}

void Bag::RemoveItem(int ID, int amount) // remove item from bag (pop)
{
	int tempAmount = amount;

	for (int i = 0; i < Slots; i++)
	{
		if (Slot[i].ID == ID)
		{
			Slot[i].stack -= tempAmount;
			Slot[i].MaxStack = false;

			if (Slot[i].stack < 0)
			{
				tempAmount = 0 - Slot[i].stack;
			}
			else break;
		}
	}
	clearSlot_reLocation();
}

void Bag::buyItem(int ID, int amount, char PID) // buy item from shop (push), PID of the shop
{
	TradeCalculation* tempList;

	GetItem(ID, amount);
	RemoveStack(ID, amount, PID);

	SharedData::GetInstance()->SD_bitcoins -= (ItemS[ID - 1].bitcoin * amount * 2);
}

void Bag::sellItem(int ID, int amount, char PID) // sell item in bag (pop), PID of the shop
{
	TradeCalculation* tempList;

	RemoveItem(ID, amount);

	if (ItemS[ID - 1].PID != PID)
	{
		SharedData::GetInstance()->SD_bitcoins += (ItemS[ID - 1].bitcoin * amount * 2);
	}
	else
	{
		AddStack(ID, amount, PID);
		SharedData::GetInstance()->SD_bitcoins += (ItemS[ID - 1].bitcoin * amount);
	}
}

bool Bag::isEmpty(int slotposition)
{
	if (Slot[slotposition].stack <= 0) return true;
	else return false;
}

void Bag::clearSlot_reLocation()
{
	for (int i = 0; i < Slots; i++)
	{
		if (isEmpty(i) == true && (i + 1) <= Slots && isEmpty(i + 1) == false)
		{
			Slot[i].name = Slot[i + 1].name;
			Slot[i].PID = Slot[i + 1].PID;
			Slot[i].ID = Slot[i + 1].ID;
			Slot[i].bitcoin = Slot[i + 1].bitcoin;
			Slot[i].stack = Slot[i + 1].stack;
			Slot[i].MaxStack = Slot[i + 1].MaxStack;
		}
		else if (isEmpty(i) == true)
		{
			Slot[i].name = "";
			Slot[i].PID = 0;
			Slot[i].ID = 0;
			Slot[i].bitcoin = 0;
			Slot[i].stack = 0;
		}
	}
}