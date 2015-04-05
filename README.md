# ecen361
There are 3 Arduino programs here. They are to be loaded onto an Arduino Uno. 
The Arduino Uno only has space for one program at a time.
In the future, we could upgrade hardware to allow the programs to be combined into 1.

For color selection, 3 potentiometers should be wired into A0, A1 and A2
Button A - pin 4
Button B - pin 5
Button Up - pin 6
Button Down - pin 7
Button Left - pin 8
Button Right - pin 9

For the LEDs, we use LPD8806 Strips.
The controller led is wired:
Data - pin 2
Clock - pin 3

The main 16 x 16 grid of LEDs is wired:
Data - A5
Clock - A4
