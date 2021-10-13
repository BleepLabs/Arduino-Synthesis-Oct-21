
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

  for (int i = 0; i < 8; i++)  {
    buttons[i].update();
  }

  if (buttons[0].fell()) {
    Serial.println("button 0 fell");
  }
  if (buttons[0].rose()) {

  }
  if (buttons[0].read() == 0) {

  }

  hue = potRead(0); //0-1.0
  bright = potRead(1); //0-1.0

  if (current_time - prev_time[0] > 33) { //33 miliseconds is about 30 Hz
    prev_time[0] = current_time;

    set_LED(0, hue, 1, bright);
    set_LED(1, hue + .6, 1, bright);
    LEDs.show(); // after we've set what we want all the LEDs to be we send the data out through this function
  }


}// loop is over
