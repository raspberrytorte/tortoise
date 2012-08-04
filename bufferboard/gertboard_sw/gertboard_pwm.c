//=============================================================================
//
//
// Gertboard Demo
//
// This code is part of the Gertboard demo
// Pulse-Width-Modulation part
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
// Try to strike a balance between keep code simple for
// novice programmers but still have reasonable quality code
//
//  ______        ____  __
// |  _ \ \      / /  \/  |
// | |_) \ \ /\ / /| |\/| |
// |  __/ \ V  V / | |  | |
// |_|     \_/\_/  |_|  |_|
//

// Beware of the following:
// 1/ Every write to a PWM register needs to be passed to the PWM clock
//    This may take a while and takes longer if the PWM clock is slow.
// 2/ On top of that the PWM does NOT pick up any new values unless the counter
//    reaches its end. So to fast pick up a new value you have to disable it
//    and then enable it again.
// 3/ The output polarity and reverse polarity bits are effective also if
//    the PWM is disabled but not of there is no clock

// This is how we control a motor with a single PWM channel:
//
//  |\                     /|
//  | \                   / |
//  |  \                 /  |
//  |   >--A-(Motor)-B--<   |
//  |  /                 \  |
//  | /                   \ |
//  |/                     \|
//
//
//  One direction:
//     +---+      +---+      +---+      +---+
// A   |   |      |   |      |   |      |   |
//   ==+   +======+   +======+   +======+   +=======
//
// B
//   ===============================================
// Motor gets its energy when A is high.
//
//
//  Other direction:
//   --+   +------+   +------+   +------+   +------
// A   |   |      |   |      |   |      |   |
//     +===+      +===+      +===+      +===+
//
//   ----------------------------------------------
// B
//
//
// Motor gets its reverse energy when A is low.
//
// Off is both A and B low (or high, but I use low)
//
//

#include "gertboard_demo.h"

// I/O access
extern volatile unsigned *gpio;
extern volatile unsigned *pwm;
extern volatile unsigned *clk;

#define GPIO_SET0   *(gpio+7)  // Set GPIO high bits 0-31
#define GPIO_SET1   *(gpio+8)  // Set GPIO high bits 32-53

#define GPIO_CLR0   *(gpio+10) // Set GPIO low bits 0-31
#define GPIO_CLR1   *(gpio+11) // Set GPIO low bits 32-53

#define PWMCLK_CNTL  *(clk+40)
#define PWMCLK_DIV   *(clk+41)

#define PWM_CONTROL *pwm
#define PWM_STATUS  *(pwm+1)
#define PWM0_RANGE  *(pwm+4)
#define PWM1_RANGE  *(pwm+8)
#define PWM0_DATA   *(pwm+5)
#define PWM1_DATA   *(pwm+9)

// PWM Control register bits
#define PWM1_MS_MODE    0x8000  // Run in MS mode
#define PWM1_USEFIFO    0x2000  // Data from FIFO
#define PWM1_REVPOLAR   0x1000  // Reverse polarity
#define PWM1_OFFSTATE   0x0800  // Ouput Off state
#define PWM1_REPEATFF   0x0400  // Repeat last value if FIFO empty
#define PWM1_SERIAL     0x0200  // Run in serial mode
#define PWM1_ENABLE     0x0100  // Channel Enable

#define PWM0_MS_MODE    0x0080  // Run in MS mode
#define PWM0_USEFIFO    0x0020  // Data from FIFO
#define PWM0_REVPOLAR   0x0010  // Reverse polarity
#define PWM0_OFFSTATE   0x0008  // Ouput Off state
#define PWM0_REPEATFF   0x0004  // Repeat last value if FIFO empty
#define PWM0_SERIAL     0x0002  // Run in serial mode
#define PWM0_ENABLE     0x0001  // Channel Enable

#define PWM_CLRFIFO     0x0040  // Clear FIFO (Self clearing bit)

// PWM status bits I need
#define PWMS_BUSERR     0x0100  // Register access was too fast
// (Write to clear it)


