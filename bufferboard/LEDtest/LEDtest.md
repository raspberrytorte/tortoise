LEDtest
========

Collection of python scripts that use quick2wire python API and admin tools.

Depedencies
-----------
quick2wire-python-api
quick2wire-admin

add the quick2wire libraries to the python path as per the quick2wire doc


export QUICK2WIRE_API_HOME=<wherever you installed quick2wire>
export PYTHONPATH=$PYTHONPATH:$QUICK2WIRE_API_HOME/src

File List

blink.py pin
flashes the defined pin, copied from the quick2wire simple example, ctrl-c required to break the code.

blinkLED.py LED numblinks
example controling the number of blinks, has LED number linked to the tortiose bufferboard pinlayout using a simple lookup

enableLED.py
script that add the path to the quick2wire api automatically before running.

switch.py
adding a switch case statement to python
