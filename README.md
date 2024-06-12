# INTERNET-OF-THINGS ALGORITHMS AND SERVICES PROJECT - "WHAT'S THE MOOD?"

# Students that work to this project
- Michele Nicoletti
	-  student id: 1886646
 	-  email: nicoletti.1886646@studenti.uniroma1.it
  	-  LinkedIn profile: https://www.linkedin.com/in/michele-nicoletti-55066b2b0

- Lorenzo Pecorari
	- student id: 1885161
 	- email: pecorari.1885161@studenti.unirioma1.it
  	- LinkedIn profile: https://www.linkedin.com/in/lorenzo-pecorari-683900302

# Description of project
An IoT project for monitoring air and temperature inside a classroom. There will be two types of smart objects as:
- Central device, one for each room;
- Helper device, at least one for each room.

These devices have to sample some parameters of the environment as CO2, humidity and temperature of the air; on the basis of the sampled data, they will act on the environment through actuators as: 
- a servomotor for operning/closing a window;
- a fan that simulates a conditioner.

Heleper(s) will send the sampled information to Central device that have to makes some caluclations on them; after that, this last one will send data to the AWS cloud via Internet. Communications between the MCUs are done through the protocol "ESP-NOW".

Devices will mainly work from 8am to 8pm, a time period where lectures are usually done. In this period, MCUs will follow a dynamic awake/asleep schedule: the sleep time is about 10 minutes, after which Central checks if there are some noises and Helpers ask to it if they have to start to sample data. If no noises are detected, all devices go back to sleep; although they start to act as usual.

The data sent from Central to the cloud will be provided by a remote terminal which can be accessed for knowing the quality of the air and other information.

Central device is connected to continuous power supply while Helpers have a battery.

# MicroController Unit
- ESP32-S3

# Sensors and external hw
- Air quality module
- Microphone
- Leds
- Fan
- Servomotor
- Battery (or batteries)

# External service
- Amazon Web Services
