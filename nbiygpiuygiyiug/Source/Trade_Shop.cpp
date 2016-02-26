#include"Trade_Shop.h"
#include "SharedData.h"

Shop::Shop() 
{
	Shops TogaShop;//shop [0]
	TogaShop.name = "TogaS MINERAL SHOP";
	TogaShop.PID = 'A';
	Item newGood1;
	newGood1.name = ItemS[1].name; // 1 = FFLINTIAU
	newGood1.ID = ItemS[1].ID;
	newGood1.bitcoin = ItemS[1].bitcoin;
	newGood1.stack = 10;
	newGood1.MaxStack = false;
	TogaShop.GoodS.push_back(newGood1);
	Item newGood2;
	newGood2.name = ItemS[2].name; // 2 = CALCH
	newGood2.ID = ItemS[2].ID;
	newGood2.bitcoin = ItemS[2].bitcoin;
	newGood2.stack = 10;
	newGood2.MaxStack = false;
	TogaShop.GoodS.push_back(newGood2);
	store.push_back(TogaShop);


	//Shops TogaShop2;//shop [1]
	//TogaShop2.name = "TogaS Upgrade SHOP";
	//TogaShop2.PID = 'A';
	//Item newItem1;
	//newItem1.name = "Ship repair";
	//newItem1.ID = 1;
	//newItem1.PID = 0;
	//newItem1.stack = 1;
	//newItem1.bitcoin = 1;
	//TogaShop2.GoodS.push_back(newItem1);
	//store.push_back(TogaShop2);


	Shops JellyShop;
	JellyShop.name = " MINERAL SHOP";
	JellyShop.PID = 'B';
	Item newGood3;
	newGood3.name = ItemS[5].name; // 5 = CALSIWM
	newGood3.ID = ItemS[5].ID;
	newGood3.bitcoin = ItemS[5].bitcoin;
	newGood3.stack = 10;
	newGood3.MaxStack = false;
	JellyShop.GoodS.push_back(newGood3);
	Item newGood4;
	newGood4.name = ItemS[6].name; // 6 = HAEARN
	newGood4.ID = ItemS[6].ID;
	newGood4.bitcoin = ItemS[6].bitcoin;
	newGood4.stack = 10;
	newGood4.MaxStack = false;
	JellyShop.GoodS.push_back(newGood4);
	store.push_back(JellyShop);
}

Shop::~Shop() {}

void Shop::AddStack(int ID, int amount, char PID) // Add stack into shop (push) PID of the shop
{
	int tempCheck = 0; // check shop whatever have this ID item, if no temp check will == GoodS.size()
	for (int x = 0; x < store.size(); x++)
	{
		if (store[x].PID == PID)
		{
			for (int y = 0; y < store[x].GoodS.size(); y++)
			{
				if (store[x].GoodS[y].ID == ID && store[x].GoodS[y].MaxStack != true)
				{
					if ((store[x].GoodS[y].stack + amount) > 99)
					{
						store[x].GoodS[y].stack = 99;
						store[x].GoodS[y].MaxStack = true;
					}
					else store[x].GoodS[y].stack += amount;
				}
				else if (store[x].GoodS[y].ID != ID) tempCheck++;
			}
			if (tempCheck == store[x].GoodS.size())
			{
				GetNewItem(ID, amount, PID);
			}
		}
	}


	
}

void Shop::GetNewItem(int ID, int amount, char PID) // get new item into shop (push) PID of the shop
{

	if (amount > 99) amount = 99;
	for (int i = 0; i < store.size(); i++)
	{
		if (store[i].PID == PID)
		{
			Item newGood1;
			newGood1.name = ItemS[ID - 1].name;
			newGood1.ID = ItemS[ID - 1].ID;
			newGood1.bitcoin = ItemS[ID - 1].bitcoin;
			newGood1.stack = amount;
			if (amount < 99) newGood1.MaxStack = false;
			else newGood1.MaxStack = true;
			store[i].GoodS.push_back(newGood1);
		}
	}
}

void Shop::RemoveStack(int ID, int amount, char PID) // remove stack from shop (pop) PID of the shop
{
	for (int x = 0; x < store.size(); x++)
	{
		if (store[x].PID == PID)
		{
			for (int y = 0; y < store[x].GoodS.size(); y++)
			{
				if (store[x].GoodS[y].ID == ID && store[x].GoodS[y].stack != 0)
				{
					store[x].GoodS[y].MaxStack = false;
					if ((store[x].GoodS[y].stack - amount) < 0)
					{
						store[x].GoodS[y].stack = 0;
					}
					else store[x].GoodS[y].stack -= amount;
				}
			}
		}
	}
}

