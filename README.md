# Rotary phone to MQTT

In this project I made the old rotary phone smart. It sends the rotated numbers to an MQTT server. Later this information can be used to automate everything.

https://user-images.githubusercontent.com/15798619/230387954-5b1b0422-ebbc-4f4e-9e12-549ca0c81e21.mp4


The project based on this library
https://github.com/markfickett/Rotary-Dial

It counts the pulses of the phone then retuns with the result.

## How to use

- After wiring the right wires to the ESP32 microcontorller, you need to upload the compiled code to the board.
- At the first start you can connect to the offered WiFi AP of the board (name: rotaryphone).
- You can reach the Web interface on the IP of the board: 192.168.4.1
- set the WiFi and the MQTT connection on the web interface
- after save it will reboot and connects to the Wifi and MQTT
- Rotated numbers will be sent over MQTT

After a dial there is a 2 secs timeout before sending out the number. It makes possible to dial longer than 1 digit numbers.

TODOs:
- make available to have some sound in the handset (for ex. mp3)
- read the onhook/offhook events
- trigger to ring the phone over MQTT
