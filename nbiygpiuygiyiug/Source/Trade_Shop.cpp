#include"Trade_Shop.h"
#include "SharedData.h"

Shop::Shop() {}

Shop::~Shop() {}

void Shop::setShopList() // item the shop sells
{
	
	Shops TogaShop;
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
	ShopS.push_back(TogaShop);

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
	ShopS.push_back(JellyShop);
}

void Shop::AddStack(int ID, int amount, char PID) // Add stack into shop (push) PID of the shop
{
	int tempCheck = 0; // check shop whatever have this ID item, if no temp check will == GoodS.size()
	for (int i = 0; i < ShopS.size(); i++)
	{
		if (ShopS[i].PID == PID)
		{
			for (int i = 0; i < ShopS[i].GoodS.size(); i++)
			{
				if (ShopS[i].GoodS[i].ID == ID && ShopS[i].GoodS[i].MaxStack != true)
				{
					if ((ShopS[i].GoodS[i].stack + amount) > 99)
					{
						ShopS[i].GoodS[i].stack = 99;
						ShopS[i].GoodS[i].MaxStack = true;
					}
					else ShopS[i].GoodS[i].stack += amount;
				}
				else if (ShopS[i].GoodS[i].ID != ID) tempCheck++;
			}
			if (tempCheck == ShopS[i].GoodS.size())
			{
				GetNewItem(ID, amount, PID);
			}
		}
	}


	
}

void Shop::GetNewItem(int ID, int amount, char PID) // get new item into shop (push) PID of the shop
{

	if (amount > 99) amount = 99;
	for (int i = 0; i < ShopS.size(); i++)
	{
		if (ShopS[i].PID == PID)
		{
			Item newGood1;
			newGood1.name = ItemS[ID - 1].name;
			newGood1.ID = ItemS[ID - 1].ID;
			newGood1.bitcoin = ItemS[ID - 1].bitcoin;
			newGood1.stack = amount;
			if (amount < 99) newGood1.MaxStack = false;
			else newGood1.MaxStack = true;
			ShopS[i].GoodS.push_back(newGood1);
		}
	}
}

void Shop::RemoveStack(int ID, int amount, char PID) // remove stack from shop (pop) PID of the shop
{
	for (int i = 0; i < ShopS.size(); i++)
	{
		if (ShopS[i].PID == PID)
		{
			for (int i = 0; i < ShopS[i].GoodS.size(); i++)
			{
				if (ShopS[i].GoodS[i].ID == ID && ShopS[i].GoodS[i].stack != 0)
				{
					ShopS[i].GoodS[i].MaxStack = false;
					if ((ShopS[i].GoodS[i].stack - amount) < 0)
					{
						ShopS[i].GoodS[i].stack = 0;
					}
					else ShopS[i].GoodS[i].stack -= amount;
				}
			}
		}
	}
}

