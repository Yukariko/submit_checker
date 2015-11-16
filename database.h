#ifndef _DATABASE_H
#define _DATABASE_H

#include <iostream>
#include <mysql/mysql.h>

using namespace std;

class DataBase
{
public:
	DataBase();
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