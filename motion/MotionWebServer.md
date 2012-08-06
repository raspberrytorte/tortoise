Installing Motion
================

Full instruction should be on the wiki, but this file explains what these files are for.

Motion Webserver
................

Ensure you have motion and lighttp installed.
Replace the default motion config file

   sudo mv /etc/motion/motion.conf /etc/motion.conf.backup
   sudo mv motion.conf /etc/motion/motion.conf

Lighttp Webpages
................

This create a very simple webpage to display the video stream from motion.

   sudo mkdir /var/www/motion
   sudo mkdir /var/www/static
   sudo cp motion.html /var/www/motion/
   sudo cp style.css /var/www/static


