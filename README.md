# autothrottle
daemon that limits the maximum cpu frequency in order to avoid overheating the cpu

### Explanation
If your computer is overheating or simply exceeds the temperature range you are comfortable with, a possible solution would be to limit the frequency at which the cpu runs.
For example, you may keep check on the temperature and manually run ````cpupower frequency-set --max <frequency>```` if you have the cpupower package installed.
Using lower maximum frequencies aid significantly in cooling the cpu.
Ideally, you would find the max frequency at which the cpu stays below a desired temperature.
When cpu usage varies, however, the optimal cpu frequency changes too.
autothrottle attempts to automate the process of setting the cpu frequency by reactively lowering the cpu frequency as temperature increases, and raising it as temperature decreases.
For this, it implements a [PID controller](https://en.wikipedia.org/wiki/PID_controller)

![autothrottle in action](example.png)

### Warnings
This tool will attempt to stabilize the cpu temperature to a given point, by changing the max cpu frequency through the linux kernels sysfs.
It is mainly intended to replace setting the max cpu frequency by hand.
Temperatures _will_ exceed the set maximum temperature (although hopefully not for too long) as the program cannot predict ahead in any way.
Do not use this program as a replacement for standard cpu cooling mechanisms such as fans (although I have had reasonable success using this program alongside a broken laptop fan).
The PID controller that sets the max frequency will have to be tuned manually for each system.

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
