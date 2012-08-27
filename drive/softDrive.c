
/*
 * softDrive.c:
 *	Drives the Tortoise using 4 channels software PWM instead
 *      of the single channel PWM.
 *      Note this software is NOT compatible with the baseline
 *      software and hardare buffer boards.
 *      Changes to the BT logic boards are needed. Although
 *      it should be possible to remove the board altogether and 
 *      drive the PWM directly.
 *
 * Pin and GPIO Mapping
 * Bufferboard J2 Pin | GPIO | WiringPi ID | BigTrak Motor Drive
 * 01 | NA | NA | +3V3
 * 02 | NA | NA | 0V
 * 03 | 00 | 08 | Left-
 * 04 | 01 | 09 | Left+
 * 05 | 04 | 07 | Right-
 * 06 | 17 | 00 | Right+
 * 07 | 18 | 01 | PWMout
 * 08 | 21 | 02 | EncoderEn
 * 09 | 22 | 03 | NC
 * 10 | 23 | 04 | NC
 *
 */

#include <wiringPi.h>
#include <softPwm.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>

#define NUM_LEDS         2
int ledMap [NUM_LEDS] = { 9, 0 } ;

void print_usage() {
    printf("Usage: drive -lrfb time(s) \n");
    printf("drive --[left right forward backward] time(s) \n");
    printf("Example: \n");
    printf(" ./drive --left 10\n");
}

void configure_outputs() {
    // Ensure all bufferboard outputs are defined
    softPwmCreate (8, 0, 100) ; //LEFT-
    softPwmCreate (9, 0, 100) ; //LEFT+
    softPwmCreate (7, 0, 100) ; //RIGHT-
    softPwmCreate (0, 0, 100) ; //RIGHT+
    pinMode (2, OUTPUT) ; // J2_08 EncoderEn
    // unused outputs
    pinMode (1, OUTPUT) ;
    pinMode (3, OUTPUT) ;
    pinMode (4, OUTPUT) ;

}
void disable_outputs() {
    // shut all outputs off
    softPwmWrite (8, 0) ;
    softPwmWrite (9, 0) ;
    softPwmWrite (7, 0) ;
    softPwmWrite (0, 0) ;
    digitalWrite (2, 0) ; // set pin low
    // reset unused pins anyway
    digitalWrite (1, 0) ;
    digitalWrite (3, 0) ;
    digitalWrite (4, 0) ;
    // kill all exported GPIO
    // THIS IS IMPORTANT OTHERWISE
    // THE PINS CAN STOP IN WEIRD STATES
    system("gpio unexport all");
}

void drivePwm(int * ledMap, int Pwm, float duration) {
    int i ;
    // enable encoders
    digitalWrite (2, 1) ;

    // set the pwms running
    for (i = 0 ; i < NUM_LEDS ; ++i)
    {
      softPwmWrite (ledMap [i], Pwm) ;
    }
    delay (duration *1000) ;

}


int main (int argc, char** argv)
{
  float duration= 0;
  int opt= 0;
  int Pwm= 0;

    //Specifying the expected options
    //The two options l and b expect numbers as argument
    static struct option long_options[] = {
        {"left",      required_argument, 0,  'l' },
        {"right",     required_argument, 0,  'r' },
        {"forward",     required_argument, 0,  'f' },
        {"backward",     required_argument, 0,  'b' },
        {"duration",     required_argument, 0,  'd' },
        {"help",     no_argument, 0,  'h' },
        {0,           0,                 0,  0   }
    };

    // Check WirpingPi is installed and setup
    if (wiringPiSetup () == -1)
      exit (1) ;

    // Control lines initialised
    configure_outputs() ;

    int long_index =0;
    while ((opt = getopt_long(argc, argv,"l:r:f:b:d:",
                   long_options, &long_index )) != -1) {
        switch (opt) {
             case 'l' :
                 Pwm = atoi(optarg);
                 printf("left %d%% \n", Pwm);
                 ledMap [0] = 8;
                 ledMap [1] = 0;
//                 digitalWrite (8, 1) ; // set pin high
//                 digitalWrite (0, 1) ; // set pin high
                 // no load pwm minimum is 40%
//                 drivePwm(ledMap, 40, duration);
                 break;
             case 'r' :
                 Pwm = atoi(optarg);
                 printf("right %d%% \n", Pwm);
                 ledMap [0] = 7;
                 ledMap [1] = 9;
//                 digitalWrite (7, 1) ; // set pin high
//                 digitalWrite (9, 1) ; // set pin high
//                 drivePwm(40, 40, duration);
                 break;
             case 'f' :
                 Pwm = atoi(optarg);
                 printf("forward %d%% \n", Pwm);
                 ledMap [0] = 0;
                 ledMap [1] = 9;
//                 digitalWrite (0, 1) ; // set pin high
//                 digitalWrite (9, 1) ; // set pin high
//                 drivePwm(20, 50, duration);
                 break;
             case 'b' :
                 Pwm = atoi(optarg);
                 printf("backward %d%% \n", Pwm);
                 ledMap [0] = 8;
                 ledMap [1] = 7;
//                 digitalWrite (8, 1) ; // set pin high
//                 digitalWrite (7, 1) ; // set pin high
//                 drivePwm(20, 50, duration);
                 break;
             case 'd' :
                 duration = atof(optarg);
                 printf("duration %5.3f(s) \n", duration);
                 break;
             case 'h' :
		print_usage();
		return 0;
             default: print_usage();
                 exit(EXIT_FAILURE);
        }
    }


    drivePwm(ledMap, Pwm, duration);

    // stop
    disable_outputs();

//    unexportall();
  return 0 ;

}
