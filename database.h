#ifndef _DATABASE_H
#define _DATABASE_H

#include <iostream>
#include <mysql/mysql.h>
#include <queue>
#include "query.h"

using namespace std;

/*
 * 데이터베이스에 연결하고 쿼리를 수행하는 클래스
 */

class DataBase
{
public:
	DataBase();
	~DataBase();

	// update 같은 결과를 가져오지 않는 쿼리 수행
	void getQuery(const string& sql);
	// select 같은 결과가 필요한 쿼리 수행
	void getQuery(const string& sql, queue<Query>& queryQueue);

	// data 경로 반환
	const string& getDataPath() const;

private:
	MYSQL *conn;
	int port;
	string host;
	string user;
	string password;
	string db;
	string dataPath;
};

#endif