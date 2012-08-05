//
//
// Gertboard Demo
//
// These routines walk the LEDs
//
//
// This file is part of gertboard_demo.
//
//
// Copyright (C) Gert Jan van Loo 2012
// No rights reserved
// You may treat this program as if it was in the public domain
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//
//


//
//
// Try to strike a balance between keep code simple for
// novice programmers but still have reasonable quality code
//

#include "gertboard_demo.h"

extern volatile unsigned *gpio;

// Use defines for the LEDS
// So if you use a different GPIO or have to order wrong
// all you have to do is change it here
//
// For novice users: don't worry about the complexity
// The compiler will optimise out all constant expressions and you
// will end up with a single constant value in your table.
//#define L0 (1<<17)
//#define L1 (1<<21)
//#define L2 (1<<22)
//#define L3 (1<<23)
//#define L4 (1<< 0)
//#define L5 (1<< 1)
#define L0 (1<< 0)
#define L1 (1<< 1)
#define L2 (1<< 4)
#define L3 (1<<17)
#define L4 (1<<18)
#define L5 (1<<21)
#define L6 (1<<22)
#define L7 (1<<23)

#define ALLEDS  (L0|L1|L2|L3|L4|L5|L6|L7)

//
//  GPIO
//

#define GPIO_SET0   *(gpio+7)  // Set GPIO high bits 0-31
#define GPIO_SET1   *(gpio+8)  // Set GPIO high bits 32-53

#define GPIO_CLR0   *(gpio+10) // Set GPIO low bits 0-31
#define GPIO_CLR1   *(gpio+11) // Set GPIO low bits 32-53



//
//  SPI register you need
//
#define SPI0_CNTLSTAT *(spi0 + 0)
#define SPI0_FIFO     *(spi0 + 1)

//
// define the various patterns
//

//static int pattern0[] = {L0, L1, L2, L3, L4, L5, -1 };
static int pattern0[] = {0x0,L0,L1,L2,L3,L4,L5,L6,L7, -1};
static int pattern1[] = {0x0,L7,L6,L5,L4,L3,L2,L1,L0, -1};
static int pattern2[] = {0x0,L0|L1|L2|L3|L4|L5|L6|L7,-1};
static int pattern3[] = {0x0,L0|L1|L2|L3,-1};
static int pattern4[] = {0x0,L4|L5|L6|L7,-1};
static int pattern5[] = {0x0,-1};


void set_pattern(int value);
void select_pattern(int p);


// Local variables
static int *pattern = pattern0;  // current pattern
static int step = 0;  // where we are now

//
// Start new with one of the available patterns
//
void select_pattern(int p)
{
   switch (p)
   {
   case 0 : pattern = pattern0; break;
   case 1 : pattern = pattern1; break;
   case 2 : pattern = pattern2; break;
   case 3 : pattern = pattern3; break;
   case 4 : pattern = pattern4; break;
   case 5 : pattern = pattern5; break;
   default: return;
   }
   step = 0;
   set_pattern(pattern[step]);

} // select_pattern

void set_pattern(int value)
{
   GPIO_CLR0 = ALLEDS;
   GPIO_SET0 = value;
} // set_pattern

//
// Do single pattern step
// return 1 on last pattern
// return 0 on all others
//
int led_step()
{
   step++;
   if (pattern[step]==-1)
      step=0;
   set_pattern(pattern[step]);
   return pattern[step+1]== -1 ? 1 : 0;
} // led_step

void leds_off()
{
   GPIO_CLR0 = ALLEDS;
}

//
// Quick play all patterns
//
void quick_led_demo()
{ int p,r,last;
  for (p=0; p<6; p++)
  {
    // run pattern several times
    select_pattern(p);
    for (r=0; r<5; r++)
    { do {
        last = led_step();
        long_wait(3);
      } while (!last);
    } // repeat 10 times
  } // for each pattern
} // play_with_leds



