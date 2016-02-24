#include"Trade_Shop.h"

Shop::Shop() {}

Shop::~Shop() {}

void Shop::setShopList() // item the shop sells
{
	Shop JellyShop;
	Shop TogaShop;

	TogaShop.name = "MINERAL SHOP";
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


	JellyShop.name = "MINERAL SHOP";
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

}

void Shop::AddStack(int ID, int amount, char PID) // Add stack into shop (push) PID of the shop
{
	Shop JellyShop;
	Shop TogaShop;

	int tempCheck = 0; // check shop whatever have this ID item, if no temp check will == GoodS.size()

	if (JellyShop.PID == PID)
	{
		for (int i = 0; i <= JellyShop.GoodS.size(); i++)
		{
			if (JellyShop.GoodS[i].ID == ID && JellyShop.GoodS[i].MaxStack != true)
			{
				if ((JellyShop.GoodS[i].stack + amount) > 99)
				{
					JellyShop.GoodS[i].stack = 99;
					JellyShop.GoodS[i].MaxStack = true;
				}
				else JellyShop.GoodS[i].stack += amount;
			}
			else if (JellyShop.GoodS[i].ID != ID) tempCheck++;
		}
		if (tempCheck == JellyShop.GoodS.size())
		{
			GetNewItem(ID, amount, PID);
		}
	}

	if (TogaShop.PID == PID)
	{
		for (int i = 0; i <= TogaShop.GoodS.size(); i++)
		{
			if (TogaShop.GoodS[i].ID == ID && TogaShop.GoodS[i].MaxStack != true)
			{
				if ((TogaShop.GoodS[i].stack + amount) > 99)
				{
					TogaShop.GoodS[i].stack = 99;
					TogaShop.GoodS[i].MaxStack = true;
				}
				else TogaShop.GoodS[i].stack += amount;
			}
			else if (TogaShop.GoodS[i].ID != ID) tempCheck++;
		}
		if (tempCheck == TogaShop.GoodS.size())
		{
			GetNewItem(ID, amount, PID);
		}
	}
}

void Shop::GetNewItem(int ID, int amount, char PID) // get new item into shop (push) PID of the shop
{
	Shop JellyShop;
	Shop TogaShop;

	if (amount > 99) amount = 99;

	if (JellyShop.PID == PID)
	{
		Item newGood1;
		newGood1.name = ItemS[ID - 1].name;
		newGood1.ID = ItemS[ID - 1].ID;
		newGood1.bitcoin = ItemS[ID - 1].bitcoin;
		newGood1.stack = amount;
		if (amount < 99) newGood1.MaxStack = false;
		else newGood1.MaxStack = true;
		JellyShop.GoodS.push_back(newGood1);
	}

	if (TogaShop.PID == PID)
	{
		Item newGood2;
		newGood2.name = ItemS[ID - 1].name;
		newGood2.ID = ItemS[ID - 1].ID;
		newGood2.bitcoin = ItemS[ID - 1].bitcoin;
		newGood2.stack = amount;
		if (amount < 99) newGood2.MaxStack = false;
		else newGood2.MaxStack = true;
		TogaShop.GoodS.push_back(newGood2);
	}
}

void Shop::RemoveStack(int ID, int amount, char PID) // remove stack from shop (pop) PID of the shop
{
	Shop JellyShop;
	Shop TogaShop;

	if (JellyShop.PID == PID)
	{
		for (int i = 0; i <= JellyShop.GoodS.size(); i++)
		{
			if (JellyShop.GoodS[i].ID == ID && JellyShop.GoodS[i].stack != 0)
			{
				JellyShop.GoodS[i].MaxStack = false;
				if ((JellyShop.GoodS[i].stack - amount) < 0)
				{
					JellyShop.GoodS[i].stack = 0;
				}
				else JellyShop.GoodS[i].stack -= amount;
			}
		}
	}
	if (TogaShop.PID == PID)
	{
		for (int i = 0; i <= TogaShop.GoodS.size(); i++)
		{
			if (TogaShop.GoodS[i].ID == ID && TogaShop.GoodS[i].stack != 0)
			{
				TogaShop.GoodS[i].MaxStack = false;
				if ((TogaShop.GoodS[i].stack - amount) < 0)
				{
					TogaShop.GoodS[i].stack = 0;
				}
				else TogaShop.GoodS[i].stack -= amount;
			}
		}
	}
}

