# Object avoider
This project allows you to build a self-driven robot that avoids objects and detects when it get stuck on obstacles making use of the accelerometer.

## Components
* Arduino Uno Rev 3
* Arduino Motor Shield
* Servo Motor (SG90 TowerPro)
* DC Motor x2
* ADXL335 - Accelerometer
* HC-SR04 - Ultra Sonic sensor
* DIY Car chassis

## Circuit sketch
![Circuit Sketch](https://github.com/MOctavio/Avoider/blob/master/circuit_sketch_bb.png?raw=true "Circuit Sketch")
Circuit image developed using [Fritzing](http://www.fritzing.org/).

### Notice
To make this work accordingly to the circuit displayed above, it's required to change the order on the servo plug so that it can be used in the _out_ of the Arduino Motor Shield.
