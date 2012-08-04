//
// Gertboard Demo
//
// main header file
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
// (e.g. do not use #ifndef big_demo_h....)
//
//


#include <stdio.h>

// I/O access
extern volatile unsigned *gpio;
extern volatile unsigned *pwm;
extern volatile unsigned *clk;
extern volatile unsigned *spi0;
extern volatile unsigned *uart;

void short_wait();
void long_wait(int v);

void setup_pwm();
void setup_spi();

void quick_led_demo();
void quick_pwm_demo();
void quick_buttons_demo();
void quick_adc_demo();
void adc_pwm_demo();

