#ifndef _QUERY_H
#define _QUERY_H

#include <iostream>
#include <vector>
#include <mysql/mysql.h>

using namespace std;

/*
 * 데이터베이스의 한 row의 데이터를 저장하는 클래스
 */

class Query
{
public:
	// 생성자로 row와 개수를 받아 저장
	Query(const MYSQL_ROW row, int num);
	// 레퍼런스 형태로 결과를 반환
	const string& getResult(int idx) const;

private:
	vector<string> results;

};

#endif