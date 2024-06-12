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


<h2>Sensors analysis</h2>

Central has sensors and leds. It hasn't actuators because only the helpers intercat directly with the environment. Sensors used by central are:

- <b>a microphone</b>, with which the device checks if there is someone inside the classroom;
- <b>an air module</b>, the hardware used to get air data;
- <b>a thermistor</b>, a resistence that changes its value with room temperature.

More informations of these sensors are on <code>README.md</code> of repository homepage.

Microphone value is taken on it's <i>digital output</i>, that will give an <i>high value</i>(1) if it detects a noise over a specific decibel value, otherwise it will give a <i>lower value</i> (0).

Air module has to give values on air quality, with an high range of values. For this reason, the output is taken on <i>analogic output</i> of the module.

//THERMISTOR, WAIT LOLLO

The leds report a visual average of data sampled. They are four, two for air values and two for temperature:

- <b>green led</b> indicates a good qaulity of the air;
- <b>yellow led</b> indicates a bad quality of the air;
- <b>red led</b> indicates a high temeperature detected;
- <b>blue led </b> indicates a òpw temperature detected.

Led status will change each time data sampling is done.



