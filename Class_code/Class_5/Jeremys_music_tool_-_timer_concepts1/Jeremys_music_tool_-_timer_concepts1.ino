//proof of concept for using timers and setting bpms of things
//LEDs flash on the 1s of two 4/4 sequences
//pot0 chages the rate of the right led
//when you hit button0 the rate of the right led will interpolate to 120, the speed of the left one
// but I need to find a good way to reset the phase so they are in usison again

IntervalTimer timer1; //acurate timers https://www.pjrc.com/teensy/td_timing_IntervalTimer.html
IntervalTimer timer2;

#include "bleep_base.h" //Contains functions we'll need when using the bleep base

unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
float hue;
float bright;
int timer1_output, timer2_output;
int led_flag[5];
unsigned long led_time[5];
int led_on_length = 100; //millis to stay on for
float timer_bpm[5];
float prev_pot0, pot0;
int lerp_enable[5];
float lerp_state[5];
float timer2_desired_bpm = 120; //waht it will return to when you hit button 0
int reset_phase[5];
void setup() {
  start_bleep_base();

  timer1.begin(timer1_function, bpm2us(120, 2)); //two divisions since I want it to blink once per measure. If it's just one its on for a measure and off for a measure
  timer2.begin(timer2_function, bpm2us(timer2_desired_bpm, 2));

} //setup is over

void timer1_function() {
  timer1_output = !timer1_output;
  led_time[1] = millis();
  led_flag[1] = 1;
}

void timer2_function() {
  timer2_output = !timer2_output;
  led_time[2] = millis();
  led_flag[2] = 1;
}


void loop() {
  current_time = millis();

  for (int i = 0; i < 8; i = i + 1)  {
    buttons[i].update();
  }
  if (buttons[0].fell()) {
    lerp_enable[2] = 1;
  }



  if (current_time - prev_time[1] > 2) {
    prev_time[1] = current_time;

    if (lerp_enable[2] == 1) {
      if (lerp_state[2] < timer2_desired_bpm) {
        lerp_state[2] += .25;
      }
      if (lerp_state[2] > timer2_desired_bpm) {
        lerp_state[2] -= .25;
      }
      //is is close to the desired bpm
      if (lerp_state[2] < timer2_desired_bpm + 1 && lerp_state[2] > timer2_desired_bpm - 1) {
        lerp_state[2] = timer2_desired_bpm;
        timer_bpm[2] = timer2_desired_bpm;
        lerp_enable[2] = 0;
        reset_phase[2]=1; //idk how to do this yet. Can't end and begin a timer inside another timers function hmmm
      }
      timer2.update(bpm2us(lerp_state[2], 2));
    }

    prev_pot0 = pot0;
    pot0 = potRead(0);
    if (pot0 > prev_pot0 + .001 || pot0 < prev_pot0 - .001) { // || means or. Were checking to see if it moved
      timer_bpm[2] = (pot0 * 100) + 100;
      lerp_state[2] = timer_bpm[2];
      Serial.print("pot0 moved. bpm:");
      Serial.println(timer_bpm[2]);
      timer2.update(bpm2us(timer_bpm[2], 2));
    }
  }



  if (led_flag[1] == 1 && current_time - led_time[1] > led_on_length) {
    led_flag[1] = 0;
  }

  if (led_flag[2] == 1 && current_time - led_time[2] > led_on_length) {
    led_flag[2] = 0;
  }

  if (current_time - prev_time[0] > 15) {
    prev_time[0] = current_time;
    Serial.print(lerp_enable[2]);
    Serial.print(" ");
    Serial.println(lerp_state[2]);

    //set_LED(led to change, hue,saturation,value aka brightness)
    set_LED(0, 0, 0, led_flag[1]*.2);
    set_LED(1, 0, 0, led_flag[2]*.2);
    LEDs.show(); // after we've set what we want all the LEDs to be we send the data out through this function
  }


}// loop is over


//conver bpm to microseconds for timers
//in is desires bpm, divisions are how many steps you want
float bpm2us (float in, int division) {

  float bpm2hz = in / (240.0); //not 60, 4* that
  float step_size = bpm2hz * division;
  float hz2us = (1.0 / step_size) * 1000000.0;
  return hz2us;
}
