# Wi-Motory (Wireless Motory)
A simple example demonstrating motor control through an ESP32 microcontroller over the Wi-Fi Radio using MQTT communication Protocol.

Motor Driver for the four brushed dc wheels is a single L293D motor driver.

### Dependencies
- AsyncMqttClient
- ServoESP32
- FreeRTOS

### Style Guide
We make use of the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)

### MQTT Commands
The following commands can be used to control the Four-Wheel body.
```[direction]:[speed]```
Where, 
Direction (Integer):
- Forward = 1
- Backward = 2
- ClockWise = 4 (Turn Left)
- AntiClockWise = 8 (Turn Right)

Speed; Unsigned 8 bit Integer ranging from 0 to 255 (8 bit PWM)
