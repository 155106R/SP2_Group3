#ifndef TRADE_CALCULATION_H
#define TRADE_CALCULATION_H

#include <vector>
#include <string>
#include <iostream>

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::cin;

class TradeCalculation{

public:

private:

	struct Item {

		string name;
		int level;
		char PID; // planet ID (A,B,O)
		int amount;
		int gold;
	};

	vector<Item> ItemS;

};

#endif