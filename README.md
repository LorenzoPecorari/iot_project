# IOT AS PROJECT - "WHAT'S THE MOOD?"

# Students that work to this project:
- Michele Nicoletti, id 1886646, nicoletti.1886646@studenti.uniroma1.it, https://www.linkedin.com/in/michele-nicoletti-55066b2b0
- Lorenzo Pecorari, id 1885161, pecorari.1885161@studenti.unirioma1.it, www.linkedin.com/in/lorenzo-pecorari-683900302

# Description of project
An IoT project for monitoring air and temperature inside a classroom. There will be two types of smart objects as:
- Central device, one for each room;
- Helper device, at least one for each room.

These devices have to sample some parameters of the environment as CO2, humidity and temperature of the air; on the basis of the sampled data, they will act on the environment through actuators as: 
- a servomotor for operning/closing a window;
- a fan that simulates a conditioner.

Heleper(s) will send the sampled information to Central device that have to makes some caluclations on them; after that, this last one will send data to the cloud via Internet. Communications between the MCUs are done through the protocol "ESP-NOW".

Devices will mainly work from 8am to 8pm, a time period where lectures are usually done. In this period, MCUs will follow a dynamic awake/asleep schedule: the sleep time is about 10 minutes, after which Central checks if there are some noises and Helpers ask to it if they have to start to sample data. If no noises are detected, all devices go back to sleep; although they start to act as usual.

Central device is connected to continuous power supply while Helpers have a battery.


# Sensors and external hw
	- air quality module
	- microphone
	- leds
	- fan
	- servomotor
	- battery (or batteries)

