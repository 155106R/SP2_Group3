#include "TradeCalculation.h"

TradeCalculation::TradeCalculation() {}

TradeCalculation::~TradeCalculation() {}

void TradeCalculation::setItemsList()
{
	// item from planet toga
	Item newItem1;
	newItem1.name = "fflintiau";
	newItem1.ID = 1; //FFLINTIAU,
	newItem1.PID = 'A'; // A & 65
	newItem1.bitcoin = 10;
	ItemS.push_back(newItem1);

	Item newItem2;
	newItem2.name = "calch";
	newItem2.ID = 2; //CALCH
	newItem2.PID = 'A'; // A & 65
	newItem2.bitcoin = 15;
	ItemS.push_back(newItem2);

	Item newItem3;
	newItem3.name = "asid ffosfforig";
	newItem3.ID = 3; //ASID_FFOSFFORING
	newItem3.PID = 'A'; // A & 65
	newItem3.bitcoin = 20;
	ItemS.push_back(newItem3);

	Item newItem4;
	newItem4.name = "eithin";
	newItem4.ID = 4; //EITHIN
	newItem4.PID = 'A'; // A & 65
	newItem4.bitcoin = 25;
	ItemS.push_back(newItem4);

	// item from planet Jelly
	Item newItem5;
	newItem5.name = "calsiwm";
	newItem5.ID = 5; //CALSIWM
	newItem5.PID = 'B'; // B & 66
	newItem5.bitcoin = 10;
	ItemS.push_back(newItem5);

	Item newItem6;
	newItem6.name = "haearn";
	newItem6.ID = 6; //HAEARN
	newItem6.PID = 'B'; // B & 66
	newItem6.bitcoin = 15;
	ItemS.push_back(newItem6);

	Item newItem7;
	newItem7.name = "copr";
	newItem7.ID = 7; //COPR
	newItem7.PID = 'B'; // B & 66
	newItem7.bitcoin = 20;
	ItemS.push_back(newItem7);

	Item newItem8;
	newItem8.name = "fflworin";
	newItem8.ID = 8; //FFLWORIN
	newItem8.PID = 'B'; // B & 66
	newItem8.bitcoin = 25;
	ItemS.push_back(newItem8);

	// item from Asteroid mining
	Item newItem9;
	newItem9.name = "aur";
	newItem9.ID = 9; //AUR
	newItem9.PID = 'O'; // O & 79
	newItem9.bitcoin = 20;
	ItemS.push_back(newItem9);

	Item newItem10;
	newItem10.name = "iridium";
	newItem10.ID = 10; //IRIDIUM
	newItem10.PID = 'O'; // O & 79
	newItem10.bitcoin = 30;
	ItemS.push_back(newItem10);

	Item newItem11;
	newItem11.name = "arian";
	newItem11.ID = 11; //ARIAN
	newItem11.PID = 'O'; // O & 79
	newItem11.bitcoin = 40;
	ItemS.push_back(newItem11);

	Item newItem12;
	newItem12.name = "osmiwm";
	newItem12.ID = 12; //OSMIWM
	newItem12.PID = 'O'; // O & 79
	newItem12.bitcoin = 50;
	ItemS.push_back(newItem12);

}

Bag::Bag() {}

Bag::~Bag() {}

