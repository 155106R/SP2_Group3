#include"Trade_Shop.h"
#include "SharedData.h"


Shop::Shop() 
{
	Shops TogaShop;//shop [0]
	TogaShop.name = "TOGA MINERAL SHOP";
	TogaShop.SID = 'A';
	Item newGood1;
	newGood1.name = ItemS[0].name; // FFLINTIAU
	newGood1.ID = ItemS[0].ID;
	newGood1.bitcoin = ItemS[0].bitcoin*2;
	newGood1.stack = 10;
	newGood1.MaxStack = false;
	TogaShop.GoodS.push_back(newGood1);
	Item newGood2;
	newGood2.name = ItemS[1].name; // CALCH
	newGood2.ID = ItemS[1].ID;
	newGood2.bitcoin = ItemS[1].bitcoin*2;
	newGood2.stack = 10;
	newGood2.MaxStack = false;
	TogaShop.GoodS.push_back(newGood2);
	store.push_back(TogaShop);

	Shops JellyShop;//shop [1]
	JellyShop.name = "JELLY MINERAL SHOP";
	JellyShop.SID = 'B';
	Item newGood3;
	newGood3.name = ItemS[4].name; // CALSIWM
	newGood3.ID = ItemS[4].ID;
	newGood3.bitcoin = ItemS[4].bitcoin*2;
	newGood3.stack = 10;
	newGood3.MaxStack = false;
	JellyShop.GoodS.push_back(newGood3);
	Item newGood4;
	newGood4.name = ItemS[5].name; // HAEARN
	newGood4.ID = ItemS[5].ID;
	newGood4.bitcoin = ItemS[5].bitcoin*2;
	newGood4.stack = 10;
	newGood4.MaxStack = false;
	JellyShop.GoodS.push_back(newGood4);
	store.push_back(JellyShop);

	Shops TogaShop2; //shop [2]
	TogaShop2.name = "TOGA UPGRADE SHOP";
	TogaShop.SID = 'C';
	Upgrade newPower0;
	newPower0.name = UpgradeS[0].name;
	newPower0.ID = UpgradeS[0].ID;
	newPower0.bitcoin = UpgradeS[0].bitcoin;
	TogaShop2.PowerS.push_back(newPower0);
	Upgrade newPower1;
	newPower1.name = UpgradeS[1].name;
	newPower1.ID = UpgradeS[1].ID;
	newPower1.bitcoin = UpgradeS[1].bitcoin;
	newPower1.level = UpgradeS[1].level;
	TogaShop2.PowerS.push_back(newPower1);
	store.push_back(TogaShop2);

	Shops Jellyshop2; //shop [3]
	Jellyshop2.name = "JELLY UPGRADE SHOP";
	JellyShop.SID = 'D';
	Upgrade newPower3;
	newPower3.name = UpgradeS[0].name;
	newPower3.ID = UpgradeS[0].ID;
	newPower3.bitcoin = UpgradeS[0].bitcoin;
	Jellyshop2.PowerS.push_back(newPower3);
	Upgrade newPower4;
	newPower4.name = UpgradeS[2].name;
	newPower4.ID = UpgradeS[2].ID;
	newPower4.bitcoin = UpgradeS[2].bitcoin;
	newPower4.level = UpgradeS[2].level;
	Jellyshop2.PowerS.push_back(newPower4);
	store.push_back(Jellyshop2);

	Shops TogaShop3; //shop [4]
	TogaShop3.name = "TOGA DRONE SHOP";
	TogaShop.SID = 'E';
	Drone newRebot0;
	newRebot0.name = DroneS[0].name;
	newRebot0.ID = DroneS[0].ID;
	newRebot0.bitcoin = DroneS[0].bitcoin;
	newRebot0.Sold = DroneS[0].Sold;
	TogaShop3.RebotS.push_back(newRebot0);
	Drone newRebot1;
	newRebot1.name = DroneS[1].name;
	newRebot1.ID = DroneS[1].ID;
	newRebot1.bitcoin = DroneS[1].bitcoin;
	newRebot1.Sold = DroneS[1].Sold;
	TogaShop3.RebotS.push_back(newRebot1);
	store.push_back(TogaShop3);

	Shops Jellyshop3; //shop [5]
	Jellyshop3.name = "JELLY DRONE SHOP";
	JellyShop.SID = 'F';
	Drone newRebot2;
	newRebot2.name = DroneS[0].name;
	newRebot2.ID = DroneS[0].ID;
	newRebot2.bitcoin = DroneS[0].bitcoin;
	newRebot2.Sold = DroneS[0].Sold;
	Jellyshop3.RebotS.push_back(newRebot2);
	Drone newRebot3;
	newRebot3.name = DroneS[1].name;
	newRebot3.ID = DroneS[1].ID;
	newRebot3.bitcoin = DroneS[1].bitcoin;
	newRebot3.Sold = DroneS[1].Sold;
	Jellyshop3.RebotS.push_back(newRebot3);
	store.push_back(Jellyshop3);



}

