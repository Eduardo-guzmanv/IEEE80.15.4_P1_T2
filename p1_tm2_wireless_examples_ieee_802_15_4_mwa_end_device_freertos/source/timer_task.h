/*
 * MyNewTask.h
 *
 *  Created on: 7 sep. 2026
 *      Author: santiagosalcedo
 */

#ifndef TIMER_TASK_H_
#define TIMER_TASK_H_

/* Fwk */
#include "TimersManager.h"
#include "FunctionLib.h"
#include "LED.h"
/* KSDK */
#include "fsl_common.h"
#include "EmbeddedTypes.h"
#include "fsl_os_abstraction.h"

/* Define the available Task's Events */
#define gMyNewTaskEvent1_c (1 << 0)
#define gMyNewTaskEvent2_c (1 << 1)
#define gMyNewTaskEvent3_c (1 << 2)
#define gMyNewTaskEventSW3_c (1 << 3)
#define gMyNewTaskEventSW4_c (1 << 4)

#define gMyTaskPriority_c   3
#define gMyTaskStackSize_c  800

void MyTaskTimer_Start(void);
void MyTaskTimer_Stop(void);
void MyTimer_Init(void);
void MyTask_SW3_Pressed(void);
void MyTask_SW4_Pressed(void);

#endif /* MYNEWTASK_H_ */
