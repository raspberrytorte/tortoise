#
# Makefile for big gertboard demo
#
#
# Copyright (C) Gert Jan van Loo 2012
# No rights reserved
# You may treat this program as if it was in the public domain
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# Keep things simple for novice users
# so do not use any implicit rules!
#

#combine all object files into an excutable
gertboard_demo : gertboard_demo.o gertboard_leds.o gertboard_pwm.o gertboard_spi.o gertboard_buttons.o
	gcc -o gertboard_demo gertboard_demo.o gertboard_leds.o gertboard_pwm.o gertboard_spi.o gertboard_buttons.o

# The next lines generate the various object files

gertboard_demo.o : gertboard_demo.c gertboard_demo.h
	gcc -c gertboard_demo.c

gertboard_leds.o : gertboard_leds.c gertboard_demo.h
	gcc -c gertboard_leds.c

gertboard_buttons.o : gertboard_buttons.c gertboard_demo.h
	gcc -c gertboard_buttons.c

gertboard_pwm.o : gertboard_pwm.c gertboard_demo.h
	gcc -c gertboard_pwm.c

gertboard_spi.o : gertboard_spi.c gertboard_demo.h
	gcc -c gertboard_spi.c


