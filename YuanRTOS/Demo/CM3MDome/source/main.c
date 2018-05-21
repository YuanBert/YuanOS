#include "YuanRTOS.h"
#include "tasks.h"

yTask *  currentTask;
yTask *  nextTask;
yTask *  idleTask;
yTask *  taskTable[2];

yTask  tTask1;
yTask  tTask2;
yTask  tTaskIdle;

yTaskStack task1Env[1024];
yTaskStack task2Env[1024];
yTaskStack idleTaskEnv[1024];

int task1flag;
int task2flag;
void task1Entry(void *param)
{
	ySetSysTickPeriod(10);
	while(1)
	{
		task1flag = 1;
		yTaskDelay(1);
		task1flag = 0;
		yTaskDelay(1);
	}
}

void task2Entry(void *param)
{
	while(1)
	{
		task2flag = 1;
		yTaskDelay(1);
		task2flag = 0;
		yTaskDelay(1);
	}
}

void idleTaskEntry(void *param)
{
	while(1)
	{
		//ø’œ–»ŒŒÒ
	}
}

extern void tTaskRunFirst(void);

int main()
{
	yTaskInit(&tTask1, task1Entry, (void *)0x11111111, &task1Env[1024]);
	yTaskInit(&tTask2, task2Entry, (void *)0x11111111, &task2Env[1024]);
	yTaskInit(&tTaskIdle, idleTaskEntry, (void *)0x11111111, &idleTaskEnv[1024]);
	
	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;
	idleTask = &tTaskIdle;
	
	nextTask = taskTable[0];
	
	tTaskRunFirst();
	
	return 0;
}
