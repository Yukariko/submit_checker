#include "submit_checker.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <assert.h>

DataBase SubmitChecker::db;
thread SubmitChecker::waitj;
atomic<bool> SubmitChecker::globalSwitch;

void SubmitChecker::run()
{
	while(true)
	{
		sleep(1);
		if(submitQueue.empty())
			searchSubmitQueue();
		else
		{
			Query& pick = submitQueue.front();
			//cout << pick.getResult(NO) << " stanby" << endl;
			check(pick);
			//cout << pick.getResult(NO) << " complete" << endl;
			submitQueue.pop();
		}
	}
}

void SubmitChecker::searchSubmitQueue()
{
	db.getQuery("select id, problem_id, lang_id from solutions where result_id = 2 order by id asc", submitQueue);
}

void SubmitChecker::waitJudge(const string& no)
{
	FILE *fp;
	while((fp = fopen("my.txt", "r")) == nullptr);

	int tc;
	while(fscanf(fp,"%d", &tc) == -1 && globalSwitch == false);
	tc = -tc;

	for(int i=1; i <= tc; i++)
	{
		int n;
		while(fscanf(fp,"%d", &n) == -1 && globalSwitch == false);
		if(n >= 0)
			break;

		char buf[4];
		sprintf(buf, "%d", i * 100 / tc);
		//db.getQuery("update solution set state = " + string(buf) + " where no = " + no);
	}
	
	fclose(fp);
}

void SubmitChecker::createCode(const string& code, const string& lang) const
{
	const char *langCode[] = {"", "test.c", "test.cpp", "test.cpp"};
	char buf[256];
	sprintf(buf, "/test/docker/judge/%s", langCode[atoi(lang.c_str())]);
	FILE *fp = fopen(buf, "w");
	assert(fp != nullptr);

	for(int i=0; i < code.length(); i += 4096)
		fwrite(code.c_str() + i, min<int>(4096, code.length() - i), 1, fp);

	fclose(fp);
}

void SubmitChecker::check(const Query& pick)
{
	queue<Query> codeQueue;
	db.getQuery("select code from codes where id = " + pick.getResult(NO), codeQueue);

	if(codeQueue.empty())
	{
		char buf[10];
		sprintf(buf, "%d", OJ_MISS);
		db.getQuery("update solutions set result_id = " + string(buf) + " where id = " + pick.getResult(NO));
		return;
	}

	createCode(codeQueue.front().getResult(0), pick.getResult(LANG));
	codeQueue.pop();


	char buf[10];
	sprintf(buf, "%d", RUNNING);
	db.getQuery("update solutions set result_id = " + string(buf) + " where id = " + pick.getResult(NO));

	string dockerCommand = "docker rm test & docker run --name=test -w /home -v /test/docker/judge:/home submit /home/judge " +
		pick.getResult(LANG) + " /home/data/" + pick.getResult(PROB_NO) + "/input.txt > my.txt";

	//cout << dockerCommand << endl;
	globalSwitch = false;
	waitj = thread(&SubmitChecker::waitJudge, this, ref(pick.getResult(NO)));

	system("rm my.txt");
	system(dockerCommand.c_str());
	globalSwitch = true;
	waitj.join();
	sleep(1);

	FILE *fp = fopen("my.txt", "r");
	assert(fp != nullptr);

	int N;
	while(fscanf(fp, "%d", &N), N < 0);

	sprintf(buf, "%d", N);

	// judge result
	db.getQuery("update solutions set result_id = " + string(buf) + " where id = " + pick.getResult(NO));
	fclose(fp);
}