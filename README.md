# autothrottle
daemon that limits the maximum cpu frequency in order to avoid overheating the cpu

![autothrottle in action](example.png)

### Warnings
This tool will attempt to stabilize the cpu temperature to a given point, by changing the max cpu frequency through the linux kernels sysfs.
It is mainly intended to replace setting the max cpu frequency by hand.
Do not use this program as a replacement for de facto cpu cooling mechanisms such as fans (although I have had reasonable success using this program alongside a broken laptop fan).
The PID controller that sets the max frequency will probably have to be tuned manually.

### Compiling
````
git clone https://github.com/d3da/autothrottle
cd autothrottle
make
sudo make install
````
Do not forget to edit the configuration file ````/etc/autothrottle.conf```` and be sure to [tweak](https://en.wikipedia.org/wiki/PID_controller#Manual_tuning) the PID parameters for your system.

### Usage
````
sudo ./autothrottle
````
or, to run in debug mode:
````
sudo ./autothrottle --no-daemon
````

### Openrc
An openrc-init script is provided to allow gentoo users to add the daemon as a system service for openrc.

### Plot
The ````plot/```` subdirectory contains a python program to create a quick and dirty graph such as the one shown above.
