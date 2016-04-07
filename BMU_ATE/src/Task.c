#include <sys/time.h>
#include <sys/un.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../inc/Task.h"
#include "../inc/eload.h"
#include "../inc/Atf.h"
#include "../inc/Ioa.h"
#include "../inc/PL455A.h"


static struct timeval Task_SampPeriod ;
static void Task_100ms(void) ;

void Task_Init(void)
{
	Ioa_Init();
	eload_Init();
	Atf_Init();
	Task_SampPeriod.tv_sec = 0 ;
	Task_SampPeriod.tv_usec = 0 ;
}

void Task_Schedule(void)
{

  struct timeval  endTime;


  gettimeofday(&endTime,NULL);

 /* BMS main task call managenment */
 unsigned int timeused = 1000000*(endTime.tv_sec - Task_SampPeriod.tv_sec ) + (endTime.tv_usec - Task_SampPeriod.tv_usec);

  if( timeused >= 96000 ) /* Sample task period:100ms */
  {
    Task_SampPeriod = endTime ;
    Task_100ms();
    //printf("timeused =%d.\n", timeused);
  }

}


void Task_100ms(void)
{
	Atf_Task();
  //eload_Task();
 //PL455_Task(0);
  Ioa_Task();
}





