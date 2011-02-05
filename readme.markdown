gnome-hdaps-applet
==================

This is a simple applet that monitors `/sys/block/sda/device/unload_heads` or
`/sys/block/hda/device/unload_heads` and displays the status of these files.
Useful for checking if you configured HDAPS correctly...

The [original applet](http://www.zen24593.zen.co.uk/hdaps/) is by Jon Escombe,
this improved version uses GIO file monitor instead of pulling sysfs every 100ms,
this reduces the CPU and power consumption on modern kernels with dynamic ticks.
The pretty [ubuntu-mono-icons](http://gnome-look.org/content/show.php?content=122887)
are by Stefan Hammer and can be downloaded from.

Packages
========

A PPA for Ubuntu maverick can be added with the following command:

 *  sudo add-apt-repository [ppa:jopsen/gnome-hdaps-applet](https://launchpad.net/~jopsen/+archive/gnome-hdaps-applet)

Authors
=======
 *  Jon Escombe (Original applet)
 *  Stefan Hammer (Pretty Icons)
 *  Jonas Finnemann Jensen (File monitoring)

