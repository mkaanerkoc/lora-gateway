#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include <my_global.h>
#include <mysql.h>

//User Library Includes //
#include "StorageManager.h"
#include "DataChannelManager.h"
#include "EthernetControl.h"
#include "RadioManager.h"
#include "ModbusTCP.h"
#include "TimerModule.h"
#include "RTCManager.h"

#include <sys/time.h>
#include <time.h>

//Timer Callback function...
static void timerHandlerFunc(int sig, siginfo_t *si, void *uc);
TimeStruct_t timerStTemp;
DateStruct_t dateStTemp;

int main(void)
{
  
	if (!bcm2835_init())
    {
        fprintf( stderr, "%s bcm2835_init() Failed\n\n", __BASEFILE__ );
        return 1;
    }
    else
    {
    	printf("<Application> BCM2835 Library initialized succesfully !\n");
    }
    mysql_library_init(0, NULL, NULL);
  	InitializeDeviceManager();
  	InitializeStorageManager();
    InitializeDataChannelManager();
  	InitializeRadioManager();
    Initialize_EthernetControl();
    Initialize_ModbusTCP();
    if(Radio_Begin(NON_THREAD))
    {
      printf("<RadioManager> Begin succesfully\n");
    }
    else
    {
      printf("<RadioManager> Cannot begin succesfully\n");
    }
    RTC_GetTime(&timerStTemp);
    RTC_GetDate(&dateStTemp);
    printf("<Application> The current time is : %02d:%02d:%02d %02d/%02d/%d \n",timerStTemp.hour,timerStTemp.minute, timerStTemp.second, dateStTemp.day,dateStTemp.month,dateStTemp.year);
    
    SX1272_SetMode(RHModeRx,NON_THREAD);
  	printf("MySQL client version: %s\n", mysql_get_client_info());
  	printf("<Application> All Modules Are Started Succesfully \n");
  	Start_EthernetControl(); //bu fonksiyon sadece pthread_join cagiriyor. Tum tasklarin calismasi icin aslinda yeterli.
  	
    return 0;
}


static void timerHandlerFunc(int sig, siginfo_t *si, void *uc)
{
  printf("Hello From Callback Function -> Caught signal %d\n", sig);
  //print_siginfo(si);
  signal(sig, SIG_IGN);
  printCurrentTime();
}
