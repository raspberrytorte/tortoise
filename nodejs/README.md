nodejs
================

node provides the web interface for the control of the raspberryTortoise.

Installation
---------------

   sudo apt-get nodejs
   cd ~/tortoise/nodejs 
   nano raspberryTortoise.js

search and replace IP addresses to match your server IP address

   sudo nohup node raspberryTortoise.js

note this sudo privs are required for the tortoise drive system. 

This is very nasty and has security implications. 

Be very careful if you deploy this on an RPi that is web connected.

Using you browser navigate to http://<yourRPiIP>:8080

Should work even if you don't have the video Tortoise available.


