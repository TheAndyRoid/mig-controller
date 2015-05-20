##MIG Controller

The purpose of this project is to control a traditional transformer based mig welder and add "useless" features.

It has "useless" features such as: 
* Voltage Read Out
* Current Read Out
* Spot Weld Control
* Pulse Weld Control
* Wire feed units <sup>1</sup>
* Run in speed
* Burn back
* On delay
* Off delay
* Gas pre flow
* Gas post flow
* Motor reverse
* Persistant settings
* Thermal cut off

The majority of above are customizable through the display menus and rotary encoder.

Main hardware requirments:
* Arduino Uno or other compatable micro controller
* H-Bridge motor driver 3 pin control
* HD44780 16x2 Display
* Solid State Relay
* Push button rotary encoder
* Power Supplies for H-bridge driver and microcontroller 

<sup>1</sup> Full configuration of the wirefeeder will require testing and reprogrammnig of the microcontroller.

