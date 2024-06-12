# Helper device #

The device has the aim to provide functionalities that concern about improvement of the quality of the room. Even if can be considered a variant of the central device, it interacts differently with the environment becuase of the actuators it is provided. It can be considered as the slave ndoe of the architecture of the project.

## Hardware ##
Other than a Espressif ESP32-S3 v3.1 and a breadboard, for the project is needed:

<table>
    <tr>
        <th>Component</th>
        <th>GPIO pin</th>
        <th>VCC</th>
        <th>Description</th>
    </tr>
    <tr>
        <td>MQ-2</td>
        <td>GPIO_NUM_1</td>
        <td>5V</td>
        <td>Sensor for detecting air quality</td>
    <tr>
    <tr>
        <td>S8050</td>
        <td>GPIO_NUM_2</td>
        <td>5V</td>
        <td>NPN transistor for managing fan usage</td>
    <tr>
    <tr>
        <td>Thermistor</td>
        <td>GPIO_NUM_3</td>
        <td>3.3V</td>
        <td>Restistor that changes its internal resistance wrt temperature</td>
    <tr>
        <tr>
        <td>Red LED</td>
        <td>GPIO_NUM_4</td>
        <td>3.3V</td>
        <td>Simple red LED for alerting about what fan is being used for</td>
    <tr>
    <tr>
        <td>Blue LED</td>
        <td>GPIO_NUM_5</td>
        <td>3.3V</td>
        <td>Simple blue LED for alerting about what fan is being used for</td>
    <tr>
    <tr>
        <td>Servomotor</td>
        <td>GPIO_NUM_6</td>
        <td>5V</td>
        <td>Actuators for managing the opening of the window</td>
    </tr>
        <td>Cooling fan</td>
        <td>None</td>
        <td>5V</td>
        <td>Fan used to simulate a conditioner for heating or cooling the room</td>
    <tr>
</table>

For each component is reported the pin to connect with, input tension and a short description about its utility.

### Hardware configuration ###

<img src = "./helper_hw_configuration.jpeg">

#### MQ-2 ####
This is the air-quality detection sensor used for monitoring how stale the air is inside the room. For this component it is adopted a sampling of the analog output provided as output in different situations for estabilishing a which level is needed to interact with the environment. Even if it provides also a digital output, it is preferred to use the analog one for threshold compatibilities.

#### NPN S8050 transistor and cooling fan ####
NPN transistor that is capable to amplify and/or commute signals for small devices like these that are used for the project. Using the signal coming from the GPIO pin, it can be used to manage the functioning of the cooling fan managed by the ESP32. So, collector is collector is linked to the negative connector of the fan, the base to the GPIO of the mcu and emitter to ground.

The fan is controlled through the previously described component and its capable to (virtually!) decrease or increase the temperature. Differently from some others fans, it has no possibility to change the speed of the fan due to lack of third pin.

#### NTC Thermistor ####
Component that belongs to family of semiconductors that changes its internal resistance with different values of temperature. For this project, it covers a relevant role because of it allows to detecte if is needed to activate the conditioner for restoring the temperature of the room to acceptable level. Also for this sensor, it is used an analog signal for getting data.

#### Servomotor ####
Actuator capable to interact with something movable that can improve the quality of the air. Its functioning is based on the usage of PWM signals generated through the ESP32 by modifing a value that represent the position and changing the period of the signal itself.



## Software ##
The required main and relevant software for making the project working are:
<table>
    <tr>
        <th>Software</th>
        <th>Description</th>
    </tr>
    <tr>
        <td>
            FreeRTOS
        </td>
        <td>
            Open source real time operating system suitable for microcontrollers and embedded systems; it provides multitasking, interrupts, synchronization, memory management and instructions scheduling.
        </td>
    </tr>
    <tr>
        <td>
            ESP-IDF
        </td>
        <td>
            Official Espressif development framework for ESP32 microcontrollers; it relies on FreeRTOS, allowing all its benefits and functionalities.
        </td>
    </tr>
    <tr>
        <td>
            ESP-NOW
        </td>
        <td>
            Wireless protocol communication for Espressif devices; it provides E2E communication with low latency, high energetic efficiency and high scalability.
        </td>
    </tr>
</table>

### Data sampling ###
By using the Analaog-to-Digital converter and the General Purpose Input-Output pins (GPIOs), it is possible to sample date from sensor and sending signals for handling actuators. In particular, after initializing the relative <code>esp_adc_cal_characteristics_t</code> structure for a certain pin using the library functions, it is needed to set the channel for detecting signals: for each of the sensor is related a channel for avoiding interferences or other issues. Once this configuration, for analog signals is needed the funcion <code>esp_adc_cal_raw_to_voltage()</code> that takes as input the raw signal from the configured ADC unit and a pointer to the structure previously initializated to it.

### Communication with other peer ###
Even if the project uses a master-slave approach, the communication between nodes happens like a P2P network with some phases and, in particular, for helper are mainly the following ones. 

#### MAC exchange #### 
It uses the callbacks <code>send_cb</code> and <code>recv_cb</code> and the function <code>send_mac</code> for exchanging its own MAC address with central node, consume the message by the usage of <code>conusme_message</code> that detects the type of the packed received/send; once get the mac, it will be set by the function <code>set_peer</code> with argument the variable containing central MAC<

#### Sampling phase #### 
Once received the approvation for sampling datas, the dedicated function <code>get_values</code> starts to collect informations from the environment, 10 samplings for temperature and other 10 for air in orrder to get an arithmetical mean of each of them over a minute; after the computation of those, data will be send to central and it will wait for other information by it

#### Reception of averaged data and interaction with environment #### 
After received information from central with an average of collected data from each of them, this device will elaborate them and decides what to do. Elaboration of data consists of a parsing of the payload that is inside the packet sent back by central device. After that operation, if temperature and quality of air are above or below a certain threshold, helper will act using the fan and the servomotor to improve the quality of the room.

#### Sleep mode time ####
For reducing energy consumption of the this node, the system is developed for using a light sleep mode: it consists of a low-consume condition for the processor that cuts off a lot of power to components, reducing availability of functionalities while maintaing a restricted set of information in memory. After receiving a packet with no need to compute the sampling phase or terminated the actions to do with actuators, the mcu will go to sleep for 10 minutes.  

