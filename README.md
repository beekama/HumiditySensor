# HumiditySensor
Arduino-Code for a humidity and temperature sensor. Measurements are sent to my Raspberry Pi running the MQTT broker mosquitto. </br>
If a display is connected, the current measurement data is also displayed there.

## Requirements
### Hardware
* ESP8266 Wifi-microchip
* DHT 11  temperature and humidity sensor
* (SSD 1306 OLED-display)
* Raspberry-Pi or Server (with Mosquitto running)

### Software
* Arduino-IDE

## Further Processing
On my Raspberry Pi I process my data via telegraf, influxdb and grafana. Grafana provides me with a visual dashboard and allows me to easily send telegram notifications as soon as a set threshold is exceeded.
