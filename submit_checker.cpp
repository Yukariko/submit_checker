#include "submit_checker.h"

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

SubmitChecker::check(const query& pick)
{
	
}