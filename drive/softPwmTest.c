/*
 * softPwmTest.c:
 *      Program to test WiringPi softPwm
 *      compile using the -lpthread
 *      gcc softPwmTest.c -I/usr/local/include -L/usr/local/lib -lwiringPi -lpt$
 *      defaults use the led layout in the tortoise buffer board.
 *      edit as required
 *
 *      Pin and GPIO Mapping
 *      Bufferboard J2 Pin |  GPIO   | WiringPi ID | BigTrak Motor Drive 
 *      01 | NA | NA | +3V3
 *      02 | NA | NA | 0V
 *      03 | 00 | 08 | Left-
 *      04 | 01 | 09 | Left+
 *      05 | 04 | 07 | Right-
 *      06 | 17 | 00 | Right+
 *      07 | 18 | 01 | PWMout
 *      08 | 21 | 02 | EncoderEn
 *      09 | 22 | 03 | NC
 *      10 | 23 | 04 | NC
 *
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
#include <softPwm.h>

#define RANGE		100
#define	NUM_LEDS	 8

int ledMap [NUM_LEDS] = { 8, 9, 7, 0, 1, 2, 3, 4 } ;

int values [NUM_LEDS] = { 1, 10, 2, 20, 5, 50, 10, 100 } ;

int main ()
{
  int i ;

  // setup wirinfPi
  if (wiringPiSetup () == -1)
  {
    fprintf (stdout, "oops: %s\n", strerror (errno)) ;
    return 1 ;
  }

  // initialise the pins
  for (i = 0 ; i < NUM_LEDS ; ++i)
  {
    // initial value = 0
    softPwmCreate (ledMap [i], 0, RANGE) ;
    softPwmWrite (ledMap [i], values [i]) ;
  }

  printf ("Wait a few seconds or Ctrl-C to quit\n");

  for (i = 0 ; i < 5; ++i)
  {
    delay(i*1000) ;
  }

  return 0 ;
}