void Bag::setBag()
{
	bitcoin = 0;
	int Slots = 3;
	MaxSlot = false;
	for (int i = 0; i < Slots; i++)
	{
		Item newSlot;
		newSlot.name = "";
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

	for (int i = 0; i <= Slots; i++)
	{
		if (Slot[i].ID == ID)
		{
			if (Slot[i].MaxStack == false)
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

	for (int i = 0; i <= Slots; i++)
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

	bitcoin -= (ItemS[ID - 1].bitcoin * amount * 2);
}

void Bag::sellItem(int ID, int amount, char PID) // sell item in bag (pop), PID of the shop
{
	TradeCalculation* tempList;

	RemoveItem(ID, amount);
	
	if (ItemS[ID - 1].PID != PID)
	{
		bitcoin += (ItemS[ID - 1].bitcoin * amount * 2);
	}
	else
	{
		AddStack(ID, amount, PID);
		bitcoin += (ItemS[ID - 1].bitcoin * amount);
	}
}

bool Bag::isEmpty(int slotposition)
{
	if (Slot[slotposition].stack <= 0) return true;
	else return false;
}

void Bag::clearSlot_reLocation()
{
	for (int i = 0; i <= Slots; i++)
	{
		if (isEmpty(i) == true && (i + 1) <= Slots && isEmpty(i + 1) == false)
		{
			Slot[i].name = Slot[i+1].name;
			Slot[i].PID = Slot[i+1].PID;
			Slot[i].ID = Slot[i+1].ID;
			Slot[i].bitcoin = Slot[i+1].bitcoin;
			Slot[i].stack = Slot[i+1].stack;
			Slot[i].MaxStack = Slot[i+1].MaxStack;
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

Shop::Shop() {}

Shop::~Shop() {}

void Shop::setShopList() // item the shop sells
{
	TradeCalculation* tempList;

	TogaShop->name = "MINERAL SHOP";
	TogaShop->PID = 'A';

	Item newGood1;
	newGood1.name = ItemS[1].name; // 1 = FFLINTIAU
	newGood1.ID = ItemS[1].ID;
	newGood1.bitcoin = ItemS[1].bitcoin;
	newGood1.stack = 10;
	newGood1.MaxStack = false;
	TogaShop->GoodS.push_back(newGood1);

	Item newGood2;
	newGood2.name = ItemS[2].name; // 2 = CALCH
	newGood2.ID = ItemS[2].ID;
	newGood2.bitcoin = ItemS[2].bitcoin;
	newGood2.stack = 10;
	newGood2.MaxStack = false;
	TogaShop->GoodS.push_back(newGood2);


	JellyShop->name = "MINERAL SHOP";
	JellyShop->PID = 'B';

	Item newGood3;
	newGood3.name = ItemS[5].name; // 5 = CALSIWM
	newGood3.ID = ItemS[5].ID;
	newGood3.bitcoin = ItemS[5].bitcoin;
	newGood3.stack = 10;
	newGood3.MaxStack = false;
	JellyShop->GoodS.push_back(newGood3);

	Item newGood4;
	newGood4.name = ItemS[6].name; // 6 = HAEARN
	newGood4.ID = ItemS[6].ID;
	newGood4.bitcoin = ItemS[6].bitcoin;
	newGood4.stack = 10;
	newGood4.MaxStack = false;
	JellyShop->GoodS.push_back(newGood4);

}

void Shop::AddStack(int ID, int amount, char PID) // Add stack into shop (push) PID of the shop
{
	int tempCheck = 0; // check shop whatever have this ID item, if no temp check will == GoodS.size()

	if (JellyShop->PID == PID)
	{
		for (int i = 0; i <= JellyShop->GoodS.size(); i++)
		{
			if (JellyShop->GoodS[i].ID == ID && JellyShop->GoodS[i].MaxStack != true)
			{
				if ((JellyShop->GoodS[i].stack + amount) > 99)
				{
					JellyShop->GoodS[i].stack = 99;
					JellyShop->GoodS[i].MaxStack = true;
				}
				else JellyShop->GoodS[i].stack += amount;
			}
			else if (JellyShop->GoodS[i].ID != ID) tempCheck++;
		}
		if (tempCheck == JellyShop->GoodS.size())
		{
			GetNewItem(ID, amount, PID);
		}
	}
	
	if (TogaShop->PID == PID)
	{
		for (int i = 0; i <= TogaShop->GoodS.size(); i++)
		{
			if (TogaShop->GoodS[i].ID == ID && TogaShop->GoodS[i].MaxStack != true)
			{
				if ((TogaShop->GoodS[i].stack + amount) > 99)
				{
					TogaShop->GoodS[i].stack = 99;
					TogaShop->GoodS[i].MaxStack = true;
				}
				else TogaShop->GoodS[i].stack += amount;
			}
			else if (TogaShop->GoodS[i].ID != ID) tempCheck++;
		}
		if (tempCheck == TogaShop->GoodS.size())
		{
			GetNewItem(ID, amount, PID);
		}
	}
}

void Shop::GetNewItem(int ID, int amount, char PID) // get new item into shop (push) PID of the shop
{
	TradeCalculation* tempList;
	if (amount > 99) amount = 99;

	if (JellyShop->PID == PID)
	{
		Item newGood1;
		newGood1.name = ItemS[ID-1].name;
		newGood1.ID = ItemS[ID-1].ID;
		newGood1.bitcoin = ItemS[ID-1].bitcoin;
		newGood1.stack = amount;
		if (amount < 99) newGood1.MaxStack = false;
		else newGood1.MaxStack = true;
		JellyShop->GoodS.push_back(newGood1);
	}

	if (TogaShop->PID == PID)
	{
		Item newGood2;
		newGood2.name = ItemS[ID - 1].name;
		newGood2.ID = ItemS[ID - 1].ID;
		newGood2.bitcoin = ItemS[ID - 1].bitcoin;
		newGood2.stack = amount;
		if (amount < 99) newGood2.MaxStack = false;
		else newGood2.MaxStack = true;
		TogaShop->GoodS.push_back(newGood2);
	}
}

void Shop::RemoveStack(int ID, int amount, char PID) // remove stack from shop (pop) PID of the shop
{
	if (JellyShop->PID == PID)
	{
		for (int i = 0; i <= JellyShop->GoodS.size(); i++)
		{
			if (JellyShop->GoodS[i].ID == ID && JellyShop->GoodS[i].stack != 0)
			{
				JellyShop->GoodS[i].MaxStack = false;
				if ((JellyShop->GoodS[i].stack - amount) < 0)
				{
					JellyShop->GoodS[i].stack = 0;
				}
				else JellyShop->GoodS[i].stack -= amount;
			}
		}
	}
	if (TogaShop->PID == PID)
	{
		for (int i = 0; i <= TogaShop->GoodS.size(); i++)
		{
			if (TogaShop->GoodS[i].ID == ID && TogaShop->GoodS[i].stack != 0)
			{
				TogaShop->GoodS[i].MaxStack = false;
				if ((TogaShop->GoodS[i].stack - amount) < 0)
				{
					TogaShop->GoodS[i].stack = 0;
				}
				else TogaShop->GoodS[i].stack -= amount;
			}
		}
	}
}

