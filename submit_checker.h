#ifndef _SUBMIT_CHECKER_H
#define _SUBMIT_CHECKER_H

#include <iostream>
#include "database.h"
#include <queue>
#include <thread>
#include <atomic>

using namespace std;

enum {NO, PROB_NO, LANG};

class SubmitChecker
{
public:
	void run();
	void searchSubmitQueue();
	void check(const Query& pick);
	void waitJudge(const string& no);

private:
	static DataBase db;
	static thread waitj;
	static atomic<bool> globalSwitch;

	queue<Query> submitQueue;

};

#endif