#include "bleep_base.h" //Contains functions we'll need when using the bleep base


unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
float hue;
float bright;
float sat;
int mode = 0;
float random1;

void setup() {
  start_bleep_base();

} //setup is over



void loop() {
  current_time = millis();


  for (int i = 0; i < 8; i++)  {
    buttons[i].update();
  }

  if (buttons[0].fell()) {
    mode = mode + 1;
    if (mode > 2) {
      mode = 0;
    }
    Serial.println(mode);
  }
  if (buttons[1].fell()) {
    mode = 1;
  }
  if (buttons[2].fell() == 0) {
    mode = 2;
  }

  if (buttons[0].rose()) {

  }
  if (buttons[0].read() == 0) {

  }
  hue = potRead(0); //0-1.0
  bright = potRead(1); //0-1.0
  sat = potRead(2);

  if (current_time - prev_time[0] > 33) { //33 miliseconds is about 30 Hz
    prev_time[0] = current_time;

    if (mode == 0) {
      set_LED(0, hue, sat, bright);
      set_LED(1, (hue + .2), sat, bright);
    }

    if (mode == 1) {
      random1 = random(100) / 99.0;
      set_LED(0, random1, sat, bright);
      set_LED(1, random1, sat, bright);
    }

    if (mode == 2) {
      random1 = random(100) / 99.0;
      set_LED(0, 1, sat, random1);
      set_LED(1, 1, sat, random1);
    }

    LEDs.show(); // after we've set what we want all the LEDs to be we send the data out through this function
  }
}
