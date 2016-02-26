#include"Trade_Bag.h"
#include "SharedData.h"

Bag::Bag() 
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

Bag::~Bag() {}

void  Bag::IncreaseSlots(int slots)
{
	MaxSlot = false;
	for (int i = 0; i < slots; i++)
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
	Slots += slots;
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

void Bag::buyItem(int ID, int amount, char PID, int num) // buy item from shop (push), PID of the shop
{
	if (SharedData::GetInstance()->SD_bitcoins > (ItemS[ID - 1].bitcoin * amount * 2) && SharedData::GetInstance()->PlayerInventory->store[0].GoodS[num].stack >= amount && Slot_Full(ID,amount) == true)
	{

		GetItem(ID, amount);
		RemoveStack(ID, amount, PID);

		SharedData::GetInstance()->SD_bitcoins -= (ItemS[ID - 1].bitcoin * amount * 2);
	}
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
	clearSlot_reLocation();
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
		if (isEmpty(i) == true && (i + 1) < Slots && isEmpty(i + 1) == false)
		{
			Slot[i].name = Slot[i + 1].name;
			Slot[i].PID = Slot[i + 1].PID;
			Slot[i].ID = Slot[i + 1].ID;
			Slot[i].bitcoin = Slot[i + 1].bitcoin;
			Slot[i].stack = Slot[i + 1].stack;
			Slot[i].MaxStack = Slot[i + 1].MaxStack;
			Slot[i + 1].stack = 0;

		}
		else if (isEmpty(i) == true)
		{
			Slot[i].name = "NO ITEM";
			Slot[i].PID = 0;
			Slot[i].ID = 0;
			Slot[i].bitcoin = 0;
			Slot[i].stack = 0;
		}
	}
}

bool Bag::Slot_Full(int ID, int amount)
{
	for (int i = 0; i < Slot.size(); i++)
	{
		if (isEmpty(i) == true)
		{
			return true;
		}
		else if (Slot[i].ID == ID && 99-Slot[i].stack > amount)
		{
			return true;
		}
	}
	return false;

}