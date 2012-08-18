RaspberryTortoise WebControl
============================

node.js is used to provide the web interface for the control of the raspberryTortoise.

Installation
------------

    sudo apt-get nodejs git
    cd ~
    git clone https://github.com/raspberrytorte/tortoise.git

Run Manually
------------

    sudo node ~/tortoise/nodejs/raspberryTortoise.js ServerIP ServerPort drivePath

where:

serverIP : default 192.168.0.10,  is the address of the webserver, this should be the same address used by the server in motion.

serverPort : default 8080, is the port of the webserver.

drivePath : default "../drive" , the path to the folder containing the motor drive code. If you use a relative path it should be routed from the directory where you manually call node. If you run node automatically, use the full path.

Run at Startup
--------------

to run the server automatically when you power up the RPi

    sudo nano /etc/rc.local

add the line

    nano <pathToTortoise>/nodejs/raspberryTortoise.js ServerIP ServerPort drivePath

Security Note
-------------

In order to execute motor drive commands the node needs to run with SUDO privilges.

This is very nasty and has security implications. 

Be very careful if you deploy this on an RPi that is web connected.

Webserver
---------

Using your browser navigate to http://ServerIP:ServerPort

example:

    http://192.168.0.10:8080
