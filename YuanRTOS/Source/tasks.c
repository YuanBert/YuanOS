#include "tasks.h"
#include "ARMCM3.h"

//CM3 内核相关寄存器的宏定义
#define NVIC_INT_CTRL		0xE000ED04
#define NVIC_PENDSVSET		0x10000000
#define NVIC_SYSPRI2		0xE000ED22
#define NVIC_PENDSV_PRI		0x000000FF

//宏操作定义
#define MEM32(addr)		*(volatile unsigned long*)(addr)
#define MEM8(addr)		*(volatile unsigned char*)(addr)

//声明外部的任务
extern yTask *  currentTask;
extern yTask *  nextTask;
extern yTask *  idleTask;
extern yTask *  taskTable[2];

void tTaskRunFirst(void)
{
	__set_PSP(0);
	
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

void ySetSysTickPeriod(uint32_t ms)
{
		//中断向量的配置
	NVIC_SetPriority(SysTick_IRQn,(1 << __NVIC_PRIO_BITS) - 1);
	
	//系统时钟的配置
	SysTick->LOAD = ms * SystemCoreClock / 1200 - 1;
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk|
					SysTick_CTRL_TICKINT_Msk|
					SysTick_CTRL_ENABLE_Msk;
}
void SysTick_Handler()
{
	int i;
	for(i = 0; i < 2; i++)
	{
		if(taskTable[i]->delayTicks > 0)
		{
			taskTable[i]->delayTicks--;
		}
	}
	yTaskSched();
}

__asm void PendSV_Handler(void)
{
	IMPORT  currentTask
	IMPORT  nextTask
	
	MRS R0,PSP
	CBZ R0,PendSVHandler_nosave
	
	STMDB R0!,{R4-R11}
	
	LDR R1,=currentTask
	LDR R1,[R1]
	STR R0,[R1]
	
PendSVHandler_nosave
	LDR R0,=currentTask
	LDR R1,=nextTask
	LDR R2,[R1]
	STR R2,[R0]
	
	LDR R0,[R2]
	LDMIA R0!,{R4-R11}
	
	MSR PSP,R0
	ORR LR,LR,#0x04
	BX LR
}

void yTaskInit(yTask * task, void (*entry)(void *),void *param, yTaskStack * stack)
{
	*(--stack) = (unsigned long)(1 << 24);
	*(--stack) = (unsigned long)entry;
	*(--stack) = (unsigned long)0x14;
	*(--stack) = (unsigned long)0x12;
	*(--stack) = (unsigned long)0x3;
	*(--stack) = (unsigned long)0x2;
	*(--stack) = (unsigned long)0x1;
	*(--stack) = (unsigned long)param;
	
	*(--stack) = (unsigned long)0x11;
	*(--stack) = (unsigned long)0x10;
	*(--stack) = (unsigned long)0x9;
	*(--stack) = (unsigned long)0x8;
	*(--stack) = (unsigned long)0x7;
	*(--stack) = (unsigned long)0x6;
	*(--stack) = (unsigned long)0x5;
	*(--stack) = (unsigned long)0x4;
	
	task->stack = stack;
	task->delayTicks = 0;
}

void yTaskSched(void)
{
   if(idleTask == currentTask)
   {
		if(0 == taskTable[0]->delayTicks)
		{
			nextTask = taskTable[0];
		}
		else if(0 == taskTable[1]->delayTicks)
		{
			nextTask = taskTable[1];
		}
		else
		{
			return;
		}
   }
   else{
		if(taskTable[0] == currentTask)
		{
			if(0 == taskTable[1]->delayTicks)
			{
				nextTask = taskTable[1];
			}
			else if(0 != currentTask->delayTicks)
			{
				nextTask = idleTask;
			}
			else
			{
				return;
			}
		}
		else if(taskTable[1] == currentTask)
		{
			if(0 == taskTable[0]->delayTicks)
			{
				nextTask = taskTable[0];
			}
			else if(0 != currentTask->delayTicks)
			{
				nextTask = idleTask;
			}
			else
			{
				return;
			}
		}
   }
   yTaskSwitch();
}

void yTaskSwitch(void)
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

void yTaskDelay(int delay)
{
	currentTask->delayTicks = delay;
	yTaskSched();
}
