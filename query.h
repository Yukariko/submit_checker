#ifndef _QUERY_H
#define _QUERY_H

#include <iostream>
#include <vector>
#include <mysql/mysql.h>

using namespace std;

class Query
{
public:
	Query(const MYSQL_ROW row, int num);

	const string& getResult(int idx) const;

private:
	vector<string> results;

};

#endif