/*
 * MyNewTask.c
 *
 *  Created on: 7 sep. 2026
 *      Author: santiagosalcedo
 */

#include "timer_task.h"

osaEventId_t mMyEvents;
/* Global Variable to store our TimerID */
tmrTimerID_t myTimerID = gTmrInvalidTimerID_c;

/* Handler ID for task */
osaTaskId_t gMyTaskHandler_ID;

/* Local variable to store the current state of the LEDs */
static uint8_t ledsState = 0;

/* Forward declarations */
void My_Task(osaTaskParam_t argument);
static void myTaskTimerCallback(void *param);

/* OSA Task Definition*/
OSA_TASK_DEFINE(My_Task, gMyTaskPriority_c, 1, gMyTaskStackSize_c, FALSE );

/* Main custom task */
void My_Task(osaTaskParam_t argument)
{
    osaEventFlags_t customEvent;
    myTimerID = TMR_AllocateTimer();

    while(1)
    {
        OSA_EventWait(mMyEvents, osaEventFlagsAll_c, FALSE, osaWaitForever_c,
                      &customEvent);

        if( !gUseRtos_c && !customEvent)
        {
            break;
        }

        /* Depending on the received event */
        switch(customEvent){
        case gMyNewTaskEvent1_c:
            TMR_StartIntervalTimer(myTimerID,           /*myTimerID*/
                                   1000,                /* Timer's Timeout */
                                   myTaskTimerCallback, /* pointer to
                                   myTaskTimerCallback function */
                                   NULL
            );
            TurnOffLeds(); /* Ensure all LEDs are turned off */
            break;

        case gMyNewTaskEvent2_c: /* Event called from myTaskTimerCallback */
            if(!ledsState) {
                TurnOnLeds();
                ledsState = 1;
            }
            else {
                TurnOffLeds();
                ledsState = 0;
            }
            break;

        case gMyNewTaskEvent3_c: /* Event to stop the timer */
            ledsState = 0;
            TurnOffLeds();
            TMR_StopTimer(myTimerID);
            break;

        default:
            break;
        }
    }
}

/* Function to init the task */
void MyTimer_Init(void)
{
    mMyEvents = OSA_EventCreate(TRUE);
    /* The instance of the MAC is passed at task creaton */
    gMyTaskHandler_ID = OSA_TaskCreate(OSA_TASK(My_Task), NULL);
}

/* This is the function called by the Timer each time it expires */
static void myTaskTimerCallback(void *param)
{
    OSA_EventSet(mMyEvents, gMyNewTaskEvent2_c);
}

/* Public function to send an event to stop the timer */
void MyTaskTimer_Stop(void)
{
    OSA_EventSet(mMyEvents, gMyNewTaskEvent3_c);
}

/* Public function to send an event to start the timer */
void MyTaskTimer_Start(void)
{
    OSA_EventSet(mMyEvents, gMyNewTaskEvent1_c);
}
