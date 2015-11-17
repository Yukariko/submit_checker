#include "query.h"

Query::Query(const MYSQL_ROW row)
{
	for(int i=0; row[i]; i++)
		results.push_back(string(row[i], mysql_fetch_lengths(row[i])));
}

const string& Query::getResult(int idx) const
{
	return results[idx];
}