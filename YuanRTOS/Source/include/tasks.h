#ifndef __task_H
#define __task_H

#include "YuanRTOS.h"

/* Cortex-M 的栈单元类型：栈单元的大小位32位 */
typedef uint32_t yTaskStack;
/* 任务结构：包含了任务的相关信息 */
typedef struct _yTask{

	uint32_t *stack;
	
	//任务延时计数器
	uint32_t delayTicks;

}yTask;

void yTaskInit(yTask * task, void (*entry)(void *),void *param, yTaskStack * stack);
void yTaskSched(void);
void yTaskSwitch(void);
void yTaskDelay(int delay);
void ySetSysTickPeriod(uint32_t ms);

#endif
