//
// Gertboard Demo
//
// This code is part of the Gertboard demo
// This routine read the buttons
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
// Try to strike a balance between keep code simple for
// novice programmers but still have reasonable quality code
//


#include "gertboard_demo.h"

#define GPIO_IN0   *(gpio+13)  // Reads GPIO input bits 0-31

void quick_buttons_demo()
{ int r,d;
  unsigned int b,prev_b;
  prev_b = 4;
  r = 30;
  while (r)
  {
    b = GPIO_IN0;
    b = (b >> 24 ) & 0x03; // keep only bits 24 & 25
    if (b^prev_b)
    { // one or more buttons changed
      printf("\n\n\n");
      for (d=0; d<6; d++)
      {
        if (b & 0x01)
          printf("  ######  ");
        else
          printf("          ");
        if (b & 0x02)
          printf("  ######\n");
        else
          printf("        \n");
      }
      printf("  ################\n");
      prev_b = b;
      r--;
    } // change
  } // repeat
} // quick_buttons



