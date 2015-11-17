#include "database.h"
#include <cstdio>
#include <assert.h>
#include <cstring>

#define DEFAULT_CONFIG_PATH "./judge.conf"

DataBase::DataBase()
{
	FILE *fp = fopen(DEFAULT_CONFIG_PATH, "r");
	assert(fp != nullptr);

	fscanf(fp, "%d",&port);

	char buf[256];
	fscanf(fp, "%s", buf);
	host = buf;

	fscanf(fp, "%s", buf);
	user = buf;

	fscanf(fp, "%s", buf);
	password = buf;

	fscanf(fp, "%s", buf);
	db = buf;

	fclose(fp);

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
	if(!mysql_real_query(conn, sql.c_str(), sql.length()))
	{
	}
}

void DataBase::getQuery(const string& sql, queue<Query>& submitQueue)
{
	assert(!mysql_real_query(conn, sql.c_str(), sql.length()));

	MYSQL_RES *res = mysql_store_result(conn);
	MYSQL_ROW row;
	while((row = mysql_fetch_row(res)) != nullptr)
		submitQueue.push(Query(row, mysql_num_fields(res)));
	mysql_free_result(res);
	//cout << submitQueue.size() << endl;
}