Shop::~Shop() {}

void Shop::AddStack(int ID, int amount, char SID) // Add stack into shop (push) SID of the shop
{
	int tempCheck = 0; // check shop whatever have this ID item, if no temp check will == GoodS.size()
	for (int x = 0; x < store.size(); x++)
	{
		if (store[x].SID == SID)
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
				GetNewItem(ID, amount, SID);
			}
		}
	}


	
}

void Shop::GetNewItem(int ID, int amount, char SID) // get new item into shop (push) SID of the shop
{

	if (amount > 99) amount = 99;
	for (int i = 0; i < store.size(); i++)
	{
		if (store[i].SID == SID)
		{
			Item newGood1;
			newGood1.name = ItemS[ID - 1].name;
			newGood1.ID = ItemS[ID - 1].ID;
			newGood1.bitcoin = ItemS[ID - 1].bitcoin*2;
			newGood1.stack = amount;
			if (amount < 99) newGood1.MaxStack = false;
			else newGood1.MaxStack = true;
			store[i].GoodS.push_back(newGood1);
		}
	}
}

void Shop::RemoveStack(int ID, int amount, char SID) // remove stack from shop (pop) SID of the shop
{
	for (int x = 0; x < store.size(); x++)
	{
		if (store[x].SID == SID)
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

void Shop::BuyDrone(int num, int SID)
{
	if (store[SID].RebotS[num].ID == 1 && SharedData::GetInstance()->SD_bitcoins >= store[SID].RebotS[num].bitcoin)
	{
		SharedData::GetInstance()->SD_bitcoins -= store[SID].RebotS[num].bitcoin;
		store[4].RebotS[num].Sold = true;
		store[5].RebotS[num].Sold = true;
		SharedData::GetInstance()->SD_RepairDrone = true;
	}
	if (store[SID].RebotS[num].ID == 2 && SharedData::GetInstance()->SD_bitcoins >= store[SID].RebotS[num].bitcoin)
	{

		if (SID == 4)// toga 
		{
			store[SID].RebotS[num].Sold = true;
			SharedData::GetInstance()->SD_bitcoins -= store[SID].RebotS[num].bitcoin;
			SharedData::GetInstance()->SD_MiningDrone_T = true;
		}
		if (SID == 5)// Jelly
		{
			store[SID].RebotS[num].Sold = true;
			SharedData::GetInstance()->SD_bitcoins -= store[SID].RebotS[num].bitcoin;
			SharedData::GetInstance()->SD_MiningDrone_J = true;
		}
	}
}
void Shop::BuyUpgrade(int ID, char SID)
{
	for (int x = 0; x < store.size(); x++)
	{
		if (store[x].SID == SID)
		{
			for (int y = 0; y < store[x].PowerS.size(); y++)
			{
				if (ID == 0 && store[x].PowerS[y].ID == ID && SharedData::GetInstance()->SD_bitcoins >= ((SharedData::GetInstance()->SD_hullmax - (int)(SharedData::GetInstance()->SD_hullIntegrity))*store[x].PowerS[y].bitcoin))
				{
					SharedData::GetInstance()->SD_bitcoins -= ((SharedData::GetInstance()->SD_hullmax - (int)(SharedData::GetInstance()->SD_hullIntegrity))*store[x].PowerS[y].bitcoin);
					SharedData::GetInstance()->SD_hullIntegrity = SharedData::GetInstance()->SD_hullmax;
					break;
				}
				else if (ID == 1 && store[x].PowerS[y].ID == ID && SharedData::GetInstance()->SD_bitcoins >= store[x].PowerS[y].bitcoin)
				{
					store[x].PowerS[y].level++;
					SharedData::GetInstance()->SD_bitcoins -= store[x].PowerS[y].bitcoin;
					SharedData::GetInstance()->SD_hullmax += 10;	
					store[x].PowerS[y].bitcoin = (store[x].PowerS[y].level * 100);		
					break;
				}
				else if (ID == 2 && store[x].PowerS[y].ID == ID && SharedData::GetInstance()->SD_bitcoins >= store[x].PowerS[y].bitcoin)
				{
					store[x].PowerS[y].level++;
					SharedData::GetInstance()->SD_bitcoins -= store[x].PowerS[y].bitcoin;
					SharedData::GetInstance()->PlayerInventory->IncreaseSlots(1);	
					store[x].PowerS[y].bitcoin = (store[x].PowerS[y].level * 1000);
					break;
				}
			}
		}
	}
}



