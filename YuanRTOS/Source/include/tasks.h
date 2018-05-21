#ifndef __task_H
#define __task_H

#include "YuanRTOS.h"

/* Cortex-M ��ջ��Ԫ���ͣ�ջ��Ԫ�Ĵ�Сλ32λ */
typedef uint32_t yTaskStack;
/* ����ṹ������������������Ϣ */
typedef struct _yTask{

	uint32_t *stack;
	
	//������ʱ������
	uint32_t delayTicks;

}yTask;

void yTaskInit(yTask * task, void (*entry)(void *),void *param, yTaskStack * stack);
void yTaskSched(void);
void yTaskSwitch(void);
void yTaskDelay(int delay);
void ySetSysTickPeriod(uint32_t ms);

#endif
