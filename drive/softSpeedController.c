/*
 * softSpeedController.c:
 *	Uses software PWM and interupts on the Encoders to form a crude 
 *      controller for speed.
 *
 *      To use the interrupt functions to read the encoders
 *      the wiringPiSetupSys() has to be used. This means that all
 *      pins in the code now refer to the BroadCom GPIO pins.
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

// Used to lock and unlock a thread
#define COUNT_LEFT	0
#define COUNT_RIGHT	1

// GPIO 24 is the left hand wheel encoder
// GPIO 10 is the right hand wheel encoder
#define LEFT_ENCODER	24
#define RIGHT_ENCODER	10

// these need to be floats for further maths to work
static volatile float globalCounterLeft = 0;
static volatile float globalCounterRight = 0;

#define NUM_LEDS         2
int ledMap [NUM_LEDS] = { 0, 1 } ;

// Setup the threads
/*
 * waitForLeftEncoder:
 *      This is a thread created using the wiringPi simplified threading
 *      mechanism. It monitors the number of encoder pulses and from this
 *      generates an estimated rate.
 *********************************************************************************
 */

PI_THREAD (waitForLeftEncoder)
{
  (void)piHiPri (10) ;  // Set this thread to be high priority

  for (;;)
  {
    if (waitForInterrupt (LEFT_ENCODER, -1) > 0)        // Got it
    {
      piLock (COUNT_LEFT) ;
      ++globalCounterLeft ;
      piUnlock (COUNT_LEFT) ;
    }
  }
}
/*
 * waitForRightEncoder:
 *      This is a thread created using the wiringPi simplified threading
 *      mechanism. It monitors the number of encoder pulses and from this
 *      generates an estimated rate.
 *********************************************************************************
 */

PI_THREAD (waitForRightEncoder)
{
  (void)piHiPri (10) ;  // Set this thread to be high priority

  for (;;)
  {
    if (waitForInterrupt (RIGHT_ENCODER, -1) > 0)        // Got it
    {
      piLock (COUNT_RIGHT) ;
      ++globalCounterRight ;
      piUnlock (COUNT_RIGHT) ;
    }
  }
}

void print_usage() {
    printf("Usage: \n softSpeedController -lrfb speed -d duration \n");
    printf(" softSpeedController --[left right forward backward]  --duration inSeconds \n");
    printf("where: \n");
    printf("    speed is pulses / second \n");
    printf("    duration is total runtime in seconds.\n");
    printf("Example: \n");
    printf(" ./softSpeedController --left 500 --duration 10\n");
}

void configure_outputs() {
    // Setup the outputs
    system ("gpio export 0 out");
    system ("gpio export 1 out");
    system ("gpio export 4 out");
    system ("gpio export 17 out");
    system ("gpio export 18 out");
    system ("gpio export 21 out");
    system ("gpio export 22 out");
    system ("gpio export 23 out");

    // Setup the inputs
    system ("gpio edge 24 falling");
    system ("gpio edge 10 falling");

    // Create the soft PWMs
    softPwmCreate (0, 0, 100) ; //LEFT-
    softPwmCreate (1, 0, 100) ; //LEFT+
    softPwmCreate (4, 0, 100) ; //RIGHT-
    softPwmCreate (17, 0, 100) ; //RIGHT+

    softPwmWrite (0,0);
    softPwmWrite (1,0);
    softPwmWrite (4,0);
    softPwmWrite (17,0);

    // Create the threads to read the encoders
    piThreadCreate (waitForLeftEncoder);
    piThreadCreate (waitForRightEncoder);

}
void disable_outputs() {
    // shut all outputs off
    softPwmWrite (0, 0) ;
    softPwmWrite (1, 0) ;
    softPwmWrite (4, 0) ;
    softPwmWrite (17, 0) ;

    // As a double safety measure directly set the PWM outputs to zero.
    digitalWrite (0, 0);
    digitalWrite (1, 0);
    digitalWrite (4, 0);
    digitalWrite (17, 0);

    digitalWrite (18, 0) ; // set pin low
    // reset unused pins anyway
    digitalWrite (21, 0) ;
    digitalWrite (22, 0) ;
    digitalWrite (23, 0) ;
}

void drivePwm(int * ledMap, float requiredSpeed, float duration) {
    int selectOutput ;
    int currentPwm = 10; // PWM as percentage
    int timerCounter = 0;
    int loopDelay = 10; //milliseconds
    float encoderCount = 0;
    float leftSpeed = 0.0;

    while (timerCounter < duration*1000) {
    // estimate current speed
    piLock(COUNT_LEFT);
    leftSpeed = ((globalCounterLeft - encoderCount) / loopDelay);
    piUnlock(COUNT_LEFT);
    // adjust pwm

    if ( leftSpeed == requiredSpeed ) {
    // do nothing
    }

    if (leftSpeed > requiredSpeed) {
    // slow down
        currentPwm = currentPwm -1;
    }

    if (leftSpeed < requiredSpeed) {
    // speed up
       currentPwm = currentPwm +1;
    }

    // set pwm outputs
    for (selectOutput = 0 ; selectOutput < NUM_LEDS ; ++selectOutput)
    {
    if ( currentPwm > 99) {
    // saturate pwm
        currentPwm = 99;
    }
      softPwmWrite (ledMap [selectOutput], currentPwm) ;
    }
//    printf(" timer = %d ", timerCounter);
    encoderCount = globalCounterLeft;
    printf("\r counts = %f, speed = %f, pwm = %d (Ctrl^C to quit)\n", globalCounterLeft, leftSpeed, currentPwm);
    delay(loopDelay); // wait 10ms should yield ~100Hz bandwidth
    timerCounter = timerCounter + loopDelay;
    }
}


int main (int argc, char** argv)
{
  float duration= 0;
  int opt= 0;
  float Pwm= 0.0;
  int myLeftCounter = 0;
  int myRightCounter = 0;
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
    if (wiringPiSetupSys () == -1)
      exit (1) ;

    // Control lines initialised
    configure_outputs() ;

    int long_index =0;

    //printf("Start Encoder Positions = %d, %d \n", myLeftCounter, myRightCounter);

    while ((opt = getopt_long(argc, argv,"l:r:f:b:d:",
                   long_options, &long_index )) != -1) {
        switch (opt) {
             case 'l' :
                 Pwm = atoi(optarg);
                 printf("left %f%% \n", Pwm);
                 ledMap [0] = 17;
                 ledMap [1] = 0;
                 break;
             case 'r' :
                 Pwm = atoi(optarg);
                 printf("right %f%% \n", Pwm);
                 ledMap [0] = 4;
                 ledMap [1] = 1;
                 break;
             case 'f' :
                 Pwm = atoi(optarg);
                 printf("forward %f%% \n", Pwm);
                 ledMap [0] = 17;
                 ledMap [1] = 1;
                 break;
             case 'b' :
                 Pwm = atoi(optarg);
                 printf("backward %f%% \n", Pwm);
                 ledMap [0] = 0;
                 ledMap [1] = 4;
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
    piLock(COUNT_LEFT);
    myLeftCounter = globalCounterLeft;
    piUnlock(COUNT_LEFT);
    piLock(COUNT_RIGHT);
    myRightCounter = globalCounterRight;
    piUnlock(COUNT_RIGHT);
    printf("End Encoder Positions %d, %d \n", myLeftCounter, myRightCounter);
  return 0 ;

}
