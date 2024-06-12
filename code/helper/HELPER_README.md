# Helper device #

The device has the aim to provide functionalities that concern abaout imporvement of the quality of the room. Even if can be considered a variant of the central device, it interacts differently with the environment becuase of the actuators it is provided.

## Hardware ##
The needed hardware requirement for this node os the following: is the following:
<li>Espressif ESP32-S3 v3.1 (microcontroller unit)</li>
<li>MQ-2 air and gas quality sensor</li>
<li>Thermistor</li>
<li>Servomotor 5V (attendere modello preciso)</li>
<li>Cooling fan 5v Dc 3010 (2 pin version)</li>
<li>Blue and red LEDs</li>
<li>S8050 transistor</li>
<li>breadboard</li>

### Hardware setup ###
Each of these sensors and actuators have their own cofigured pin, here the list:
<table>
    <tr>
        <th>COMPONENT</th>
        <th>GPIO pin</th>
        <th>VCC</th>
    </tr>
    <tr>
        <td>MQ-2</td>
        <td>GPIO_NUM_1</td>
        <td>5V</td>
    <tr>
    <tr>
        <td>S8050</td>
        <td>GPIO_NUM_2</td>
        <td>5V</td>
    <tr>
    <tr>
        <td>Thermistor</td>
        <td>GPIO_NUM_3</td>
        <td>3.3V</td>
    <tr>
        <tr>
        <td>Red LED</td>
        <td>GPIO_NUM_4</td>
        <td>3.3V</td>
    <tr>
    <tr>
        <td>Blue LED</td>
        <td>GPIO_NUM_5</td>
        <td>3.3V</td>
    <tr>
    <tr>
        <td>Servomotor</td>
        <td>GPIO_NUM_6 (?)</td>
        <td>5V</td>
    </tr>
</table>

// TO DISCUSS WITH MICHELE // 

### Details about components ###
#### MQ-2 ####
This is the air-quality detection sensor used for monitoring how  stale the air is inside the room. For this component it was adopted a sampling of the analog output provided by in different situations for estabilishing a which level is needed to interact with the environment.

#### Thermistor ####

## Software ##
The required software and libraries used for developing the node are the following:
<li>FreeRTOS</li>
<li>ESP-IDF</li>
<li>ESP-NOW</li>

