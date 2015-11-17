#ifndef _DATABASE_H
#define _DATABASE_H

#include <iostream>
#include <mysql/mysql.h>
#include <queue>
#include "query.h"

using namespace std;

class DataBase
{
public:
	DataBase();
	~DataBase();

	void getQuery(const string& sql);
	void getQuery(const string& sql, queue<Query>& submitQueue);

private:
	MYSQL *conn;
	int port;
	string host;
	string user;
	string password;
	string db;
};

#endif