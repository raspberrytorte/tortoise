Drive
================

'drive' is a simple program to control motion of the RaspberryTortoise.
Its written in 'C' and is dependent on 'WiringPi' library.

Installation
------------

    git clone https://github.com/WiringPi/WiringPi.git

    cd wiringPi/wiringPi
    make
    sudo make install
    cd ../gpio
    make
    sudo make install
    cd ../examples
    make

drive.c
-------

compile using 

    gcc drive.c -I/usr/local/include -L/usr/local/lib -lwiringPi -o drive

Usage:

    sudo ./drive --[left right forwards backwards] time(s)

Example

    sudo ./drive --forwards 10

ledEnable.c
----------------

'ledEnable' is a programme that enables leds on the RaspberryTortoise buffer board.

compile using

    gcc ledEnable.c -I/usr/local/include -L/usr/local/lib -lwiringPi -o ledEnable

Usage:

    sudo ./ledEnable gpioPin state

where the Led to GPIO Pin mapping is

    Bufferboard J2 Pin |  GPIO   | WiringPi ID | BigTrak Motor Drive 
    01 | NA | NA | 
    02 | NA | NA | 
    03 | 00 | 08 | 
    04 | 01 | 09 | 
    05 | 04 | 07 | 
    06 | 17 | 00 | 
    07 | 18 | 01 | 
    08 | 21 | 02 | 
    09 | 22 | 03 | 
    10 | 23 | 04 |

'state' is 1 for 'on', 0 for 'off'.

Example:
To enable bufferboard LED 6 on J2 Pin 7, address the LED via its correct WiringPi ID

    sudo ./ledEnable 1 1

to switch it off again

    sudo ./ledEnable 1 0


