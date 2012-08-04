//
// Gertboard Demo
//
// SPI (ADC/DAC) control code
//
// This code is part of the Gertboard demo
// These routines access the AD and DA chips
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

#include "gertboard_demo.h"

extern volatile unsigned *spi0;

////  SPI
//
#define SPI0_CNTLSTAT *(spi0 + 0)
#define SPI0_FIFO     *(spi0 + 1)
#define SPI0_CLKSPEED *(spi0 + 2)

// SPI0_CNTLSTAT register bits

#define SPI0_CS_CS2ACTHIGH   0x00800000 // CS2 active high
#define SPI0_CS_CS1ACTHIGH   0x00400000 // CS1 active high
#define SPI0_CS_CS0ACTHIGH   0x00200000 // CS0 active high
#define SPI0_CS_RXFIFOFULL   0x00100000 // Receive FIFO full
#define SPI0_CS_RXFIFO3_4    0x00080000 // Receive FIFO 3/4 full
#define SPI0_CS_TXFIFOSPCE   0x00040000 // Transmit FIFO has space
#define SPI0_CS_RXFIFODATA   0x00020000 // Receive FIFO has data
#define SPI0_CS_DONE         0x00010000 // SPI transfer done. WRT to CLR!
#define SPI0_CS_MOSI_INPUT   0x00001000 // MOSI is input, read from MOSI (BI-dir mode)
#define SPI0_CS_DEASRT_CS    0x00000800 // De-assert CS at end
#define SPI0_CS_RX_IRQ       0x00000400 // Receive irq enable
#define SPI0_CS_DONE_IRQ     0x00000200 // irq when done
#define SPI0_CS_DMA_ENABLE   0x00000100 // Run in DMA mode
#define SPI0_CS_ACTIVATE     0x00000080 // Activate: be high before starting
#define SPI0_CS_CS_POLARIT   0x00000040 // Chip selects active high
#define SPI0_CS_CLRTXFIFO    0x00000020 // Clear TX FIFO    (auto clear bit)
#define SPI0_CS_CLRRXFIFO    0x00000010 // Clear RX FIFO    (auto clear bit)
#define SPI0_CS_CLRFIFOS     0x00000030 // Clear BOTH FIFOs (auto clear bit)
#define SPI0_CS_CLK_IDLHI    0x00000008 // Clock pin is high when idle
#define SPI0_CS_CLKTRANS     0x00000004 // 0=first clock in middle of data bit
                                        // 1=first clock at begin of data bit
#define SPI0_CS_CHIPSEL0     0x00000000 // Use chip select 0
#define SPI0_CS_CHIPSEL1     0x00000001 // Use chip select 1
#define SPI0_CS_CHIPSEL2     0x00000002 // No chip select (e.g. use GPIO pin)
#define SPI0_CS_CHIPSELN     0x00000003 // No chip select (e.g. use GPIO pin)

#define SPI0_CS_CLRALL      (SPI0_CS_CLRFIFOS|SPI0_CS_DONE)


//
// Set-up the SPI interface
//
// Speed depends on what you talk to
// In this case use 1MHz
//
void setup_spi()
{
  // Want to have 1 MHz SPI clock.
  // Assume 250 Mhz system clock
  // So divide 250MHz system clock by 250 to get 1MHz45
  SPI0_CLKSPEED = 250;

  // clear FIFOs and all status bits
  SPI0_CNTLSTAT = SPI0_CS_CLRALL;
  SPI0_CNTLSTAT = SPI0_CS_DONE; // make sure done bit is cleared
} // setup_spi()



//
// Read a value from one of the two ADC channels
//
// To understand this code you had better read the
// datasheet of the AD chip (MCP3002)
//
int read_adc(int chan) // 'chan' must be 0 or 1. This is not checked!
{ unsigned char v1,v2,rec_c;
  int status,w;
  // Set up for single ended, MS comes out first
  v1 = 0xD0 | (chan<<5);
  // Delay to make sure chip select is high for a short while
  short_wait();

  // Enable SPI interface: Use CS 0 and set activate bit
  SPI0_CNTLSTAT = SPI0_CS_CHIPSEL0|SPI0_CS_ACTIVATE;

  // Write the command into the FIFO so it will
  // be transmitted out of the SPI interface to the ADC
  // We need a 16-bit transfer so we send a command byte
  // folowed by a dummy byte
  SPI0_FIFO = v1;
  SPI0_FIFO = 0; // dummy

  // wait for SPI to be ready
  // This will take about 16 micro seconds
  do {
     status = SPI0_CNTLSTAT;
  } while ((status & SPI0_CS_DONE)==0);
  SPI0_CNTLSTAT = SPI0_CS_DONE; // clear the done bit

  // Data from the ADC chip should now be in the receiver
  // read the received data
  v1 = SPI0_FIFO;
  v2 = SPI0_FIFO;
  // Combine the 8-bit and 2 bit values into an 10-bit integer
  // NOT!!!  return ((v1<<8)|v2)&0x3FF;
  // I have checked the result and it returns 3 bits in the MS byte not 2!!
  // So I might have my SPI clock/data pahse wrong.
  // For now its easier to dadpt the results (running out of time)
  return ( (v1<<7) | (v2>>1) ) & 0x3FF;
} // read_adc

//
// Write 12 bit value to DAC channel 0 or 1
//
// To understand this code you had better read the
// datasheet of the AD chip (MCP4802/MCP4812/MCP4822)
//
void write_dac(int chan, // chan must be 0 or 1, this is not checked
                int val) // chan must be max 12 bit
{ char v1,v2,dummy;
  int status;
  val &= 0xFFF;  // force value in 12 bits

  // Build the first byte: write, channel 0 or 1 bit
  // and the 4 most significant data bits
  v1 = 0x30 | (chan<<7) | (val>>8);
  // Remain the Least Significant 8 data bits
  v2 = val & 0xFF;

  // Delay to have CS high for a short while
  short_wait();

  // Enable SPI: Use CS 1 and set activate bit
  SPI0_CNTLSTAT = SPI0_CS_CHIPSEL1|SPI0_CS_ACTIVATE;

  // send the values
  SPI0_FIFO = v1;
  SPI0_FIFO = v2;

  // wait for SPI to be ready
  // This will take about 16 micro seconds
  do {
     status = SPI0_CNTLSTAT;
  } while ((status & SPI0_CS_DONE)==0);
  SPI0_CNTLSTAT = SPI0_CS_DONE; // clear the done bit

  // For every transmit there is also data coming back
  // We MUST read that received data from the FIFO
  // even if we do not use it!
  dummy = SPI0_FIFO;
  dummy = SPI0_FIFO;
} // write_dac


//
//  Read ADC input 0 and show as horizontal bar
//
void quick_adc_demo()
{ int r,v,l;
  for (r=0; r<150000; r++)
  {
    v= read_adc(0);
    // V should be in range 0-1023
    // map to 0-63
    printf("%04d ",v);
    v >>= 4;
    l= v;
    // show horizontal bar
    while (l--)
      putchar('#');
    l = 64 - v;
    while (l--)
      putchar(' ');
    putchar(0x0D); // go to star of the line
    short_wait();
  } // repeated read
} // quick_adc_demo
