#ifndef _SUBMIT_CHECKER_H
#define _SUBMIT_CHECKER_H

#include <iostream>
#include "database.h"
#include <queue>
#include <thread>
#include <atomic>

using namespace std;

enum {NO, PROB_NO, LANG};
typedef enum {NO_RESULT, READY, ACCEPT, WRONG_ANSWER, COMPILE_ERROR, RUNTIME_ERROR, TIME_LIMIT_EXCEED, MEMORY_LIMIT_EXCEED, OUTPUT_LIMIT_EXCEED, OUTPUT_FORMAT_WRONG, OJ_MISS} result;

class SubmitChecker
{
public:
	void run();
	void searchSubmitQueue();
	void check(const Query& pick);
	void waitJudge(const string& no);
	void createCode(const string& code, const string& lang) const;
private:
	static DataBase db;
	static thread waitj;
	static atomic<bool> globalSwitch;

	queue<Query> submitQueue;

};

#endif