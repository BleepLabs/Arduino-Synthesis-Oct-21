//Class 2 starting point for using the bleep base controls and LED
// info on all the functions here https://github.com/BleepLabs/Arduino-Synthesis-Oct-21/wiki/Class-2-cheat-sheet

#include "bleep_base.h" //Contains functions we'll need when using the bleep base

unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
float hue;
float bright;

void setup() {
  start_bleep_base();

} //setup is over


void loop() {
  current_time = millis();

  for (int i = 0; i < 8; i = i + 1)  {
    buttons[i].update();
  }

  for (int i = 0; i < 8; i = i + 1)  {
    if (buttons[i].fell()) {
      //Serial.println("button 0 fell");
      Serial.print(i);
      Serial.println(" fell!");
    }
  }

  if (buttons[0].rose()) {

  }
  if (buttons[0].read() == 0) { //0 is pressed,  1 is not pressed

  }

  hue = potRead(0); //0-1.0
  //hue = 1.0-(analogRead(A10)/1023.0);
  bright = potRead(1); //0-1.0

  if (current_time - prev_time[0] > 33) { //33 miliseconds is about 30 Hz
    prev_time[0] = current_time;

    //set_LED(led to change, hue,saturation,value aka brightness)
    // led to change is 0-63
    // all other are 0.0 to 1.0
    // hue - 0 is red, then through the ROYGBIV to 1.0 as red again
    // saturation - 0 is fully white, 1 is fully colored.
    // value - 0 is off, 1 is the value set by max_brightness
    // (it's not called brightness since, unlike in photoshop, we're going from black to fully lit up

    set_LED(0, hue, 1, bright);
    set_LED(1, hue + .6, 1, bright);
    LEDs.show(); // after we've set what we want all the LEDs to be we send the data out through this function
  }


}// loop is over
