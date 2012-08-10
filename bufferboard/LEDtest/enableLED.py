#!/usr/bin/python3

import sys
import os
sys.path.insert(0, '/home/jack/quick2wire-python-api/src')

from time import sleep
from quick2wire.gpio import Pin
from switch import switch
## import switch

LED = int(sys.argv[1]) if len(sys.argv) > 1 else 1
ontime = int(sys.argv[2]) if len(sys.argv) > 2 else 5



for case in switch(LED):
    if case(1):
        pin = Pin(3, Pin.Out); break
    if case(2):
        pin = Pin(5, Pin.Out); break
    if case(3):
        pin = Pin(7, Pin.Out); break
    if case(4):
        pin = Pin(11, Pin.Out); break
    if case(5):
        pin = Pin(12, Pin.Out); break
    if case(6):
        pin = Pin(13, Pin.Out); break
    if case(7):
        pin = Pin(15, Pin.Out); break
    if case(8):
        pin = Pin(16, Pin.Out); break
    if case(): # default, could also just omit condition or 'if True'
        pin = Pin(3, Pin.Out);
        # No need to break here, it'll stop anyway

print LED
print pin

try:
    pin.value = 1 ## set pin.value = 1 for on
    for count in range(0, ontime): ## wait for required time
        sleep(1) ## waiting!
    pin.value = 0 ## 1 - pin.value ## set to zero for off
    pin.unexport() ## release the pin on exit
except KeyboardInterrupt: ## what to do if the programme breaks or ctrl-c
    pin.value = 0
    pin.unexport()


