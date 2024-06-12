<h1>What's the mood - CENTRAL DEVICE</h1>
<i>Central device</i> is the main node of this IoT system. It's the bridge between the IoT network and the external broker. It manages the execution of the <i>helpers device</i>, giving them the instructions for starting the data sampling and the general result of all data sampled on the system. This result is evalueted computing the average of the data sampled by all the system devices. The average is shared to helper devices and a mqtt broker (<i>HiveMQ</i>).

<h2>Sensors</h2>

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

<h2>Hardware configuration</h2>

