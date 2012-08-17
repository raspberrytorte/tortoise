Installing Motion
================

Full instruction should be on the wiki, but this file explains what these files are for.

Motion Webserver
---------------

Ensure you have motion and lighttp installed.
Replace the default motion config file

    sudo mv /etc/motion/motion.conf /etc/motion.conf.backup
    sudo mv motion.conf /etc/motion/motion.conf

    sudo nano /etc/defaults/motion

edit the file to enable the daemon to yes.

