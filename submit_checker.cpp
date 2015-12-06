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
		// 1초 간격으로 큐를 검사
		// 큐가 비어있는 경우 불러오고 들어있는 경우 채점
		sleep(1);
		if(submitQueue.empty())
			searchSubmitQueue();
		else
		{
			Query& pick = submitQueue.front();
			check(pick);
			submitQueue.pop();
		}
	}
}

void SubmitChecker::searchSubmitQueue()
{
	// result_id = 2 -> 대기중
	db.getQuery("select id, problem_id, user_id, lang_id from solutions where result_id = 2 order by id asc", submitQueue);
}

/*
 * 채점 진행 상황을 체크하는 함수
 * 채점이 진행되며 쌓이는 로그를 계속 읽어들여서 데이터베이스 갱신
 * 차후 mongo db로 연동 예정
 */

void SubmitChecker::waitJudge(const string& no)
{
	// 파일이 생성될 때 까지 open
	// 채점이 종료될때 까지 생성되지 않으면 종료
	FILE *fp;
	while((fp = fopen("my.txt", "r")) == nullptr && globalSwitch == false);
	if(fp == nullptr)
		return;
	// 정답 코드와 구분하기위해 음수로 입력받음
	// 테스트 케이스 입력
	int code;
	int tc;
	while((code = fscanf(fp,"%d", &tc)) == -1 && globalSwitch == false);
	if(code == -1)
	{
		fclose(fp);
		return;
	}
	tc = -tc;

	// 한 테스트케이스가 끝날때마다 퍼센테이지를 갱신해줌
	for(int i=1; i <= tc; i++)
	{
		int n;
		while((code = fscanf(fp,"%d", &n)) == -1 && globalSwitch == false);
		if(code == -1 || n >= 0)
			break;

		char buf[4];
		sprintf(buf, "%d", i * 100 / tc);

		// 아직 미처리
		//db.getQuery("update solution set state = " + string(buf) + " where no = " + no);
	}
	
	fclose(fp);
}

void SubmitChecker::createCode(const string& code, const string& lang) const
{
	// 언어에 해당하는 확장자로 파일을 생성
	const char *langCode[] = {"", "test.c", "test.cpp", "test.cpp"};
	char buf[256];
	sprintf(buf, "/test/docker/judge/%s", langCode[atoi(lang.c_str())]);
	FILE *fp = fopen(buf, "w");
	assert(fp != nullptr);

	for(size_t i=0; i < code.length(); i += 4096)
		fwrite(code.c_str() + i, min<int>(4096, code.length() - i), 1, fp);

	fclose(fp);
}

void SubmitChecker::check(const Query& pick)
{
	// 코드를 불러옴
	queue<Query> codeQueue;
	db.getQuery("select code from codes where id = " + pick.getResult(NO), codeQueue);

	// 코드가 없는데도 채점이 된 경우는 DB에러로 추정
	if(codeQueue.empty())
	{
		char buf[10];
		sprintf(buf, "%d", OJ_MISS);
		db.getQuery("update solutions set result_id = " + string(buf) + " where id = " + pick.getResult(NO));
		return;
	}

	createCode(codeQueue.front().getResult(0), pick.getResult(LANG));
	codeQueue.pop();

	// 수행하려는 채점의 상태를 대기중에서 채점중으로 변경

	char buf[256];
	sprintf(buf, "%d", RUNNING);
	db.getQuery("update solutions set result_id = " + string(buf) + " where id = " + pick.getResult(NO));

	// 도커 명령 실행
	// --privileged : ptrace를 하기 위한 설정
	// --rm 도커가 끝나면 자동으로 종료 (이게 안되는 경우가 생기면 오류발생)
	string dockerCommand = "docker run --name=test --privileged --rm -w /home -v /test/docker/judge:/home submit /home/judge " +
		pick.getResult(LANG) + " /home/data/" + pick.getResult(PROB_NO) + " > my.txt";

	// 채점 진행률 쓰레드 생성
	globalSwitch = false;
	waitj = thread(&SubmitChecker::waitJudge, this, ref(pick.getResult(NO)));

	assert(system("rm my.txt") != -1);
	assert(system(dockerCommand.c_str()) != -1);
	globalSwitch = true;
	waitj.join();

	// 채점 결과를 읽어 DB에 업데이트

	FILE *fp = fopen("my.txt", "r");
	assert(fp != nullptr);

	int code;
	int N;
	while((code = fscanf(fp, "%d", &N)) == 1 && N < 0);
	assert(code == 1);

	// 정답인 경우만 메모리, 시간을 출력함
	
	int cpuTime = -1, memory = -1;
	if(N == ACCEPT)
		assert(fscanf(fp, "%d%d", &cpuTime, &memory) == 2);

	assert(sprintf(buf, "result_id = %d, time = %d, memory = %d", N, cpuTime, memory) > 0);
	// judge result
	db.getQuery("update solutions set " + string(buf) + " where id = " + pick.getResult(NO));

	assert(sprintf(buf, "%d", N) > 0);

	queue<Query> statQueue;

	bool firstClear = false;
	if(N == ACCEPT)
	{
		db.getQuery("select count from statistics where problem_id = " + pick.getResult(PROB_NO)\
					 + " and user_id = " + pick.getResult(USER_NO) + " and result_id = " + string(buf), statQueue);
		if(statQueue.size() && atoi(statQueue.front().getResult(0).c_str()) > 0)
			firstClear = true;
	}


	db.getQuery("insert into statistics (problem_id, user_id, result_id, count) values ("\
					 + pick.getResult(PROB_NO) + "," + pick.getResult(USER_NO) + "," + string(buf)\
					 + ", 1) ON DUPLICATE KEY UPDATE count = count + 1");

	db.getQuery("insert into problem_statistics (problem_id, result_id, count) values ("\
					 + pick.getResult(PROB_NO) + "," + string(buf)\
					 + ", 1) ON DUPLICATE KEY UPDATE count = count + 1");

	db.getQuery("insert into user_statistics (user_id, result_id, count) values ("\
					 + pick.getResult(USER_NO) + "," + string(buf)\
					 + ", 1) ON DUPLICATE KEY UPDATE count = count + 1");

	db.getQuery("update problems set total_submit = total_submit + 1 where id = " + pick.getResult(PROB_NO));
	if(firstClear)
		db.getQuery("update users set total_submit = total_submit + 1, total_clear = total_clear + 1 where id = " + pick.getResult(USER_NO));
	else
		db.getQuery("update users set total_submit = total_submit + 1 where id = " + pick.getResult(USER_NO));
	fclose(fp);
}