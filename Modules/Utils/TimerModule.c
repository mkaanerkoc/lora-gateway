#include "TimerModule.h"


  #define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                               } while (0)

/* Private Variables */

timer_t timerid_a,timerid_b;

struct 		sigevent	sev;
struct 		itimerspec 	its;
long long 	freq_nanosecs;
sigset_t 	mask;
struct 		sigaction 	sa;

/* Private Function Prototypes */
static void	print_siginfo(siginfo_t *si);

/* Functions */
void SetupSoftTimer(SoftwareTimerStruct *timerStructParam)
{
	long long timerPeriodTemp = ((timerStructParam->timerPeriodInMillis) * 1000000);
	timerStructParam->sa.sa_flags = SA_SIGINFO;
	timerStructParam->sa.sa_sigaction = timerStructParam->timerCallbackFunction;
	sigemptyset(&(timerStructParam->sa.sa_mask));
	if (sigaction(SIG, &(timerStructParam->sa), NULL) == -1)
	{
	   errExit("sigaction");
	}

	sigemptyset(&(timerStructParam->mask));
	sigaddset(&(timerStructParam->mask), SIG);
	if (sigprocmask(SIG_SETMASK, &(timerStructParam->mask), NULL) == -1)
	{
	   errExit("sigprocmask");
	}

	/* Create the timer */
	timerStructParam->sev.sigev_notify = SIGEV_SIGNAL;
	timerStructParam->sev.sigev_signo = SIG;
	timerStructParam->sev.sigev_value.sival_ptr = &(timerStructParam->timerID);
	if (timer_create(CLOCKID, &(timerStructParam->sev), &(timerStructParam->timerID)) == -1)
	{
	   errExit("timer_create");
	}
           

	freq_nanosecs = timerPeriodTemp;
    timerStructParam->its.it_value.tv_sec = freq_nanosecs / 1000000000;
    timerStructParam->its.it_value.tv_nsec = freq_nanosecs % 1000000000;
    timerStructParam->its.it_interval.tv_sec = timerStructParam->its.it_value.tv_sec;
    timerStructParam->its.it_interval.tv_nsec = timerStructParam->its.it_value.tv_nsec;
    if (timer_settime(timerStructParam->timerID, 0, &(timerStructParam->its), NULL) == -1)
    {
    	errExit("timer_settime");
    }  
}
void StopSoftTimer(SoftwareTimerStruct *timerStructParam)
{
	timerStructParam->its.it_value.tv_sec = 0;
    timerStructParam->its.it_value.tv_nsec = 0;
    timerStructParam->its.it_interval.tv_sec = timerStructParam->its.it_value.tv_sec;
    timerStructParam->its.it_interval.tv_nsec = timerStructParam->its.it_value.tv_nsec;
    if (timer_settime(timerStructParam->timerID, 0, &(timerStructParam->its), NULL) == -1)
    {
    	errExit("timer_settime");
    }  
}

void SetupTimer(long long timerPeriodParam,void (*callbackF)(int, siginfo_t*, void*))
{

	long long timerPeriodTemp = (timerPeriodParam * 1000000);
	//printf("Establishing handler for signal %d\n", SIG);
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = (*callbackF);
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIG, &sa, NULL) == -1)
	{
	   errExit("sigaction");
	}

	/* Block timer signal temporarily */

	//printf("Blocking signal %d\n", SIG);
	sigemptyset(&mask);
	sigaddset(&mask, SIG);
	if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
	{
	   errExit("sigprocmask");
	}

	/* Create the timer */

	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIG;
	sev.sigev_value.sival_ptr = &timerid_a;
	if (timer_create(CLOCKID, &sev, &timerid_a) == -1)
	{
	   errExit("timer_create");
	}
           

	freq_nanosecs = timerPeriodTemp;
    its.it_value.tv_sec = freq_nanosecs / 1000000000;
    its.it_value.tv_nsec = freq_nanosecs % 1000000000;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;
    if (timer_settime(timerid_a, 0, &its, NULL) == -1)
    {
    	errExit("timer_settime");
    }           
}

static void TimerHandler(int sig, siginfo_t *si, void *uc)
{
   	printf("Caught signal %d\n", sig);
   	print_siginfo(si);
	signal(sig, SIG_IGN);
	printCurrentTime();
}


static void	print_siginfo(siginfo_t *si)
{
   timer_t *tidp;
   int or;

   tidp = si->si_value.sival_ptr;

   printf("    sival_ptr = %p; ", si->si_value.sival_ptr);
   printf("    *sival_ptr = 0x%lx\n", (long) *tidp);

   or = timer_getoverrun(*tidp);
   if (or == -1)
       errExit("timer_getoverrun");
   else
       printf("    overrun count = %d\n", or);
}


void printCurrentTime(void)
{
	time_t rawtime;
	struct tm * timeinfo;

  	time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
  	printf ( "Current local time and date: %s", asctime (timeinfo) );
}