#include "database.h"
#include <cstdio>
#include <assert.h>
#include <cstring>

#define DEFAULT_CONFIG_PATH "./judge.conf"

DataBase::DataBase()
{
	// conf 파일에서 설정값들을 읽어옴
	FILE *fp = fopen(DEFAULT_CONFIG_PATH, "r");
	assert(fp != nullptr);

	// 포트번호 입력
	assert(fscanf(fp, "%d",&port) == 1);

	char buf[256];

	// host 주소 입력
	assert(fscanf(fp, "%s", buf) == 1);
	host = buf;

	// user id 입력
	assert(fscanf(fp, "%s", buf) == 1);
	user = buf;

	// user pw 입력
	assert(fscanf(fp, "%s", buf) == 1);
	password = buf;

	// DB 명 입력
	assert(fscanf(fp, "%s", buf) == 1);
	db = buf;

	fclose(fp);

	// DB 연결
	conn = mysql_init(NULL);

	const char timeout = 30;
	mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
	assert(mysql_real_connect(conn, host.c_str(), user.c_str(), password.c_str(), db.c_str(), port, 0 , 0));

	const char * utf8sql = "set names utf8";
	assert(!mysql_real_query(conn, utf8sql, strlen(utf8sql)));
}

DataBase::~DataBase()
{
	mysql_close(conn);
}

void DataBase::getQuery(const string& sql)
{
	// 예외처리를 따로 하지 않음 
	if(mysql_real_query(conn, sql.c_str(), sql.length()))
	{
		cout << "[Error] Query Fail : " << sql << endl;
	}
}

void DataBase::getQuery(const string& sql, queue<Query>& queryQueue)
{
	// 쿼리 결과를 queue에 저장
	assert(!mysql_real_query(conn, sql.c_str(), sql.length()));
	MYSQL_RES *res = mysql_store_result(conn);
	MYSQL_ROW row;
	while((row = mysql_fetch_row(res)) != nullptr)
		queryQueue.push(Query(row, mysql_num_fields(res)));
	mysql_free_result(res);
}