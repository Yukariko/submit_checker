#include "submit_checker.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <assert.h>

DataBase SubmitChecker::db;
atomic<bool> SubmitChecker::globalSwitch;
thread SubmitChecker::waitj;

SubmitChecker::run()
{
	while(true)
	{
		sleep(1);
		if(submitQueue.empty())
			searchSubmitQueue();
		else
		{
			query& pick = submitQueue.front();
			check(pick);
			submitQueue.pop();
		}
	}
}

SubmitChecker::searchSubmitQueue()
{
	db.getQuery("select no, prob_no, lang from solution where result = -1", submitQueue);
}

void waitJudge(const string& no)
{
	FILE *fp;
	while((fp = fopen("/test/docker/judge/my.txt", "r")) == nullptr);

	int tc;
	while(fscanf(fp,"%d", &tc) == -1);
	tc = -tc;

	for(int i=1; i <= tc; i++)
	{
		int n;
		while(fscanf(fp,"%d", &tc) == -1);
		if(n >= 0)
			break;

		char buf[4];
		sprintf(buf, "%d", i * 100 / tc);
		db.getQuery("update solution set state = " + buf + " where no = " + no);
	}
	
	fclose(fp);
}

SubmitChecker::check(const Query& pick)
{
	string dockerCommand = "docker run -name test -w /home -v /test/docker/judge:/home submit " +
		"/home/judge " + pick.getResult(LANG) + " /home/data/" + pick.getResult(PROB_NO) + "/input.txt" +
		" > my.txt";


	waitj = thread(&SubmitChecker::waitJudge,this);

	system("rm my.txt");
	system(dockerCommand.c_str());

	waitj.join();
	sleep(1);

	FILE *fp = fopen("my.txt", "r");
	assert(fp != nullptr);

	int N;
	while(fscanf(fp, "%d", &N), N < 0);

	// judge result

	fclose(fp);
}