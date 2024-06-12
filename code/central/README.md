<h1>What's the mood - CENTRAL DEVICE</h1>
<i>Central device</i> is the main node of this IoT system. It's the bridge between the IoT network and the external broker. It manages the execution of the <i>helpers device</i>, giving them the instructions for starting the data sampling and the general result of all data sampled on the system. This result is evalueted computing the average of the data sampled by all the system devices. The average is shared to helper devices and a mqtt broker (<i>HiveMQ</i>).

<h2>Hardware configuration</h2>
Hardware element required for the central device implementation are:

- <b>ESP32S3</b>
- <b>breadboard</b>
- <b>MQ2 module</b>, the air module
- <b>AZDelivery KY-038 module</b>, the microphone
- <b>Thermistor</b>
- <b>4 leds</b>, one green, one yellow, one red and one blue
- <b>5 resistence</b>, on 10Ω, three 1kΩ and one 100kΩ
- <b>cables</b>, at least 17

The first step is to set the ESP on the top of breadboard and connect the 5V pin to the <i>left red line</i> of the breadboard, the left ESP ground to the <i>left blue line</i>, the 3V pin to <i>right red line</i> and the right ESP ground to the <i>right blue line</i>.

Set the microphone on the breadboard, connect its <i>positive pin</i> to the 5V line and <i>GND pin</i> one of ground lines. Connect <i>digital output (DO)</i> to the <i>GPIO1</i> of the ESP. Do the same for the air module, but for the sensor output use the <i>analog output (AO)</i> instead of the digital one. Connect this pin to the <i>GPIO2</i> of the ESP.

For the thermistor, set it on the breadboard and connect one of its leg to 3V line. On the orizzontal line of the other leg, insert the cable that connects the sensor to the ESP <i>GPIO3</i> pin and one of the leg 100kΩ resistence. The other part of the resistence is connected to a ground line.

Leds have the same configuration: the shortest led pin is connected to a ground line, the longest one is connected to a resistence that has also a connection to a <i>GPIO</i> pin of ESP. Each led has its resistence and its ESP pin, as shown in the following table.

<table>
    <tr>
        <th>LED</th>
        <th>RESISTENCE</th>
        <th>GPIO</th>
    </tr>
    <tr>
        <td>GREEN</td>
        <td>10Ω</td>
        <td>4</td>
    </tr>
    <tr>
        <td>YELLOW</td>
        <td>1kΩ</td>
        <td>5</td>
    </tr>
    <tr>
        <td>BLUE</td>
        <td>1kΩ</td>
        <td>6</td>
    </tr>
    <tr>
        <td>RED</td>
        <td>1kΩ</td>
        <td>7</td>
    </tr>
</table>

The hardware configuration could be as follow
<img src="README_pictures/hw_configuration.jpeg">


<h3>Sensors analysis</h3>

Central has sensors and leds. It hasn't actuators because only the helpers intercat directly with the environment. Sensors used by central are:

- <b>a microphone</b>, with which the device checks if there is someone inside the classroom;
- <b>an air module</b>, the hardware used to get air data;
- <b>a thermistor</b>, a resistence that changes its value with room temperature.

More details of these sensors are on <code>README.md</code> of repository homepage.

Microphone value is taken on it's <i>digital output</i>, that will give an <i>high value</i>(1) if it detects a noise over a specific decibel value, otherwise it will give a <i>lower value</i> (0).

Air module has to give values on air quality, with an high range of values. For this reason, the output is taken on <i>analogic output</i> of the module.

Thermistor is a resistence with a value that changes with of little temperature changement of the place where is located. To detect this changement, thermistor has of its pin connect tho ESP pin.

The leds report a visual average of data sampled. They are four, two for air values and two for temperature:

- <b>green led</b> indicates a good qaulity of the air;
- <b>yellow led</b> indicates a bad quality of the air;
- <b>red led</b> indicates a high temeperature detected;
- <b>blue led </b> indicates a òpw temperature detected.

Led status will change each time data sampling is done.

<h2>Build and execution</h2>

To start central device, its code has to be <i>built</i>. Open a ESP-IDF terminal, that gives the possbility to execute the program <code>idf.py</code> (execution manager of the program loaded on an ESP device).

The first command to execute is <code>idf.py build</code>, which checks if there are errors on the written code and compiles the code. When the code is ready to be executed, it has to be loaded on the microcontroller with the command <code>idf.py flash</code>. In the end, for the program execution, the command to use is <code>idf.py monitor</code>.

The terminal used for program execution has to be opened on the directory of the program. In this case, the directory is <code>central/</code>.

To get a good execution of the whole project, the execution of each device follows an order: the first to be executed are the helper devices and then the central. If there are some problem with the execution, press the <i>RST</i> button on the ESPs, first the helper one and then the central one.

<h2>Software analysis</h2>

Central software starts with the initialization of part of the element required: wifi, esp-now and sensors. Mqtt unit will be initialized only when device has to trasmit the data to the broker. 

After initiliazations, central starts the communication with helper device with the goal of exchanging the MAC address: central communicates its address in broadcast, helper takes that MAC and it replies to the center on its address. After sending the message with the function <code>esp_now_tx()</code> (general function to trasmit the data with esp now), central device will wait the reply from the helper (<code>esp_now_rx()</code>, function that manages all the messages received with esp now). When helper MAC is correctly received by the central, the execution goes in an endless loop (the main part).

The first step of the loop is the people detection: <code>check_people()</code> is the function that returns <code>true</code> if it gets an high value from the microphone, otherwise it returns <code>false</code>. In this last case, central communicates to helper that nobody has been detected (it sends a message with payload "0"), it goes to sleep (<code>esp_light_sleep_start()</code>) and when the device wakes up it restarts the loop, checking the people in the room.

If a noise has been detected, central communicates to helper to start the data sampling (message with payload "1"), it waits that data from helper and then it samples its data with function <code>air_detection()</code>. When central has its data and helper's data, it proceeds with <code>elaboration()</code> that gives the data average. Average is communicated to the helper and to the mqtt broker. When these communications have been completed, central device goes to sleep.

