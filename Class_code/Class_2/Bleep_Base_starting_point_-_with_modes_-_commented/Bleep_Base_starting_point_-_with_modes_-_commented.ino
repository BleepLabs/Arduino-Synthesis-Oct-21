//using he buttons to change between different LED modes

#include "bleep_base.h" //Contains functions we'll need when using the bleep base

unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
float hue;
float bright;
float saturation;
int mode = 0;
float random1;

void setup() {
  start_bleep_base();  //must be called in setup

} //setup is over


void loop() {
  current_time = millis();

  for (int i = 0; i < 8; i = i + 1)  { //execute the code inside the {} 8 times while incrementing i from 0-7
    buttons[i].update(); //update all the buttons so fell, rose, and state work
  }

  if (buttons[0].fell()) { //was button 0 not pressed the last loop but this loop it is being pressed?
    mode = mode + 1;
    if (mode > 2) {
      mode = 0;
    }
    //mode will go from 0-2
    Serial.println(mode); //println prints a new line after the variable
  }

  if (buttons[1].fell()) {
    mode = 1;
  }
  if (buttons[2].fell()) {
    mode = 2;
  }

  if (buttons[0].rose()) { //was button 0 being pressed the last loop but this loop it is not?

  }

  if (buttons[0].read() == 0) { //is the button current pressed or not. 0 is pressed,  1 is not pressed

  }

  hue = potRead(0); //returns 0-1.0
  bright = potRead(1);
  saturation = potRead(2);

  if (current_time - prev_time[0] > 33) { //33 milliseconds is about 30 Hz
    prev_time[0] = current_time;

    //set_LED(led to change, hue,saturation,value aka brightness)
    // led to change is 0-63
    // all other are 0.0 to 1.0
    // hue - 0 is red, then through the ROYGBIV to 1.0 as red again
    // saturation - 0 is fully white, 1 is fully colored.
    // value - 0 is off, 1 is the value set by max_brightness
    // (it's not called brightness since, unlike in photoshop, we're going from black to fully lit up

    if (mode == 0) { //== is compare. are these two values the same?
      set_LED(0, hue, saturation, bright);
      set_LED(1, hue + .6, saturation, bright);
    }
    if (mode == 1) {
      random1 = random(100) / 99.0;  //0-1.0
      set_LED(0, random1, saturation, bright);
      set_LED(1, random1, saturation, bright);
    }
    if (mode == 2) {
      set_LED(0, 1, saturation, 0);
      set_LED(1, 1, saturation, 0);
    }

    LEDs.show(); // after we've set what we want all the LEDs to be we send the data out through this function
  }


}// loop is over
