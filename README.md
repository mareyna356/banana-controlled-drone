# banana-controlled-drone
My final project for my Microprocessors and Microcontrollers course during my 5th semester at the Autonomous University of Baja California (September - December, 2020): a mini drone controlled by bananas.

Partly inspired by WesleyCMD's "[Mind Control Drone](https://projecthub.arduino.cc/WesleyCMD/mind-control-drone-4b6e4d)" project. Huge thanks to him for the instructions on how to hack the drone controller and connect it to the Arduino through a low-pass RC filter.

The circuit schematic can be seen in [***Circuit.png***](Circuit.png).  
The Arduino code used in this project is in [***BananaDrone.ino***](BananaDrone.ino).  
This code requires the [Wire](https://www.arduino.cc/reference/en/language/functions/communication/wire/) and [Adafruit_MPR121](https://github.com/adafruit/Adafruit_MPR121/tree/master) libraries.

Tools used:
- Holy Stone HS210 Mini Drone RC
- Adafruit MPR121 (capacitive sensor)
- Arduino Uno
- A power supply capable of supplying 4.5V (the drone's controller uses 3 1.5V batteries, so it needs 4.5V total)
- Breadboard
- 4 100μF capacitors
- 4 220Ω resistors
- 4 5.1kΩ resistors
- Male to male jumper wires
- Alligator clips
- Soldering kit (soldering iron & solder wire)
- Desoldering pump
- Screwdriver
- Multimeter
- Oscilloscope
- 8 bananas for basic movement + 4 more bananas for optional automated tricks

Procedure:  
- First, I used a screwdriver to open up the HS210 Mini Drone's controller to expose its PCB. Then, with the controller powered up, I used a multimeter to measure the low, middle and high voltages of each axis of each joystick's potentiometer. With those voltages written down, I then powered down the controller and extracted the PCB. Afterwards, I desoldered the joysticks' potentiometers from the PCB. Using a multimeter, I measured the potentiometers' base resistances on both axes and obtained resistors of equal values; my 4 readings were of 5.01kΩ, but since I couldn't find resistors of exactly 5.01kΩ, I obtained 4 5.1kΩ resistors (the closest I could find to 5.01kΩ). I then soldered those resistors into the + and - holes where the potentiometers used to be to re-complete the circuit and I also soldered jumper wires into each signal hole where the potentiometers used to be; those signal wires are what allow me to simulate the movement of the joysticks using the Arduino's analogWrite() function. Each signal wire controls the different movements of the drone: one controls the throttle, one controls the yaw, one controls the roll, and the last one controls the pitch. I also had to solder wires into the + and - power holes of the PCB to supply it with voltage from the external power supply; since the controller uses 3 1.5V batteries, I need to supply it with 4.5V.  
- Because the controller PCB only takes in continuous voltages, it is necessary to construct a low-pass filter that converts the Arduino's PWM signals into continuous signals; I constructed 4 low-pass RC filters, one for each signal wire of the PCB, on a breadboard using 4 100μF capacitors and 4 220Ω resistors (these values I obtained from WesleyCMD's previously mentioned [project](https://projecthub.arduino.cc/WesleyCMD/mind-control-drone-4b6e4d)). Each filter's input is wired to a digital port of the Arduino capable of PWM output. I then used an oscilloscope to test the output of the filters; the 4 filters were successfully outputing continuous signals. I then wired each filter's output to a signal hole of the controller PCB's joysticks using alligator clips.  
- Next, I wired the MPR121 to the Arduino: the MPR121's SCL pin is wired to the Arduino's analog pin 5, the MPR121's SDA pin is wired to the Arduino's analog pin 4, the MPR121's power pin is wired to the Arduino's 3.3V pin, and the MPR121's GND pin is wired to the Arduino's GND pin (the entire system of course shares the same ground). All of the MPR121's 12 input pins (pin 0 - 11) are wired to bananas using alligator clips. Now the system is completed.  
- Finally, I wrote the code for the Arduino using as starter code the example file [MPR121test.ino](https://github.com/adafruit/Adafruit_MPR121/blob/master/examples/MPR121test/MPR121test.ino) from the Adafruit_MPR121 library. Using the low, middle and high voltages I had measured from the joysticks' potentiometers at the beginning as references, I did several tests to determine the appropriate values for the several analogWrite() functions I had to use. While 8 bananas are enough to fully control the drone, I decided to use 4 more bananas in the remaining 4 pins of the MPR121 for automated tricks. The banana-controller works as follows:

Basic movements
- Pin 0 - Throttle up
- Pin 1 - Throttle down
- Pin 2 - Yaw right
- Pin 3 - Yaw left
- Pin 4 - Roll right
- Pin 5 - Roll left
- Pin 6 - Pitch forward
- Pin 7 - Pitch backwards

Automated tricks
- Pin 8 - Horizontal zigzag
- Pin 9 - Vertical zigzag
- Pin 10 - Vertical flying in an 8 shape
- Pin 11 - Vertical flying in a circle

Video of the drone in action:
https://drive.google.com/file/d/1NmOQ4WA6EaTchputYUUQdnm8fJbHGoa8/view?usp=sharing
