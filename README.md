# IOT AS - PROJECT

# Students that work to this project:
- Michele Nicoletti, id 1886646, nicoletti.1886646@studenti.uniroma1.it, https://www.linkedin.com/in/michele-nicoletti-55066b2b0
- Lorenzo Pecorari, id 1885161, pecorari.1885161@studenti.unirioma1.it, www.linkedin.com/in/lorenzo-pecorari-683900302

# Description of project
An IoT project for monitoring air and temperature inside a classroom. There will be two types of smart objects as:
- central device, one for each room;
- helper device, at least one for each room.
These devices have to sample some parameters of the environment as CO2, humidity and temperature of the air; on the basis of the sampled data, they will act on the environment through some actuators as a servomotor for operning/closing a window and a fan that simulates a conditioner. The central device will send data to the cloud, the heleper(s) will send the sampled information to the central device that have to makes some caluclations on them.
Communications between the MCUs are done through the protocol "ESP-NOW" 

# Sensors and external hw
	- air quality module
	- microphone
	- leds
	- (computer) fan
	- servomotor
	- battery (or batteries)

