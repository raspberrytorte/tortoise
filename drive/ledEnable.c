
/*
 * ledtest.c:
 *   enable pin as output and set state
 *   pin = arg1 , state = arg2
 *
 */

#include <wiringPi.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main (int argc, char** argv)
{
  int led ;
  int state ;

  // require the wiring setup
  // note the default is incompatible with 2 buffer board LEDS 1 & 2
  // which are defined as I2C pins .. or something!

  if (wiringPiSetup () == -1)
    exit (1) ;

  led = atoi(argv[1]) ;
  state = atoi(argv[2]) ;

  pinMode (led, OUTPUT) ;
  digitalWrite (led, state) ;

  return 0 ;

}
