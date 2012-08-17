
/*
 * forward.c:
 *	Program to ramp the PWM to get up to speed
 *      then run a the final PWN for a short while
 *      user defined pins are enabled to route the PWM
 *      as per the bufferboard.
 *
 */

#include <wiringPi.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>

void print_usage() {
    printf("Usage: drive -lrfb time(s) \n");
    printf("drive --[left right forward backward] time(s) \n");
    printf("Example: \n");
    printf(" ./drive --left 10\n");
}

void configure_outputs() {
    // Ensure all bufferboard outputs are defined
    pinMode (1, PWM_OUTPUT) ; // J2_07
    pinMode (8, OUTPUT) ; // J2_03
    pinMode (9, OUTPUT) ; // J2_04
    pinMode (7, OUTPUT) ; // J2_05
    pinMode (0, OUTPUT) ; // J2_06
    pinMode (2, OUTPUT) ; // J2_08


}
void disable_outputs() {
    //Disable all control lines
    pwmWrite (1, 0) ; // disable PWM
    digitalWrite (8, 0) ; // set pin low
    digitalWrite (9, 0) ; // set pin low
    digitalWrite (7, 0) ; // set pin low
    digitalWrite (0, 0) ; // set pin low
    digitalWrite (2, 0) ; // set pin low
}

int main (int argc, char** argv)
{
  char direction ;
  int duration= 0;
  int prf ;
  int opt= 0;

    //Specifying the expected options
    //The two options l and b expect numbers as argument
    static struct option long_options[] = {
        {"left",      required_argument, 0,  'l' },
        {"right",     required_argument, 0,  'r' },
        {"forward",   required_argument, 0,  'f' },
        {"backward",  required_argument, 0,  'b' },
        {0,           0,                 0,  0   }
    };

// Pin and GPIO Mapping
// Bufferboard J2 Pin |  GPIO   | WiringPi ID | BigTrak Motor Drive 
// 01 | NA | NA | +3V3
// 02 | NA | NA | 0V
// 03 | 00 | 08 | Left+
// 04 | 01 | 09 | Left-
// 05 | 04 | 07 | Right+
// 06 | 17 | 00 | Right-
// 07 | 18 | 01 | PWMout
// 08 | 21 | 02 | EncoderEn
// 09 | 22 | 03 | NC
// 10 | 23 | 04 | NC

    // Check WirpingPi is installed and setup
    if (wiringPiSetup () == -1)
      exit (1) ;

    // Control lines initialised
    configure_outputs() ;
    disable_outputs() ;

    // Select the direction and set enables
//    digitalWrite (2, 1) ; // Enable Encoders

    int long_index =0;
    while ((opt = getopt_long(argc, argv,"l:r:f:b:",
                   long_options, &long_index )) != -1) {
        switch (opt) {
             case 'l' :
                 duration = atoi(optarg);
                 printf("left %d(s)\n", duration);
                 digitalWrite (8, 1) ; // set pin high
                 digitalWrite (0, 1) ; // set pin high
                 break;
             case 'r' :
                 duration = atoi(optarg);
                 printf("right %d(s)\n", duration);
                 digitalWrite (7, 1) ; // set pin high
                 digitalWrite (9, 1) ; // set pin high
                 break;
             case 'f' :
                 duration = atoi(optarg);
                 printf("forward %d(s)\n", duration);
                 digitalWrite (8, 1) ; // set pin high
                 digitalWrite (7, 1) ; // set pin high
                 break;
             case 'b' :
                 duration = atoi(optarg);
                 printf("backward %d(s)\n", duration);
                 digitalWrite (0, 1) ; // set pin high
                 digitalWrite (9, 1) ; // set pin high
                 break;
             default: print_usage();
                 exit(EXIT_FAILURE);
        }
    }

    // this generates a ramping PRF 
    // assuming 0 PRF to start and stopping at ~80% say
    for (prf = 1 ; prf < 100 ; ++prf)
    {
      pwmWrite (1, (1024/128) * prf) ; // set prf 0 - 1023
      delay (20) ; // set duration ms
      // by controlling the delay its possible to change the acceleration
    }

    // run for a further n seconds at final prf
    pwmWrite (1, (1024/128) * 100) ;
    delay (duration * 1000);

    // stop
    pwmWrite (1, 0) ; // disable PWM
    digitalWrite (8, 0) ; // set pin low
    digitalWrite (9, 0) ; // set pin low
    digitalWrite (7, 0) ; // set pin low
    digitalWrite (0, 0) ; // set pin low
    digitalWrite (2, 0) ; // set pin low

  return 0 ;

}
