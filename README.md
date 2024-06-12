<h1>INTERNET-OF-THINGS ALGORITHMS AND SERVICES PROJECT - "WHAT'S THE MOOD?"</h1>

<h2>Students that work to this project</h2>

- Michele Nicoletti
	-  student id: 1886646
 	-  email: nicoletti.1886646@studenti.uniroma1.it
  	-  LinkedIn profile: https://www.linkedin.com/in/michele-nicoletti-55066b2b0

- Lorenzo Pecorari
	- student id: 1885161
 	- email: pecorari.1885161@studenti.unirioma1.it
  	- LinkedIn profile: www.linkedin.com/in/lorenzo-pecorari-683900302

<h2>Description of project</h2>
An IoT project for monitoring air and temperature inside a classroom. There will be two types of smart objects as:
- Central device, one for each room;
- Helper device, at least one for each room.

These devices have to sample some parameters of the environment as temperature and quality of the air; on the basis of the sampled data, they will act on the environment through actuators as: 
- a servomotor for operning/closing a window;
- a fan that simulates a conditioner.

Heleper(s) will send the sampled information to Central device that have to makes some caluclations on them; after that, this last one will send data to a MQTT broker. Communications between the MCUs are done through the protocol "ESP-NOW".

 MCUs will follow a dynamic awake/asleep schedule: the sleep time is about 10 minutes, after which Central checks if there are some noises and Helpers wait for starting to sample data. If no noises are detected, all devices go back to sleep; although they start to act as usual.

The data sent from Central to the broker will be provided by a remote terminal which can be accessed for knowing the quality of the air and other information.

Central device is connected to continuous power supply, while Helpers may have a battery.

<h2>MicroController Unit</h2>

- ESP32-S3: https://amzn.eu/d/cchmXop

<h2>Sensors and external hardware</h2>

- Air quality module: https://amzn.eu/d/0QER94N
- Microphone: https://amzn.eu/d/gcEBj6b
- Leds, cables, resistences, breadboard: https://amzn.eu/d/eTA6slE
- Fan: https://amzn.eu/d/cga2CR7
- Servomotor: https://amzn.eu/d/b9mI92F

<h2>External service</h2>

- HiveMQ as external broker

<h2>Devices details</h2>

Each device has its own <code>README.md</code>. To see the details of them, open the directory the devices and find the informations you need.