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

void TradeCalculation::setBag()
{
	Backpack.bitcoin = 0;
	Backpack.MaxSlot = false;
	for (int i = 0; i < 3; i++)
	{
		Item newSlot;
		newSlot.name = "";
		newSlot.PID = 0;
		newSlot.ID = 0;
		newSlot.bitcoin = 0;
		newSlot.stack = 0;
		newSlot.MaxStack = false;
		Backpack.Slot.push_back(newSlot);
	}
}

void TradeCalculation::setShopList() // item the shop sells
{
	Shop newShop1;
	newShop1.name = "MINERAL SHOP";
	newShop1.PID = 'A';

		Item newGood1;
		newGood1.name = ItemS[1].name; // 1 = FFLINTIAU
		newGood1.ID = ItemS[1].ID;
		newGood1.bitcoin = ItemS[1].bitcoin;
		newGood1.stack = 10;
		newShop1.GoodS.push_back(newGood1);

		Item newGood2;
		newGood2.name = ItemS[2].name; // 2 = CALCH
		newGood2.ID = ItemS[2].ID;
		newGood2.bitcoin = ItemS[2].bitcoin;
		newGood2.stack = 10;
		newShop1.GoodS.push_back(newGood2);

	ShopS.push_back(newShop1);

	Shop newShop2;
	newShop2.name = "MINERAL SHOP";
	newShop2.PID = 'B';

		Item newGood3;
		newGood3.name = ItemS[5].name; // 5 = CALSIWM
		newGood3.ID = ItemS[5].ID;
		newGood3.bitcoin = ItemS[5].bitcoin;
		newGood3.stack = 10;
		newShop2.GoodS.push_back(newGood3);

		Item newGood4;
		newGood4.name = ItemS[6].name; // 6 = HAEARN
		newGood4.ID = ItemS[6].ID;
		newGood4.bitcoin = ItemS[6].bitcoin;
		newGood4.stack = 10;
		newShop2.GoodS.push_back(newGood4);

	ShopS.push_back(newShop2);

}

void TradeCalculation::sellItem(int ID, int amount, char PID) // sell item in bag, PID of the shop
{
	int temp;
	int tempBitcoin;

	if (Backpack.Slot[0].ID == ID) temp = 0;
	else if (Backpack.Slot[1].ID == ID) temp = 1;
	else if (Backpack.Slot[2].ID == ID) temp = 1;

	if (Backpack.Slot[temp].stack >= amount)
	{
		if (Backpack.Slot[temp].PID == PID) tempBitcoin = amount * Backpack.Slot[temp].bitcoin;
		else tempBitcoin = amount * Backpack.Slot[temp].bitcoin * 2;

		Backpack.bitcoin += tempBitcoin;
		Backpack.Slot[temp].stack -= amount;
	}
}

void TradeCalculation::getItem(int ID, int amount) // get item into bag
{
	int temp;
	int tempAmount;

	if (Backpack.Slot[0].ID == ID || Backpack.Slot[0].ID == 0) temp = 0;
	else if (Backpack.Slot[1].ID == ID || Backpack.Slot[1].ID == 0) temp = 1;
	else if (Backpack.Slot[2].ID == ID || Backpack.Slot[2].ID == 0) temp = 2;
	
	if (Backpack.Slot[temp].MaxStack == false)
	{
		Backpack.Slot[temp].stack += amount;
	}


}

void TradeCalculation::buyItem(int ID, int amount) // buy item from shop
{


}