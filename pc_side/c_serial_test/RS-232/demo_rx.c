#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "rs232.h"
#include <sys/time.h>

int main()
{
  int i, n,
      cport_nr=0,        /* /dev/ttyS0 (COM1 on windows) */
      bdrate=230400;

  unsigned char buf[4096];
  char mode[]={'8','N','1',0};
  if(RS232_OpenComport(cport_nr, bdrate, mode))
  {
    printf("Can not open comport\n");
    return(1);
  }
	
  int k;
  struct timeval time;
  gettimeofday(&time, NULL);
  long startTime = ((unsigned long long)time.tv_sec * 1000000) + time.tv_usec;
  for (k = 0; k < 100; k++)
  {
    n = RS232_PollComport(cport_nr, buf, 4095);
	printf("%s\n", buf);
//     if(n > 0)
//     {
//       buf[n] = 0;   /* always put a "null" at the end of a string! */
// 
//       for(i=0; i < n; i++)
//       {
//         if(buf[i] < 32)  /* replace unreadable control-codes by dots */
//         {
//           buf[i] = '.';
//         }
//       }
// 
//       printf("received %i bytes: %s\n", n, (char *)buf);
//    }
  }
  struct timeval time_n;
  gettimeofday(&time_n, NULL);
  long endTime = ((unsigned long long)time_n.tv_sec * 1000000) + time_n.tv_usec;
  double aveTime = (endTime - startTime) / 100.0;
  printf("average receive time is %f\n", aveTime);
  return(0);
}

