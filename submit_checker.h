#ifndef _SUBMIT_CHECKER_H
#define _SUBMIT_CHECKER_H

#include <iostream>
#include "database.h"
#include <queue>
#include <thread>
#include <atomic>

using namespace std;

enum {NO, PROB_NO, USER_NO, LANG};

// 채점 결과 목록
typedef enum {EMPTY, NO_RESULT, READY, RUNNING, ACCEPT, WRONG_ANSWER, COMPILE_ERROR, RUNTIME_ERROR, TIME_LIMIT_EXCEED, MEMORY_LIMIT_EXCEED, OUTPUT_LIMIT_EXCEED, OUTPUT_FORMAT_WRONG, OJ_MISS} result;

/*
 * 데이터베이스에서 채점 대기 목록을 가져와 채점을 수행하는 클래스
 */

class SubmitChecker
{
public:
	// 채점 시작
	void run();
	// DB에서 대기중인 채점목록 가져오기
	void searchSubmitQueue();
	// 채점큐에서 하나씩 빼서 채점 수행
	void check(const Query& pick);
	// 채점 진행률 업데이트
	void waitJudge(const string& no);
	// 채점 코드 생성
	void createCode(const string& code, const string& lang) const;
private:
	static DataBase db;
	static thread waitj;
	static atomic<bool> globalSwitch;
	// 채점 목록들이 저장되어있음
	queue<Query> submitQueue;

};

#endif