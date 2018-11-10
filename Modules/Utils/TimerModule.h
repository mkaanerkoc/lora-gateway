/*
*	Timer Module
*   Author : Mehmet Kaan ERKOC
*	Date : 17.02.2018
*
*/


#ifndef TIMER_MODULE_H
#define TIMER_MODULE_H	


/* Includes  */

#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/timeb.h>  /* ftime, timeb (for timestamp in millisecond) */
#include <sys/time.h>   /* gettimeofday, timeval (for timestamp in microseco

/* Defines */

#define CLOCKID 	CLOCK_REALTIME
#define SIG 		SIGRTMIN

/* Enums */
typedef enum SetupTimerReturnEnum
{
	TIMER_ERROR,
	TIMER_SET_SUCCESFULLY
}SetupTimerReturnEnum;

/* Structures */
typedef struct SoftwareTimerStruct
{
	long long 	timerPeriodInMillis;
	timer_t 	timerID;
	struct 		sigevent	sev;
	struct 		itimerspec 	its;
	long long 	freq_nanosecs;
	sigset_t 	mask;
	struct 		sigaction 	sa;
	void 		(*timerCallbackFunction)(int, siginfo_t*, void*);
}SoftwareTimerStruct;


/* Functions */

void SetupSoftTimer(SoftwareTimerStruct *timerStructParam);

void StopSoftTimer(SoftwareTimerStruct *timerStructParam);

void SetupTimer(long long timerPeriodParam,void (*callbackF)(int, siginfo_t*, void*));

static void TimerHandler(int sig, siginfo_t *si, void *uc);

void printCurrentTime(void);

#endif