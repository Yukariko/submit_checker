#ifndef _SUBMIT_CHECKER_H
#define _SUBMIT_CHECKER_H

#include <iostream>
#include "database.h"
#include <queue>

using namespace std;

#define 

class SubmitChecker
{
public:
	void run();
	void searchSubmitQueue();
	void check(const Query& pick);


private:
	DateBase db;
	queue<Query> submitQueue;
};

#endif