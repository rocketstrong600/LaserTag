# LaserTag
<p>This Project uses ESP32-Arduino</p>
<p>This is a ongoing project for an implementaion of milestag proticol for a esp32 microcontroller. Transmit is working on a esp32 using the rmt driver</p>

<p>http://www.lasertagparts.com/mtformat-2.htm</P>

<p>IF YOU ARE USING THE LIBRARY IN RELEASE CHANGE #define CDEBUG 0 to #define CDEBUG 1 in MilesTag.cpp. this enable the frequency generator for ir.</p>
<p> configTx.tx_config.carrier_freq_hz = 56000 in MilesTag.cpp. is the ir frequency.</p>
<p>Basic Use can be seen in main.cpp</p>
# Progress
<p>TODO: Structure Recieve Code, Scores, Health, Add Board Schematics</p>
<p>Done: ir Comms and error Checking</p>
<p>Redesigning for ESP32 With Audio Chip Expect Major Code Changes</p>
