
#include "bleep_base.h" //Contains functions we'll need when using the bleep base

unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
float hue;
float bright;
float saturation;
int mode = 0;
int lfo;
int lfo_mode;
float random1;
float lfo_f;

void setup() {
  start_bleep_base();

} //setup is over


void loop() {
  current_time = millis();

  for (int i = 0; i < 8; i = i + 1)  {
    buttons[i].update();
  }

  if (buttons[0].fell()) {
    mode = mode + 1;
    if (mode > 2) {
      mode = 0;
    }
    Serial.println(mode); //new line after printing the input
  }

  if (buttons[1].fell()) {
    mode = 1;
  }
  if (buttons[2].fell()) {
    mode = 2;
  }

  if (buttons[0].rose()) {

  }
  if (buttons[0].read() == 0) { //0 is pressed,  1 is not pressed

  }

  hue = potRead(0); //0-1.0
  //hue = 1.0-(analogRead(A10)/1023.0);
  //lfo_f = lfo / 255.0;
  lfo_f = map(float(lfo), 0, 255, 0, 1.0); //only woks if input is a float
  bright = lfo_f * potRead(1); //0-1.0
  saturation = potRead(2);

  if (current_time - prev_time[0] > 33) { //33 miliseconds is about 30 Hz
    prev_time[0] = current_time;

    //set_LED(led to change, hue,saturation,value aka brightness)
    // led to change is 0-63
    // all other are 0.0 to 1.0
    // hue - 0 is red, then through the ROYGBIV to 1.0 as red again
    // saturation - 0 is fully white, 1 is fully colored.
    // value - 0 is off, 1 is the value set by max_brightness
    // (it's not called brightness since, unlike in photoshop, we're going from black to fully lit up

    if (mode == 0) {
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


  if (current_time - prev_time[1] > 10) {
    prev_time[1] = current_time;
    Serial.println(lfo);

    if (lfo_mode == 1) {
      lfo += 1;
    }
    if (lfo_mode == 0) {
      lfo -= 4;
    }

    if (lfo > 254) {
      lfo_mode = 0;
    }

    if (lfo < 1) {
      lfo = 1;
      lfo_mode = 1;
    }

  }

}// loop is over
