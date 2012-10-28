/*
 * readEncoders.c:
 *	Reads and counts encoder pluses on defined interupts
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

// A 'key' which we can lock and unlock - values are 0 through 3
//	This is interpreted internally as a pthread_mutex by wiringPi
//	which is hiding some of that to make life simple.

#define	COUNT_LEFT	0
#define COUNT_RIGHT     1

// What BCM_GPIO input are we using?
//	GPIO 24 is the left hand wheel encoder
//      GPIO 10 is the right hand wheel encoder

#define	LEFT_ENCODER	24
#define RIGHT_ENCODER   10

// globalCounter:
//	Global variable to count interrupts
//	Should be declared volatile to make sure the compiler doesn't cache it.

static volatile int globalCounterLeft = 0 ;
static volatile int globalCounterRight = 0;

/*
 * waitForLeftEncoder:
 *	This is a thread created using the wiringPi simplified threading
 *	mechanism. It monitors the number of encoder pulses and from this
 *      generates an estimated rate.
 *********************************************************************************
 */

PI_THREAD (waitForLeftEncoder)
{
  (void)piHiPri (10) ;	// Set this thread to be high priority

  for (;;)
  {
    if (waitForInterrupt (LEFT_ENCODER, -1) > 0)	// Got it
    {
      piLock (COUNT_LEFT) ;
	++globalCounterLeft ;
	//printf("globalCounterLeft = %d \n", globalCounterLeft);
      piUnlock (COUNT_LEFT) ;
    }
  }
}

PI_THREAD (waitForRightEncoder)
{
  (void)piHiPri (10); // Set this thread to be high priority
  
  for (;;)
  {
    if (waitForInterrupt (RIGHT_ENCODER, -1) > 0)
    {
      piLock (COUNT_RIGHT);
        ++globalCounterRight;
        //printf("globalCounterRight = %d \n", globalCounterRight);
      piUnlock (COUNT_RIGHT);
    }
  }  
}


/*
 * setup:
 *	Demo a crude but effective way to initialise the hardware
 *********************************************************************************
 */

void setup (void)
{

// Use the gpio program to initialise the hardware
//	(This is the crude, but effective bit)

  system ("gpio edge 24 falling") ;
  system ("gpio edge 10 falling") ;
  system ("gpio export 17 out") ;
  system ("gpio export 18 out") ;

// Setup wiringPi

  wiringPiSetupSys () ;

// Fire off our interrupt handler

  piThreadCreate   (waitForLeftEncoder) ;
  piThreadCreate   (waitForRightEncoder) ;
}


/*
 * main
 *********************************************************************************
 */

int main (void)
{
  int myLeftCounter, myRightCounter   = 0 ;

  setup () ;

  for (;;)
  {
    printf ("Waiting ... \n") ; fflush (stdout) ;

    //while (myCounter == lastCounter)
    while (1)
    {
      piLock (COUNT_LEFT) ;
	myLeftCounter = globalCounterLeft ;
      piUnlock (COUNT_LEFT) ;
      piLock (COUNT_RIGHT) ;
        myRightCounter = globalCounterRight ;
      piUnlock (COUNT_RIGHT) ;
      printf("\r Left Count = %d, Right Count = %d (Ctrl^C to quit)\n", myLeftCounter, myRightCounter);
      delay (10) ; //wait 10ms
    }
  }

  return 0 ;
}