//
// Setup the Pulse Width Modulator
// It needs a clock and needs to be off
//
void setup_pwm()
{
   // Derive PWM clock direct from X-tal
   // thus any system auto-slow-down-clock-to-save-power does not effect it
   // The values below depends on the X-tal frequency!
   PWMCLK_DIV  = 0x5A000000 | (32<<12); // set pwm div to 32 (19.2/3 = 600KHz)
   PWMCLK_CNTL = 0x5A000011; // Source=osc and enable

   // Make sure it is off and that the B driver (GPIO4) is low
   GPIO_CLR0 = 1<<4; // Set GPIO 4 LOW
   PWM_CONTROL = 0;  short_wait();

   // I use 1024 steps for the PWM
   // (Just a nice value which I happen to like)
   PWM0_RANGE = 0x400;  short_wait();

} // setup_pwm

//
// Set PWM value
// This routine does not wait for the value to arrive
// If a new value comes in before it is picked up by the chip
// it will definitely be too fast for the motor to respond to it
//
void set_pwm0(int v)
{ // make sure value is in safe range
  if (v<0) v=0;
  if (v>0x400) v=0x400;
  PWM0_DATA = v;
} // set_pwm0

//
// Force PWM value update
// This routine makes sure the new value goes in.
// This is done by dis-abling the PWM, write the value
// and enable it again. This routine is weak as it
// uses a delay which is tested (but not guaranteed)
// Controls channel 0 only.
//
void force_pwm0(int v,int mode)
{ int w;
  // disable
  PWM_CONTROL  = 0;
  // wait for this command to get to the PWM clock domain
  // that depends on PWN clock speed
  // unfortunately there is no way to know when this has happened :-(
  short_wait();
  // make sure value is in safe range
  if (v<0) v=0;
  if (v>0x400) v=0x400;
  PWM0_DATA = v;
  short_wait();

  PWM_CONTROL  = mode;
  short_wait();
} // force_pwm0

void pwm_off()
{
  GPIO_CLR0 = 1<<4;
  force_pwm0(0,0);
}

void quick_pwm_demo()
{ int r,s;
  for (r=0; r<1; r++)
  {
     printf("\n>>> "); fflush(stdout);
     GPIO_CLR0 = 1<<4;
     force_pwm0(0,PWM0_ENABLE);
     for (s=0x100; s<=0x400; s+=0x10)
     { long_wait(6);
       set_pwm0(s);
       putchar('+'); fflush(stdout);
     }
     for (s=0x400; s>=0x100; s-=0x10)
     { long_wait(6);
       set_pwm0(s);
       putchar('-'); fflush(stdout);
     }
     // same in reverse direction
     GPIO_SET0 = 1<<4;
     force_pwm0(0,PWM0_ENABLE|PWM0_REVPOLAR);
     printf("\n<<< ");
     for (s=0x100; s<=0x400; s+=0x10)
     { long_wait(6);
       set_pwm0(s);
       putchar('+'); fflush(stdout);
     }
     for (s=0x400; s>=0x100; s-=0x10)
     { long_wait(6);
       set_pwm0(s);
       putchar('-'); fflush(stdout);
     }
  }
  pwm_off();
  putchar('\n');
} // quick_pwm_demo

void adc_pwm_demo()
{ int r,v,fwd;
  fwd = 1;
  GPIO_CLR0 = 1<<4;
  force_pwm0(0,PWM0_ENABLE);
  for (r=0; r<1200000; r++)
  {
    v= read_adc(0);
    // map adc 0-1023 to pwm -1023..+1023
    if (v<511)
    { // convert 0...510 to 1023..0
      v = 1023-(v * 2);
      if (fwd)
      { // going in the wrong direction
        // reverse polarity
        GPIO_SET0 = 1<<4;
        force_pwm0(v,PWM0_ENABLE|PWM0_REVPOLAR);
        fwd = 0;
      }
      else
        set_pwm0(v);
    }
    else
    { // convert 511..1023 to 0..1023
      v = (v-511)*2;
      if (!fwd)
      { // going in the wrong direction
        // reverse polarity
        GPIO_CLR0 = 1<<4;
        force_pwm0(v,PWM0_ENABLE);
        fwd = 1;
      }
      else
        set_pwm0(v);
    }
    short_wait();
  } // repeated read
  GPIO_CLR0 = 1<<4;
  force_pwm0(0,PWM0_ENABLE);
} // adc_pwm_demo
