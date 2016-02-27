#include "TradeCalculation.h"


TradeCalculation::TradeCalculation() 
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

TradeCalculation::~TradeCalculation() 
{}



