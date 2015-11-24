#include "query.h"

Query::Query(const MYSQL_ROW row, int num)
{
	// row를 string으로 변환하여 일괄 저장
	for(int i=0; i < num; i++)
		results.push_back(string(row[i]));
}

const string& Query::getResult(int idx) const
{
	return results[idx];
}