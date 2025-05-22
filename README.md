# ESP32-BLE-with-Xbox-controller
XIAO ESP32-C6 connected via BLE (Bluetooth Low Energy) to an Xbox Wireless Controller for controlling a toy car.
Note: This project is still in continuous development and integration (CD/CI) as of November 2024.

# Project Overview
I wanted to create a project to control a car, tank, or rover with my Xbox controller. The task sounds simple, but like many things, it has its challenges. I was already familiar with ESP microcontrollers, which come with built-in WiFi and Bluetooth features. Since my Xbox controller uses Bluetooth to communicate with games, I thought, why not combine the two?

# Parts
This project requires few basic components:
1x ESP32 (any ESP module will work; I’m using the XIAO ESP32-C6)
1x Xbox Wireless Controller, model 1708 or newer (mine is model 1914)
1x Car, tank, or rover.

# XIAO ESP32-C6
I chose this board because it uses a USB-C connector, which simplifies power, configuration, and serial connection. It’s a small board with just the right amount of connectivity for this project.
Note: The XIAO ESP32-C6 only supports BLE (Bluetooth Low Energy), so it’s incompatible with standard Bluetooth.
https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/

# Xbox Wireless Controller
I used my existing Xbox controller, model 1914, which supports BLE connectivity as a HID (Human Interface Device). Any BLE-compatible controller that follows the GATT protocol for HID should work. In particular, older Microsoft controllers, like models 1708 and 1797, should also be compatible with the latest firmware updates.

# Car Kit
I initially used a simple, inexpensive car kit from AliExpress. What a mistake! Instead, I recommend printing this design: https://www.thingiverse.com/thing:2800657 and purchasing 4x TT motors with transmission and wheels. This will likely be cheaper, and the construction will be sturdier than the cheapest car kit.
Each TT motor requires 2x M3 bolts and 2x M3 nuts. Don’t forget the motor driver. I’m using a DRV8833 (though mine does not support PWM functionality).

# Software 
The hardware was easy to assemble, but getting the ESP32 to work with the controller was the challenging part. Most BLE devices don’t pair or bond in the traditional sense; instead, they rely on subscriptions for communication.
