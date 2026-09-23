/*
 * MyNewTask.c
 *
 *  Created on: 7 sep. 2026
 *      Author: santiagosalcedo
 */

#include "timer_task.h"
#include "MemManager.h"
#include "FunctionLib.h"
#include "PhyInterface.h"
#include "MacInterface.h"

osaEventId_t mMyEvents;
/* Global Variable to store our TimerID */
tmrTimerID_t myTimerID = gTmrInvalidTimerID_c;

/* Information about the PAN we are part of */
static panDescriptor_t mCoordInfo;

static instanceId_t   macInstance;

static addrModeType_t mAddrMode;

static uint8_t maMyAddress[8];

/* Handler ID for task */
osaTaskId_t gMyTaskHandler_ID;

/* Data request packet for sending UART input to the coordinator */
static nwkToMcpsMessage_t *mpPacket;

/* The MSDU handle is a unique data packet identifier */
static uint8_t mMsduHandle;

/*
        #define gRedLedIdx_c                    0
        #define gGreenLedIdx_c                  1
        #define gBlueLedIdx_c                   2
 * */
typedef enum {
	GREEN,
	RED,
	BLUE,
	MAGENTA
}t_LED_color;

t_LED_color color;
/* Forward declarations */
void My_Task(osaTaskParam_t argument);
static void myTaskTimerCallback(void *param);

void Send_string(t_LED_color color);

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
        	color=GREEN;
            TMR_StartIntervalTimer(myTimerID,           /*myTimerID*/
                                   1000,                /* Timer's Timeout */
                                   myTaskTimerCallback, /* pointer to
                                   myTaskTimerCallback function */
                                   NULL
            );
            TurnOffLeds();
            LED_TurnOnLed(1);
            break;

        case gMyNewTaskEvent2_c: /* Event called from myTaskTimerCallback */
            TurnOffLeds();
            if(color != MAGENTA){
            	color +=1;
            }
            else{
            	color = GREEN;
            }
            switch(color){
            	case GREEN:
            		TurnOffLeds();
					LED_TurnOnLed(1);
            		break;
            	case RED:
            		TurnOffLeds();
					LED_TurnOnLed(0);
					break;
            	case BLUE:
            		TurnOffLeds();
					LED_TurnOnLed(2);
					break;
            	case MAGENTA:
            		TurnOffLeds();
					LED_TurnOnLed(0);
					LED_TurnOnLed(2);
					break;
            }
            break;

        case gMyNewTaskEvent3_c: /* Event to stop the timer */
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

void Send_string(t_LED_color color){
    mpPacket = MSG_Alloc(sizeof(nwkToMcpsMessage_t) + gMaxPHYPacketSize_c);
    mpPacket->msgType = gMcpsDataReq_c;
	mpPacket->msgData.dataReq.pMsdu = (uint8_t*)(&mpPacket->msgData.dataReq.pMsdu) +
                                              sizeof(mpPacket->msgData.dataReq.pMsdu);
	FLib_MemCpy(&mpPacket->msgData.dataReq.dstAddr, &mCoordInfo.coordAddress, 8);
	FLib_MemCpy(&mpPacket->msgData.dataReq.srcAddr, &maMyAddress, 8);
	FLib_MemCpy(&mpPacket->msgData.dataReq.dstPanId, &mCoordInfo.coordPanId, 2);
	FLib_MemCpy(&mpPacket->msgData.dataReq.srcPanId, &mCoordInfo.coordPanId, 2);
	mpPacket->msgData.dataReq.dstAddrMode = mCoordInfo.coordAddrMode;
	mpPacket->msgData.dataReq.srcAddrMode = mAddrMode;
	mpPacket->msgData.dataReq.msduLength = 10;
	/* Request MAC level acknowledgement of the data packet */
	mpPacket->msgData.dataReq.txOptions = gMacTxOptionsAck_c;
	/* Give the data packet a handle. The handle is
	returned in the MCPS-Data Confirm message. */
	mpPacket->msgData.dataReq.msduHandle = mMsduHandle++;
	/* Don't use security */
	mpPacket->msgData.dataReq.securityLevel = gMacSecurityNone_c;

	/* Send the Data Request to the MCPS */
	(void)NWK_MCPS_SapHandler(mpPacket, macInstance);

	/* Prepare for another data buffer */
	mpPacket = NULL;
}
