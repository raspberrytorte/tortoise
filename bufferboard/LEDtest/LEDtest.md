LEDtest
========

Collection of python scripts that use quick2wire python API and admin tools.

Depedencies
-----------

https://github.com/quick2wire/quick2wire-python-api

https://github.com/quick2wire/quick2wire-gpio-admin

add the quick2wire libraries to the python path as per the quick2wire doc

    export QUICK2WIRE_API_HOME=<wherever you installed quick2wire>
    export PYTHONPATH=$PYTHONPATH:$QUICK2WIRE_API_HOME/src

Scripts
-------

    python blink.py pin
flashes the defined pin, copied from the quick2wire simple example, ctrl-c required to break the code.

    python blinkLED.py LED numblinks
example controling the number of blinks, has LED number linked to the tortiose bufferboard pinlayout using a simple lookup

    python enableLED.py LED ontime
script that add the path to the quick2wire api automatically before running. Remeber to edit the path before running this!

    switch.py
adding a switch case statement to python
